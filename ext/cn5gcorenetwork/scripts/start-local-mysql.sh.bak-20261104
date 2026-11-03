#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MYSQL="$ROOT/ext/mysql"
PID_FILE="$MYSQL/run/mysqld.pid"
SOCKET="$MYSQL/run/mysqld.sock"
LOG="$MYSQL/logs/error.log"
RUN_USER="${OAI_MYSQL_USER:-${SUDO_USER:-$(id -un)}}"
export LD_LIBRARY_PATH="$MYSQL/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

if [[ -s "$PID_FILE" ]] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
  echo "Project-local MySQL is already running (PID $(cat "$PID_FILE"))."
  exit 0
fi
[[ -x "$MYSQL/bin/mysqld" ]] || { echo "ERROR: missing $MYSQL/bin/mysqld" >&2; exit 1; }
[[ -d "$MYSQL/data/mysql" ]] || { echo "ERROR: project MySQL data directory is not initialized" >&2; exit 1; }
if ss -ltn 2>/dev/null | grep -qE '127\.0\.0\.1:3306[[:space:]]'; then
  echo 'ERROR: TCP port 3306 is occupied; stop the system mysql/mariadb service first.' >&2
  exit 1
fi
mkdir -p "$MYSQL/run" "$MYSQL/logs"
rm -f "$SOCKET" "$PID_FILE"
nohup "$MYSQL/bin/mysqld" --no-defaults \
  --basedir="$MYSQL" --datadir="$MYSQL/data" --plugin-dir="$MYSQL/plugin" \
  --lc-messages-dir="$MYSQL/share" --user="$RUN_USER" \
  --socket="$SOCKET" --pid-file="$PID_FILE" --log-error="$LOG" \
  --bind-address=127.0.0.1 --port=3306 --mysqlx=OFF \
  >"$MYSQL/logs/stdout.log" 2>&1 </dev/null &
for _ in {1..30}; do
  [[ -s "$PID_FILE" ]] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null &&
    ss -ltn 2>/dev/null | grep -qE '127\.0\.0\.1:3306[[:space:]]' && {
      echo "Project-local MySQL started (PID $(cat "$PID_FILE"))."
      exit 0
    }
  sleep 1
done
tail -n 40 "$LOG" >&2 || true
echo 'ERROR: project-local MySQL did not become ready within 30 seconds' >&2
exit 1
