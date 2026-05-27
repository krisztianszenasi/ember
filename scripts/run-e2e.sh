#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_PYTHON="$ROOT_DIR/.venv/bin/python"

if [ -x "$VENV_PYTHON" ]; then
    PYTHON="$VENV_PYTHON"
elif command -v python3 >/dev/null 2>&1; then
    PYTHON="python3"
else
    echo "error: python3 is required. Run scripts/setup-python.sh first." >&2
    exit 1
fi

cd "$ROOT_DIR"
exec "$PYTHON" tests/end2end/run.py "$@"
