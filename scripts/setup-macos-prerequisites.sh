#!/usr/bin/env bash
set -euo pipefail

brew install premake bison flex

if [ -d /opt/homebrew/opt/bison/bin ]; then
    echo "/opt/homebrew/opt/bison/bin" >> "$GITHUB_PATH"
    echo "/opt/homebrew/opt/flex/bin" >> "$GITHUB_PATH"
fi

if [ -d /usr/local/opt/bison/bin ]; then
    echo "/usr/local/opt/bison/bin" >> "$GITHUB_PATH"
    echo "/usr/local/opt/flex/bin" >> "$GITHUB_PATH"
fi

export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/flex/bin:/usr/local/opt/bison/bin:/usr/local/opt/flex/bin:$PATH"

premake5 --version
bison --version
flex --version
