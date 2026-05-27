<p align="center">
  <img src="./assets/ember-logo-light.png#gh-light-mode-only" alt="Ember VM logo" width="420">
  <img src="./assets/ember-logo-dark.png#gh-dark-mode-only" alt="Ember VM logo" width="420">
</p>

<p align="center">
  <img alt="Version" src="https://img.shields.io/badge/version-0.1.0-2f2f2f">
  <img alt="Executable format" src="https://img.shields.io/badge/executable%20format-v1-2f2f2f">
</p>

# Ember

## Table of contents

- [About](#about)
- [Documentation](#documentation)
- [Installation](#installation)
- [Quick start](#quick-start)
- [License](#license)

## About

Ember, short for **Efficient Machine for Bytecode Execution and Runtime**, is a
small stack-based virtual machine with its own assembler, executable format, and
command-line tooling.

The project includes:

- `ember-cli`: command-line interface for assembling and running Ember programs
- `ember-vm`: virtual machine runtime used by the CLI and runtime libraries
- `ember-assembler`: assembler implementation used by the CLI and build outputs
- `ember-core`: shared runtime, executable, value, object, and bytecode models

## Documentation

- General user documentation: [`docs/README.md`](./docs/README.md)
- Benchmarks and benchmark runner usage: [`docs/benchmarks/README.md`](./docs/benchmarks/README.md)
- End-to-end test runner usage: [`tests/end2end/README.md`](./tests/end2end/README.md)

## Installation

Ember can be built on [Linux](#linux-prerequisites), [macOS](#macos-prerequisites),
and [Windows](#windows-prerequisites). Linux and macOS share the same
[Unix install steps](#build-and-install-on-linux-and-macos), while Windows uses
its own [Windows install script](#build-and-install-on-windows).

Installation links:

- [Prebuilt release artifacts](#prebuilt-release-artifacts)
- [Linux prerequisites](#linux-prerequisites)
- [macOS prerequisites](#macos-prerequisites)
- [Windows prerequisites](#windows-prerequisites)
- [Build and install on Linux and macOS](#build-and-install-on-linux-and-macos)
- [Build and install on Windows](#build-and-install-on-windows)
- [Manual build](#manual-build)
- [Docker](#docker)

## Linux

### Prebuilt release artifacts

Tagged GitHub releases publish prebuilt archives with these names:

- `ember-linux-x64.tar.gz`
- `ember-linux-aarch64.tar.gz`
- `ember-macos-x64.tar.gz`
- `ember-macos-arm64.tar.gz`
- `ember-windows-x64.zip`
- `ember-windows-arm64.zip`

Each archive is accompanied by a matching SHA-256 checksum file:

- `ember-linux-x64.tar.gz.sha256`
- `ember-linux-aarch64.tar.gz.sha256`
- `ember-macos-x64.tar.gz.sha256`
- `ember-macos-arm64.tar.gz.sha256`
- `ember-windows-x64.zip.sha256`
- `ember-windows-arm64.zip.sha256`

### Linux prerequisites

Before building Ember on Linux, make sure the following tools are available:

- a C compiler, such as `gcc` or `clang`
- `make`
- `premake5`
- `flex` 2.6 or newer
- `bison` 3.7 or newer

### Install prerequisites on Ubuntu/Debian

Use the shared setup script:

```bash
./scripts/setup-linux-prerequisites.sh
```

The script installs the required build tools with `apt`, downloads Premake 5,
installs it into `/usr/local/bin`, and prints the detected tool versions.

To install the tools manually instead, run:

```bash
sudo apt update
sudo apt install build-essential make flex bison wget tar

wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-linux.tar.gz
tar -xzf premake-5.0.0-beta7-linux.tar.gz
sudo install -m 0755 premake5 /usr/local/bin/premake5

premake5 --version
bison --version
flex --version
```

## macOS

### macOS prerequisites

Before building Ember on macOS, make sure the following tools are available:

- Apple command-line developer tools
- `premake5`
- `flex` 2.6 or newer
- `bison` 3.7 or newer

> Ember uses modern Bison directives, so the Apple-provided Bison version may be
too old.

### Install prerequisites on macOS

[Homebrew](https://brew.sh/) is the recommended way to install the required tools on macOS.

Install the required tools:

```bash
brew install premake bison flex
```

Homebrew’s Bison and Flex are not always placed before the Apple-provided tools
on PATH. Add them to your shell configuration.

On Apple Silicon Macs:

```bash
export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/flex/bin:$PATH"
```

On Intel Macs:

```bash
export PATH="/usr/local/opt/bison/bin:/usr/local/opt/flex/bin:$PATH"
```

Install Apple command-line developer tools if needed:

```bash
xcode-select --install
```

Check the active tool versions:

```bash
which bison
bison --version
which flex
flex --version
premake5 --version
```

### Build and install on Linux and macOS

After the prerequisites are installed, use the shared Unix install script:

```bash
./scripts/install.sh release
```

To build the debug configuration:

```bash
./scripts/install.sh debug
```

By default, the script installs the `ember-cli` executable into:

```bash
~/.local/bin
```

To use a custom install directory:

```bash
EMBER_INSTALL_DIR=/usr/local/bin ./scripts/install.sh release
```

If `~/.local/bin` is not on your `PATH`, add this to your shell configuration:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

## Windows

### Windows prerequisites

Before building Ember on Windows, make sure the following tools are available:

- Visual Studio Build Tools or Visual Studio Community
- MSVC C/C++ build tools
- MSBuild
- Premake 5
- WinFlexBison

The easiest setup is:

- Install Visual Studio Build Tools or Visual Studio Community
- Select the Desktop development with C++ workload
- Install Premake 5
- Install WinFlexBison

### Install Visual Studio Build Tools

Install Visual Studio Build Tools or Visual Studio Community from [here](https://visualstudio.microsoft.com/downloads/).

During installation, select:

```
Desktop development with C++
```

After installation, `Developer PowerShell for VS` should be available on your system.

Launch `Developer PowerShell for VS` and check if `msbuild` is available:

```powershell
msbuild -version
cl
```

### Install Premake 5 on Windows

Download Premake from <https://premake.github.io/download/>.

Put `premake5.exe` somewhere permanent, for example:

```powershell
C:\Tools\premake5.exe
```

Then add the containing folder to `PATH`:

```powershell
C:\Tools
```

Open a new PowerShell window and check:

```powershell
premake5 --version
```

### Install WinFlexBison on Windows

[Chocolatey](https://chocolatey.org/install) is the recommended way to install
WinFlexBison.

Install it with:

```powershell
choco install winflexbison3
```

Check that the tools are available:

```powershell
win_bison --version
win_flex --version
```

If you do not want to use Chocolatey, download WinFlexBison manually and add the
folder containing `win_bison.exe` and `win_flex.exe` to `PATH`.

### Build and install on Windows

> Run these commands inside `Developer PowerShell for VS` so `msbuild` is
> available.

Use the `-VsAction` value that matches your Visual Studio version.

For Visual Studio 2022:

```powershell
.\scripts\install.ps1 -Config Release -VsAction vs2022
```

For Visual Studio 2026:

```powershell
.\scripts\install.ps1 -Config Release -VsAction vs2026
```

To build the debug configuration, replace `Release` with `Debug`:

```powershell
.\scripts\install.ps1 -Config Debug -VsAction vs2022
```

If PowerShell blocks script execution, allow scripts for the current terminal
session:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

By default, the script installs the `ember-cli` executable into:

```powershell
%USERPROFILE%\.ember\bin
```

Add that directory to your `PATH` if you want to run `ember-cli` from any
terminal.

## Manual build

The install scripts are recommended, but the project can also be built manually.

### Manual build on Linux and macOS

Generate GNU Make build files:

```bash
premake5 gmake2
```

Build a selected configuration:

```bash
make -C build config=debug
make -C build config=release
```

The generated build outputs are placed under:

```text
bin/Debug/
bin/Release/
```

### Manual build on Windows

Generate Visual Studio build files for the version you have installed:

```powershell
premake5 vs2026
```

or

```powershell
premake5 vs2022
```

Build the generated solution:

```powershell
msbuild build\ember.sln /p:Configuration=Release
```

The generated build outputs are placed under:

```text
bin\Debug\
bin\Release\
```

## Docker

If you want to try Ember without installing the toolchain locally, build the
included Docker image. The final image contains `ember-cli`, the example
programs, the end-to-end test runner, the benchmark runner, and the supporting
Python dependencies for those workflows.

Build the image:

```bash
./scripts/install-docker.sh
```

Build a debug image or force Premake source install:

```bash
./scripts/install-docker.sh --config debug
./scripts/install-docker.sh --premake-install-mode source
```

Use `--premake-install-mode source` on ARM systems such as Apple Silicon Macs
or Raspberry Pi devices. The binary Premake path is mainly appropriate for x86
Linux hosts; on ARM you may otherwise need x86 emulation such as Rosetta or a
similar compatibility layer.

Check that the CLI starts:

```bash
docker run --rm ember-cli
docker run --rm ember-cli --version
```

Mount your current directory if you want to assemble or run files from the host:

```bash
docker run --rm -v "$PWD:/workspace" ember-cli assemble docs/examples/helloworld.emb -o hello.emx
docker run --rm -v "$PWD:/workspace" ember-cli run hello.emx
```

The image also ships with the example programs under:

```text
/opt/ember/examples
```

It also includes:

```text
/opt/ember/tests/end2end
/opt/ember/docs/benchmarks
/opt/ember/scripts
```

Run the bundled end-to-end test suite inside the container:

```bash
docker run --rm --entrypoint /opt/ember/scripts/run-e2e.sh ember-cli --ember-cli /usr/local/bin/ember-cli --color never
```

List bundled benchmarks inside the container:

```bash
docker run --rm --entrypoint /opt/ember/scripts/run-benchmarks.sh ember-cli --list
```

## Quick start

After installation, make sure `ember-cli` is available:

```bash
ember-cli --version
```

Create a file named `helloworld.emb`:

```text
.constants:
  message = "Hello, World!\n"

.code:
  func main(arity=0, locals=0):
    ldc message
    print
    ret
```

Assemble the program:

```bash
ember-cli assemble helloworld.emb
```

If you do not provide an output path, `ember-cli assemble` writes `a.emx`.

Run the generated executable:

```bash
ember-cli run a.emx
```

## Scripts

The repository also includes helper scripts under [`scripts/`](./scripts/).

Common entrypoints:

- End-to-end tests: `./scripts/run-e2e.sh`
- Benchmarks: `./scripts/run-benchmarks.sh`
- Python environment setup: `./scripts/setup-python.sh`

Examples:

```bash
# Run the full end-to-end suite
./scripts/run-e2e.sh

# Run the fibo_it benchmark 5 times with input 30
./scripts/run-benchmarks.sh fibo_it 30 --runs 5
```

```powershell
# Run the full end-to-end suite
.\scripts\run-e2e.ps1

# Run the fibo_it benchmark 5 times with input 30
.\scripts\run-benchmarks.ps1 fibo_it 30 --runs 5
```

If you built the project with the install script, you can also run the unit
tests directly from the build output:

```bash
./bin/Release/ember_test
```

For more details, see the related documentation in the [Documentation](#documentation)
section above, especially:

- [`tests/end2end/README.md`](./tests/end2end/README.md)
- [`docs/benchmarks/README.md`](./docs/benchmarks/README.md)
- [`docs/README.md`](./docs/README.md)

For a fuller explanation of the assembly language and how Ember programs are
structured from a user's perspective, see [docs/README.md](./docs/README.md).

## License

This project is licensed under the Apache License 2.0. See [`LICENSE`](./LICENSE).
