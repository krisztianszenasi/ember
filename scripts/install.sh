#!/usr/bin/env bash
set -euo pipefail

CONFIG="${1:-release}"
INSTALL_DIR="${EMBER_INSTALL_DIR:-$HOME/.local/bin}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

detect_platform() {
    case "$(uname -s)" in
        Darwin)
            echo "macos"
            ;;
        Linux)
            echo "linux"
            ;;
        *)
            echo "unknown"
            ;;
    esac
}

print_homebrew_hint_if_missing() {
    if [ "$(detect_platform)" = "macos" ] && ! command -v brew >/dev/null 2>&1; then
        echo
        echo "Homebrew is not installed."
        echo "Install Homebrew from:"
        echo "  https://brew.sh/"
        echo
    fi
}

print_install_hint() {
    local command_name="$1"
    local platform="$2"

    echo
    echo "Missing required command: $command_name"
    echo

    case "$platform" in
        macos)
            case "$command_name" in
                premake5)
                    print_homebrew_hint_if_missing
                    echo "Install Premake with Homebrew:"
                    echo "  brew install premake"
                    ;;
                bison)
                    print_homebrew_hint_if_missing
                    echo "Install GNU Bison with Homebrew:"
                    echo "  brew install bison"
                    echo
                    echo "Then make sure Homebrew's Bison is first on PATH."
                    echo "Apple Silicon:"
                    echo '  export PATH="/opt/homebrew/opt/bison/bin:$PATH"'
                    echo
                    echo "Intel macOS:"
                    echo '  export PATH="/usr/local/opt/bison/bin:$PATH"'
                    ;;
                flex)
                    print_homebrew_hint_if_missing
                    echo "Install Flex with Homebrew:"
                    echo "  brew install flex"
                    echo
                    echo "Then make sure Homebrew's Flex is first on PATH."
                    echo "Apple Silicon:"
                    echo '  export PATH="/opt/homebrew/opt/flex/bin:$PATH"'
                    echo
                    echo "Intel macOS:"
                    echo '  export PATH="/usr/local/opt/flex/bin:$PATH"'
                    ;;
                make|cc)
                    echo "Install Apple command-line developer tools:"
                    echo "  xcode-select --install"
                    ;;
                *)
                    echo "Install '$command_name' and make sure it is available on PATH."
                    ;;
            esac
            ;;
        linux)
            case "$command_name" in
                premake5)
                    echo "On Ubuntu/Debian, install the download tools first:"
                    echo
                    echo "  sudo apt update"
                    echo "  sudo apt install wget tar"
                    echo
                    echo "Then install Premake 5:"
                    echo
                    echo "  wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-linux.tar.gz"
                    echo "  tar -xzf premake-5.0.0-beta7-linux.tar.gz"
                    echo "  sudo mv premake5 /usr/local/bin/"
                    echo
                    echo "If this does not work, checkout:"
                    echo "  https://premake.github.io/download/"
                    ;;
                make|cc|flex|bison)
                    echo "On Ubuntu/Debian, install the build dependencies with:"
                    echo
                    echo "  sudo apt update"
                    echo "  sudo apt install build-essential make flex bison"
                    ;;
                *)
                    echo "Install '$command_name' with your distribution's package manager."
                    ;;
            esac
            ;;
        *)
            echo "Install '$command_name' and make sure it is available on PATH."
            ;;
    esac

    echo
}

require_cmd() {
    local command_name="$1"
    local platform
    platform="$(detect_platform)"

    if ! command -v "$command_name" >/dev/null 2>&1; then
        print_install_hint "$command_name" "$platform" >&2
        exit 1
    fi
}

require_bison_version() {
    local required_major=3
    local required_minor=7

    local version
    version="$(bison --version | head -n1 | grep -Eo '[0-9]+\.[0-9]+' | head -n1)"

    if [ -z "$version" ]; then
        echo "Could not detect Bison version." >&2
        exit 1
    fi

    local major="${version%%.*}"
    local minor="${version#*.}"

    if [ "$major" -lt "$required_major" ] || {
        [ "$major" -eq "$required_major" ] && [ "$minor" -lt "$required_minor" ];
    }; then
        echo
        echo "Bison $version is too old. Ember requires Bison 3.7 or newer." >&2
        print_install_hint "bison" "$(detect_platform)" >&2
        exit 1
    fi
}

case "$CONFIG" in
    debug|release)
        ;;
    *)
        echo "Unknown configuration: $CONFIG" >&2
        echo "Expected: debug or release" >&2
        exit 1
        ;;
esac

echo "Checking dependencies..."
require_cmd premake5
require_cmd make
require_cmd cc
require_cmd flex
require_cmd bison
require_bison_version

require_third_party() {
    if [ ! -d "$ROOT_DIR/third_party" ] || [ -z "$(find "$ROOT_DIR/third_party" -mindepth 1 -maxdepth 1 2>/dev/null)" ]; then
        echo "error: third_party/ is empty. Initialize submodules first:" >&2
        echo "  git submodule update --init --recursive" >&2
        exit 1
    fi
}

echo "Using configuration: $CONFIG"
echo "Install directory: $INSTALL_DIR"

cd "$ROOT_DIR"

require_third_party

echo "Generating build files..."
premake5 gmake2

echo "Building Ember..."
make -C build "config=$CONFIG"

case "$CONFIG" in
    debug)
        BUILD_DIR="$ROOT_DIR/bin/Debug"
        ;;
    release)
        BUILD_DIR="$ROOT_DIR/bin/Release"
        ;;
esac

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build output directory not found: $BUILD_DIR" >&2
    exit 1
fi

mkdir -p "$INSTALL_DIR"

echo "Installing executables..."

for exe in ember-cli ember-vm ember-assembler; do
    if [ -f "$BUILD_DIR/$exe" ]; then
        cp "$BUILD_DIR/$exe" "$INSTALL_DIR/$exe"
        chmod +x "$INSTALL_DIR/$exe"
        echo "Installed $exe"
    fi
done

echo
echo "Done."
echo "Installed to: $INSTALL_DIR"

case ":$PATH:" in
    *":$INSTALL_DIR:"*) ;;
    *)
        echo
        echo "Warning: $INSTALL_DIR is not on your PATH."
        echo "Add this to your shell config:"
        echo
        echo "  export PATH=\"\$HOME/.local/bin:\$PATH\""
        ;;
esac
