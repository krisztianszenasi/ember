# Changelog

## [v0.1.2] - 2026-05-29

### Fixed

- Fixed bytecode corruption caused by stale label patch entries being reused across function bodies.
- Cleared the patch table between function bodies so label resolution only patches locations belonging to the current function.

### Added

- Added end-to-end test coverage for reused label names across function bodies.

## [v0.1.1] - 2026-05-28

### Added

- Added first-class `none` literal support.
- Added test coverage for `none` literal assembly/value behavior.

## [v0.1.0] - 2026-05-27

Initial release of Ember.

### Features

- Stack-based virtual machine for executing Ember bytecode.
- Assembler and CLI tooling for assembling, running, disassembling, and inspecting programs.
- Native `.emx` executable format.
- Cross-platform build and installation support for Linux, macOS, and Windows.
- Example programs, benchmarks, and end-to-end test coverage.
