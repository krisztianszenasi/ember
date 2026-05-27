#!/usr/bin/env bash
set -euo pipefail

PREMAKE_VERSION="5.0.0-beta7"
PREMAKE_INSTALL_MODE="${PREMAKE_INSTALL_MODE:-auto}"
PREMAKE_BINARY_ARCHIVE="premake-${PREMAKE_VERSION}-linux.tar.gz"
PREMAKE_BINARY_URL="https://github.com/premake/premake-core/releases/download/v${PREMAKE_VERSION}/${PREMAKE_BINARY_ARCHIVE}"
PREMAKE_SOURCE_ARCHIVE="premake-${PREMAKE_VERSION}-src.zip"
PREMAKE_SOURCE_URL="https://github.com/premake/premake-core/releases/download/v${PREMAKE_VERSION}/${PREMAKE_SOURCE_ARCHIVE}"

detect_default_premake_mode() {
    case "$(uname -m)" in
        x86_64|amd64)
            echo "binary"
            ;;
        aarch64|arm64)
            echo "source"
            ;;
        *)
            echo "source"
            ;;
    esac
}

install_premake_binary() {
    local tmp_dir="$1"

    curl -fsSL -o "$tmp_dir/$PREMAKE_BINARY_ARCHIVE" "$PREMAKE_BINARY_URL"
    tar -xzf "$tmp_dir/$PREMAKE_BINARY_ARCHIVE" -C "$tmp_dir"
    sudo install -m 0755 "$tmp_dir/premake5" /usr/local/bin/premake5
}

install_premake_source() {
    local tmp_dir="$1"

    curl -fsSL -o "$tmp_dir/$PREMAKE_SOURCE_ARCHIVE" "$PREMAKE_SOURCE_URL"
    unzip -q "$tmp_dir/$PREMAKE_SOURCE_ARCHIVE" -d "$tmp_dir/premake-src"
    make -C "$tmp_dir/premake-src/build/gmake.unix" Premake5 config=release
    sudo install -m 0755 "$tmp_dir/premake-src/bin/release/premake5" /usr/local/bin/premake5
}

if [ "$PREMAKE_INSTALL_MODE" = "auto" ]; then
    PREMAKE_INSTALL_MODE="$(detect_default_premake_mode)"
fi

sudo apt update
sudo apt install -y build-essential make flex bison ca-certificates curl tar unzip uuid-dev

tmp_dir="$(mktemp -d)"
trap 'rm -rf "$tmp_dir"' EXIT

case "$PREMAKE_INSTALL_MODE" in
    binary)
        install_premake_binary "$tmp_dir"
        ;;
    source)
        install_premake_source "$tmp_dir"
        ;;
    *)
        echo "Invalid PREMAKE_INSTALL_MODE: $PREMAKE_INSTALL_MODE" >&2
        echo "Expected: auto, binary, or source" >&2
        exit 1
        ;;
esac

premake5 --version
bison --version
flex --version
