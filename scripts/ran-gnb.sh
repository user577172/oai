#!/usr/bin/env bash
# A-board B210 RF launcher for the RVV OAI build and local 5G core.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CORE="$ROOT/ext/cn5gcorenetwork"
ENV_FILE="${K3_RF_ENV_FILE:-$CORE/config/rf.env}"
BUILD="${K3_OAI_BUILD:-$ROOT/cmake_targets/ran_build/rv64-rvv-release}"
GNB="$BUILD/nr-softmodem"
TEMPLATE="${K3_GNB_TEMPLATE:-$CORE/config/gnb-a-rvv.conf}"
RUNTIME="$CORE/run/gnb-b210.conf"
UNIT="${K3_GNB_UNIT:-oai-k3-b210-gnb.service}"
LOG_DIR="$ROOT/log"
GNB_LOG="$LOG_DIR/gnb.log"
die() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }
usage() {
  printf 'Usage: %s {check|start|recover|restart|status|log|stop|stop-all}\n' "$0"
  printf 'Set USRP_SERIAL in %s. start/restart/recover enable RF transmission.\n' "$ENV_FILE"
}
active() { sudo systemctl is-active --quiet "$UNIT"; }
core_ready() {
  local status
  status=$(sudo "$CORE/scripts/status-cn5g.sh" 2>/dev/null) || return 1
  pgrep -x upf >/dev/null && pgrep -x smf >/dev/null && pgrep -x amf >/dev/null &&
    ip -4 addr show cn5g-gnb 2>/dev/null | grep -Fq "${GNB_N2_IP%%/*}" &&
    [[ "$status" == *'N4: associated'* && "$status" == *'N2/SCTP 38412: listening'* ]]
}
ensure_core() {
  local attempt
  core_ready && return 0
  printf '5G core is absent or incomplete; cleaning stale state before startup.\n'
  sudo "$CORE/scripts/stop-cn5g.sh" || true
  sudo "$CORE/scripts/start-cn5g.sh"
  for attempt in {1..10}; do
    core_ready && return 0
    sleep 1
  done
  die '5G core did not become ready within 10 seconds (N4/N2 check failed)'
}
load_env() {
  [[ -r "$ENV_FILE" ]] || die "Missing $ENV_FILE; copy rf.env.example to rf.env."
  # shellcheck disable=SC1090
  source "$ENV_FILE"
  [[ -n "${USRP_SERIAL:-}" ]] || die "Set USRP_SERIAL in $ENV_FILE."
  AMF_IP="${AMF_IP:-$(awk -F= '$1=="HOST_IP" {print $2}' "$CORE/config/runtime.env")}" 
  GNB_N2_IP="${GNB_N2_IP:-$AMF_IP}"
  GNB_N3_IP="${GNB_N3_IP:-$(awk -F= '$1=="GNB_N3_IP" {print $2}' "$CORE/config/runtime.env")}"
  MCC="${MCC:-262}"
  MNC="${MNC:-99}"
  TAC="${TAC:-1}"
  MIN_RXTXTIME="${K3_GNB_MIN_RXTXTIME:-12}"
  SL_AHEAD="${K3_GNB_SL_AHEAD:-6}"
  [[ "$MIN_RXTXTIME" =~ ^[0-9]+$ ]] || die "K3_GNB_MIN_RXTXTIME must be an integer"
  [[ "$SL_AHEAD" =~ ^[0-9]+$ ]] || die "K3_GNB_SL_AHEAD must be an integer"
}
check() {
  load_env
  [[ "$(uname -m)" == riscv64 ]] || die "Expected riscv64 K3 host."
  [[ -x "$GNB" ]] || die "Missing $GNB; run scripts/build-oai-rvv.sh (RVV) or scripts/build-oai-no-v.sh (no-V) first."
  [[ -f "$BUILD/libparams_libconfig.so" ]] || die "Missing $BUILD/libparams_libconfig.so; rebuild the runtime config plugin."
  [[ -r "$TEMPLATE" ]] || die "Missing B210 template: $TEMPLATE"
  command -v uhd_find_devices >/dev/null || die "uhd_find_devices is unavailable."
  command -v uhd_usrp_probe >/dev/null || die "uhd_usrp_probe is unavailable."
  ip -4 addr show | grep -Fq "${GNB_N2_IP%%/*}" || die "GNB N2 address not assigned locally: $GNB_N2_IP"
  if active; then
    # UHD does not enumerate a B210 that is already exclusively owned by the
    # running nr-softmodem.  Treat a live service with a real MainPID as the
    # authoritative device check in this state.
    local main_pid
    main_pid=$(sudo systemctl show "$UNIT" -p MainPID --value)
    [[ "$main_pid" =~ ^[1-9][0-9]*$ && -d "/proc/$main_pid" ]] ||
      die "$UNIT claims to be active but has no live MainPID"
    printf 'B210 is owned by active %s (PID %s); skip idle UHD enumeration.\n' "$UNIT" "$main_pid"
  else
    uhd_find_devices 2>&1 | grep -Fq "$USRP_SERIAL" || die "B210 serial $USRP_SERIAL was not found by UHD."
  fi
  [[ -x "$CORE/scripts/start-cn5g.sh" && -x "$CORE/scripts/status-cn5g.sh" ]] || die "Bundled core scripts are incomplete."
  printf 'Preflight passed: gNB=%s AMF=%s N2=%s N3=%s USRP=%s PLMN=%s/%s TAC=%s\n' \
    "$GNB" "$AMF_IP" "$GNB_N2_IP" "$GNB_N3_IP" "$USRP_SERIAL" "$MCC" "$MNC" "$TAC"
}
write_config() {
  mkdir -p "$CORE/run" "$LOG_DIR"
  cp "$TEMPLATE" "$RUNTIME"
  sed -Ei \
    -e "s/(tracking_area_code[[:space:]]*=[[:space:]]*)[^;]+;/\\1$TAC;/" \
    -e "s/(mcc[[:space:]]*=[[:space:]]*)[0-9]+;/\\1$MCC;/" \
    -e "s/(mnc[[:space:]]*=[[:space:]]*)[0-9]+;/\\1$MNC;/" \
    -e "s#(amf_ip_address.*ipv4[[:space:]]*=[[:space:]]*)\"[^\"]+\"#\\1\"$AMF_IP\"#" \
    -e "s#(GNB_IPV4_ADDRESS_FOR_NG_AMF[[:space:]]*=[[:space:]]*)\"[^\"]+\"#\\1\"$GNB_N2_IP\"#" \
    -e "s#(GNB_IPV4_ADDRESS_FOR_NGU[[:space:]]*=[[:space:]]*)\"[^\"]+\"#\\1\"$GNB_N3_IP\"#" \
    "$RUNTIME"
  grep -Fq "ipv4 = \"$AMF_IP\"" "$RUNTIME" || die "AMF address was not written into $RUNTIME"
}
tune_usb_irq() {
  local irq old
  irq=$(awk '/xhci-hcd:usb8$/ {gsub(":", "", $1); print $1; exit}' /proc/interrupts)
  [[ -n "$irq" && -r "/proc/irq/$irq/smp_affinity_list" ]] || die "B210 xHCI IRQ was not found"
  old=$(cat "/proc/irq/$irq/smp_affinity_list")
  # Preserve the original affinity across a retry after an interrupted run.
  if [[ ! -e "$CORE/run/b210-usb-irq-affinity.prev" ]]; then
    printf '%s %s\n' "$irq" "$old" >"$CORE/run/b210-usb-irq-affinity.prev"
  fi
  printf '6\n' | sudo tee "/proc/irq/$irq/smp_affinity_list" >/dev/null
  printf 'B210 xHCI IRQ %s: CPU %s -> CPU 6\n' "$irq" "$old"
}
restore_usb_irq() {
  local irq old
  [[ -r "$CORE/run/b210-usb-irq-affinity.prev" ]] || return 0
  read -r irq old <"$CORE/run/b210-usb-irq-affinity.prev"
  if [[ -n "$irq" && -e "/proc/irq/$irq/smp_affinity_list" ]]; then
    printf '%s\n' "$old" | sudo tee "/proc/irq/$irq/smp_affinity_list" >/dev/null
  fi
  rm -f "$CORE/run/b210-usb-irq-affinity.prev"
}
pin_core_workers() {
  local name pid
  for name in mysqld upf smf amf; do
    for pid in $(pgrep -x "$name" || true); do
      sudo taskset -apc 0,6 "$pid" >/dev/null
    done
  done
}
restore_core_workers() {
  local name pid
  for name in mysqld upf smf amf; do
    for pid in $(pgrep -x "$name" || true); do
      sudo taskset -apc 0-7 "$pid" >/dev/null
    done
  done
}
case "${1:-start}" in
  check) check ;;
  start)
    # Load addresses first, then create the core-owned N2/N3 interfaces.  The
    # old order called check before start-cn5g.sh and made a clean boot fail
    # with "GNB N2 address not assigned locally".
    load_env
    if active; then
      core_ready || die "$UNIT is active but the 5G core is not ready; run '$0 recover'"
      printf 'gNB unit %s and the 5G core are already active; start is idempotent.\n' "$UNIT"
      exit 0
    fi
    ensure_core
    check
    pin_core_workers
    pgrep -x nr-softmodem >/dev/null && die "An unmanaged nr-softmodem is already running."
    write_config
    # systemd opens the file with truncate mode for each new gNB service run.
    # Do this after preflight so a failed check keeps the previous run's log.
    mkdir -p "$LOG_DIR"
    tune_usb_irq
    extra_args=()
    # Only the B reference binary supports this option. Keep it opt-in so the
    # project's own no-V and K3-V builds retain their existing command line.
    if [[ "${K3_GNB_REORDER_DISABLE:-0}" == 1 ]]; then
      extra_args+=(--reorder-thread-disable 1)
    fi
    sudo systemctl stop "$UNIT" >/dev/null 2>&1 || true
    sudo systemctl reset-failed "$UNIT" >/dev/null 2>&1 || true
    if ! sudo systemd-run --collect --unit="$UNIT" --service-type=exec --property=Restart=no \
      --property=KillMode=mixed --property=TimeoutStopSec=15 --property=LimitMEMLOCK=infinity \
      --property="StandardOutput=truncate:$GNB_LOG" --property=StandardError=inherit \
      --property=Nice=-20 --property=CPUAffinity=1-5,7 --setenv="LD_LIBRARY_PATH=$BUILD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" "$GNB" \
      -O "$RUNTIME" --RUs.[0].sdr_addrs "serial=$USRP_SERIAL" \
      --thread-pool 4,5,7 \
      --L1s.[0].L1_rx_thread_core 1 --L1s.[0].L1_tx_thread_core 3 \
      --RUs.[0].ru_thread_core 2 --usrp-tx-thread-config 1 \
      --RUs.[0].sl_ahead "$SL_AHEAD" \
      "${extra_args[@]}" \
      --gNBs.[0].min_rxtxtime "$MIN_RXTXTIME" -q \
      --log_config.global_log_options level,nocolor,time; then
      restore_usb_irq
      die "Unable to start gNB systemd unit"
    fi
    sleep 3
    if ! active; then
      restore_usb_irq
      if [[ -f "$GNB_LOG" ]]; then
        tail -n 35 "$GNB_LOG" | cut -c1-220
      else
        sudo journalctl -u "$UNIT" -n 35 --no-pager -o cat | cut -c1-220
      fi
      die "gNB exited during startup"
    fi
    printf 'gNB started as %s. Log: %s (follow with: %s log)\n' "$UNIT" "$GNB_LOG" "$0"
    ;;
  recover|restart)
    "$0" stop
    sudo "$CORE/scripts/stop-cn5g.sh" || true
    sleep 2
    sudo "$CORE/scripts/start-cn5g.sh"
    exec "$0" start
    ;;
  stop-all)
    "$0" stop
    exec sudo "$CORE/scripts/stop-cn5g.sh"
    ;;
  status)
    load_env
    sudo systemctl --no-pager --full status "$UNIT" || true
    sudo "$CORE/scripts/status-cn5g.sh" || true
    ;;
  log)
    [[ -f "$GNB_LOG" ]] || die "gNB log does not exist yet: $GNB_LOG; start gNB first."
    tail -n 50 -F "$GNB_LOG"
    ;;
  stop)
    sudo systemctl stop "$UNIT" 2>/dev/null || true
    restore_usb_irq
    restore_core_workers
    printf 'Stopped gNB unit %s; 5G core remains running.\n' "$UNIT"
    ;;
  -h|--help) usage ;;
  *) usage >&2; exit 2 ;;
esac
