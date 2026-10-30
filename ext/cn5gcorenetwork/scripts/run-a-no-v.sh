#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
export K3_OAI_BUILD="$ROOT/cmake_targets/ran_build/rv64-no-v-release"
export K3_GNB_TEMPLATE="$ROOT/ext/cn5gcorenetwork/config/gnb-a-no-v.conf"
export K3_RF_ENV_FILE="$ROOT/ext/cn5gcorenetwork/config/rf.env"
unset LD_LIBRARY_PATH

CORE_SCRIPTS="$ROOT/ext/cn5gcorenetwork/scripts"
GNB_SCRIPT="$ROOT/scripts/run-a-no-v-gnb.sh"

core_ready() {
  local status
  status=$(sudo "$CORE_SCRIPTS/status-cn5g.sh" 2>/dev/null) || return 1
  pgrep -x upf >/dev/null && pgrep -x smf >/dev/null && pgrep -x amf >/dev/null &&
    ip -4 addr show cn5g-gnb 2>/dev/null | grep -Fq 192.168.71.140 &&
    [[ "$status" == *'N4: associated'* && "$status" == *'N2/SCTP 38412: listening'* ]]
}

start_clean_core() {
  local attempt
  core_ready && return 0
  sudo "$CORE_SCRIPTS/stop-cn5g.sh" || true
  sudo "$CORE_SCRIPTS/start-cn5g.sh"
  for attempt in {1..10}; do
    core_ready && return 0
    sleep 1
  done
  echo 'ERROR: 5G core failed its N4/N2 readiness check within 10 seconds' >&2
  exit 1
}

warm_restart_gnb() {
  sleep "${K3_GNB_WARMUP_SECONDS:-8}"
  sudo "$GNB_SCRIPT" stop
  sleep "${K3_GNB_RESTART_GAP_SECONDS:-3}"
  "$GNB_SCRIPT" start
}

recover_stack() {
  # Clear stale RRC/NAS/PDU state before recreating the radio.  Starting the
  # core first also guarantees that the N2/N3 dummy interfaces exist before
  # the gNB launcher performs its preflight check.
  sudo "$GNB_SCRIPT" stop || true
  sudo "$CORE_SCRIPTS/stop-cn5g.sh" || true
  sleep 2
  sudo "$CORE_SCRIPTS/start-cn5g.sh"
  "$GNB_SCRIPT" start
  warm_restart_gnb
}

case "${1:-}" in
  start)
    if sudo systemctl is-active --quiet oai-k3-b210-gnb.service; then
      core_ready || { echo 'ERROR: gNB is active but the 5G core is not ready; use recover' >&2; exit 1; }
      echo 'gNB and 5G core are already active. Use recover to clear UE/PDU state.'
      exit 0
    fi
    start_clean_core
    "$GNB_SCRIPT" start
    # The RM520N occasionally misses the first B210 transmission after a cold
    # full-stack start.  One warm RF restart makes cell acquisition repeatable.
    warm_restart_gnb
    ;;
  restart|recover)
    recover_stack
    ;;
  stop-all)
    sudo "$GNB_SCRIPT" stop || true
    exec sudo "$CORE_SCRIPTS/stop-cn5g.sh"
    ;;
  stop|status|log|check)
    exec "$GNB_SCRIPT" "$1"
    ;;
  *)
    echo "Usage: $0 {start|restart|recover|stop|stop-all|status|log|check}" >&2
    exit 2
    ;;
esac
