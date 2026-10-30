#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MYSQL="$ROOT/ext/mysql"
PID_FILE="$MYSQL/run/mysqld.pid"
if [[ ! -s "$PID_FILE" ]]; then
  echo 'Project-local MySQL is not running.'
  exit 0
fi
pid=$(cat "$PID_FILE")
kill -TERM "$pid" 2>/dev/null || true
for _ in {1..30}; do
  kill -0 "$pid" 2>/dev/null || { rm -f "$PID_FILE" "$MYSQL/run/mysqld.sock"; echo 'Project-local MySQL stopped.'; exit 0; }
  sleep 1
done
echo "ERROR: project-local MySQL PID $pid did not stop" >&2
exit 1
