# Ember Benchmarks

This directory contains small benchmark programs used to compare Ember against
equivalent Python implementations.

## Contents

- `benchmark.py`: benchmark runner
- `programs/*.emb`: Ember benchmark programs
- `programs/*.py`: Python reference implementations of the same programs

Each benchmark is expected to exist as a pair with the same base name, for
example:

- `programs/fibo_it.emb`
- `programs/fibo_it.py`

## What The Runner Does

The benchmark runner:

1. Assembles the selected Ember program into `docs/benchmarks/.benchmark-build/`
2. Runs the Ember executable
3. Runs the matching Python program
4. Measures execution time for both
5. Extracts the final integer result from program output
6. Reports per-run timings, averages, and whether the results match

Input is passed to both programs through standard input.

## List Available Benchmarks

```bash
./scripts/setup-python.sh
./scripts/run-benchmarks.sh --list
```

## Run A Benchmark

Run a benchmark by passing its name and input value:

```bash
./scripts/run-benchmarks.sh fibo_it 30
```

Run the same benchmark multiple times:

```bash
./scripts/run-benchmarks.sh fibo_it 30 --runs 5
```

Print the full output from each Ember and Python run:

```bash
./scripts/run-benchmarks.sh fibo_it 30 --runs 3 --verbose
```

## Requirements

The runner expects:

- `ember-cli` to be available on `PATH`, or passed explicitly with `--ember-cli`
- Python to be available, or passed explicitly with `--python`

Example with an explicit Ember CLI path:

```bash
./scripts/run-benchmarks.sh loop_sum 100000 --ember-cli ./bin/Release/ember-cli
```

## Adding A New Benchmark

To add a new benchmark:

1. Create an Ember program in `programs/<name>.emb`
2. Create the matching Python version in `programs/<name>.py`
3. Make sure both programs read the same input format
4. Make sure both programs print the same final numeric result

The runner uses the benchmark filename stem as the benchmark name and only
lists entries that have both `.emb` and `.py` versions.
