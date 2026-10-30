#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
PID_FILE="$ROOT/ext/mysql/run/mysqld.pid"
if [[ -s "$PID_FILE" ]] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
  echo "mysql: running (project-local PID $(cat "$PID_FILE"))"
  exit 0
fi
echo 'mysql: stopped'
exit 1
