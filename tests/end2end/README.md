# End-to-End Tests

This directory contains a small end-to-end test runner for Ember programs.

## Files

- `run.py`: test runner
- `cases.yml`: test case definitions
- `programs/`: `.emb` programs used by the cases

## Run The Test Suite

From the repository root:

```bash
./scripts/setup-python.sh
./scripts/run-e2e.sh
```

By default the runner:

- reads cases from `tests/end2end/cases.yml`
- reads programs from `tests/end2end/programs/`
- writes temporary `.emx` files to `tests/end2end/.build/`
- tries to find `ember-cli` from `EMBER_CLI`, `bin/Release/ember-cli`,
  `bin/Debug/ember-cli`, then `PATH`

## Useful Flags

Install the Python dependency first if you have not already:

```bash
./scripts/setup-python.sh
```

List available cases:

```bash
./scripts/run-e2e.sh --list
```

Run only one case:

```bash
./scripts/run-e2e.sh --case fibonacci_it_10
```

Use a specific Ember CLI binary:

```bash
./scripts/run-e2e.sh --ember-cli ./bin/Release/ember-cli
```

Keep assembled test artifacts:

```bash
./scripts/run-e2e.sh --keep-build
```

Control colored output:

```bash
./scripts/run-e2e.sh --color auto
./scripts/run-e2e.sh --color always
./scripts/run-e2e.sh --color never
```

Color modes:

- `auto`: enable colors only when output goes to a terminal
- `always`: always print ANSI colors
- `never`: disable colors completely

## Case Format

Cases are defined under the top-level `cases:` list in `cases.yml`.

Example run case:

```yaml
cases:
  - name: helloworld
    program: helloworld.emb
    expected:
      exit_code: 0
      stdout: |
        Hello, World!
      stderr: ""
```

Supported fields:

- `name`: unique case name
- `program`: program filename relative to `programs/`
- `stdin`: text sent to the program on standard input
- `expected.exit_code`: expected process exit code
- `expected.stdout`: exact expected standard output
- `expected.stderr`: exact expected standard error
- `expected.stdout_contains`: substring that must appear in standard output
- `expected.stderr_contains`: substring that must appear in standard error

Important: `stdout` and `stderr` matching is exact, including prompts and
trailing newlines.

## Assemble-Failure Cases

If a case should fail during assembly, use:

```yaml
cases:
  - name: syntax_error
    program: syntax_error.emb
    assemble:
      expect_failure: true
    expected:
      exit_code: 1
      stderr_contains: "error"
```

In that mode the runner validates the assembler result and does not try to run
the program.

## Missing Programs

If a case references a program that does not exist in `programs/`, the runner
marks that case as `SKIP`. This is useful while drafting fictive cases before
all sample programs exist.
