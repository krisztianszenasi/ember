#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

IMAGE_NAME="ember-cli"
PROJECT_CONFIG="release"
PREMAKE_INSTALL_MODE="binary"

while [ "$#" -gt 0 ]; do
    case "$1" in
        --image-name)
            IMAGE_NAME="${2:?missing value for --image-name}"
            shift 2
            ;;
        --config)
            PROJECT_CONFIG="${2:?missing value for --config}"
            shift 2
            ;;
        --premake-install-mode)
            PREMAKE_INSTALL_MODE="${2:?missing value for --premake-install-mode}"
            shift 2
            ;;
        *)
            echo "error: unknown argument: $1" >&2
            echo "usage: ./scripts/install-docker.sh [--image-name <name>] [--config <debug|release>] [--premake-install-mode <binary|source>]" >&2
            exit 1
            ;;
    esac
done

case "$PROJECT_CONFIG" in
    debug|release)
        ;;
    *)
        echo "error: --config must be debug or release" >&2
        exit 1
        ;;
esac

case "$PREMAKE_INSTALL_MODE" in
    binary|source)
        ;;
    *)
        echo "error: --premake-install-mode must be binary or source" >&2
        exit 1
        ;;
esac

cd "$ROOT_DIR"
docker build \
    --build-arg PROJECT_CONFIG="$PROJECT_CONFIG" \
    --build-arg PREMAKE_INSTALL_MODE="$PREMAKE_INSTALL_MODE" \
    -t "$IMAGE_NAME" \
    .

echo
echo "Docker image built: $IMAGE_NAME"
