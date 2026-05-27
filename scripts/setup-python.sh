#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_DIR="$ROOT_DIR/.venv"

if ! command -v python3 >/dev/null 2>&1; then
    echo "error: python3 is required but was not found on PATH" >&2
    exit 1
fi

echo "Creating virtual environment at $VENV_DIR"
python3 -m venv "$VENV_DIR"

PYTHON="$VENV_DIR/bin/python"

echo "Upgrading pip"
"$PYTHON" -m pip install --upgrade pip

for requirements in \
    "$ROOT_DIR/tests/end2end/requirements.txt" \
    "$ROOT_DIR/docs/benchmarks/requirements.txt"
do
    if [ -f "$requirements" ]; then
        echo "Installing dependencies from $requirements"
        "$PYTHON" -m pip install -r "$requirements"
    fi
done

echo
echo "Python environment ready."
echo "Activate with:"
echo "  . \"$VENV_DIR/bin/activate\""
