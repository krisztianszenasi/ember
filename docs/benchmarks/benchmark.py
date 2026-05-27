#!/usr/bin/env python3

import argparse
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parent
PROGRAMS_DIR = ROOT_DIR / "programs"
BUILD_DIR = ROOT_DIR / ".benchmark-build"


@dataclass
class RunResult:
    command: list[str]
    stdout: str
    stderr: str
    returncode: int
    elapsed_seconds: float

    @property
    def combined_output(self) -> str:
        return self.stdout + self.stderr


def list_benchmarks() -> list[str]:
    benchmarks: list[str] = []

    if not PROGRAMS_DIR.exists():
        return benchmarks

    for ember_file in PROGRAMS_DIR.glob("*.emb"):
        name = ember_file.stem
        python_file = PROGRAMS_DIR / f"{name}.py"

        if python_file.exists():
            benchmarks.append(name)

    return sorted(benchmarks)


def run_command(command: list[str], stdin_text: str | None = None) -> RunResult:
    start = time.perf_counter()

    completed = subprocess.run(
        command,
        input=stdin_text,
        text=True,
        capture_output=True,
        check=False,
    )

    elapsed = time.perf_counter() - start

    return RunResult(
        command=command,
        stdout=completed.stdout,
        stderr=completed.stderr,
        returncode=completed.returncode,
        elapsed_seconds=elapsed,
    )


def extract_last_integer(output: str) -> int | None:
    """
    Tries to extract the benchmark result.

    Works with outputs like:
        Output: 55

    Falls back to the last integer found in stdout/stderr.
    """
    output_match = re.search(r"Output:\s*(-?\d+)", output)

    if output_match:
        return int(output_match.group(1))

    numbers = re.findall(r"-?\d+", output)

    if not numbers:
        return None

    return int(numbers[-1])


def require_command(command: str) -> None:
    if shutil.which(command) is None:
        print(f"error: required command not found: {command}", file=sys.stderr)
        sys.exit(1)


def assemble_ember_program(name: str, ember_cli: str) -> Path:
    ember_source = PROGRAMS_DIR / f"{name}.emb"
    output_file = BUILD_DIR / f"{name}.emx"

    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    result = run_command(
        [
            ember_cli,
            "assemble",
            str(ember_source),
            "-o",
            str(output_file),
        ]
    )

    if result.returncode != 0:
        print("error: failed to assemble Ember benchmark", file=sys.stderr)
        print_command_result(result)
        sys.exit(result.returncode)

    return output_file


def print_command_result(result: RunResult) -> None:
    print(f"command: {' '.join(result.command)}")
    print(f"exit code: {result.returncode}")

    if result.stdout:
        print()
        print("stdout:")
        print(result.stdout.rstrip())

    if result.stderr:
        print()
        print("stderr:")
        print(result.stderr.rstrip())


def run_ember_benchmark(name: str, input_value: str, ember_cli: str) -> RunResult:
    executable = assemble_ember_program(name, ember_cli)

    return run_command(
        [
            ember_cli,
            "run",
            str(executable),
        ],
        stdin_text=f"{input_value}\n",
    )


def run_python_benchmark(name: str, input_value: str, python: str) -> RunResult:
    python_file = PROGRAMS_DIR / f"{name}.py"

    return run_command(
        [
            python,
            str(python_file),
        ],
        stdin_text=f"{input_value}\n",
    )


def print_result_summary(label: str, result: RunResult) -> int | None:
    value = extract_last_integer(result.combined_output)

    print(f"{label}:")
    print(f"  time:   {result.elapsed_seconds:.6f}s")
    print(f"  result: {value if value is not None else '<not found>'}")

    if result.returncode != 0:
        print(f"  exit:   {result.returncode}")

    return value


def run_benchmark(
    name: str,
    input_value: str,
    runs: int,
    ember_cli: str,
    python: str,
    verbose: bool,
) -> None:
    ember_file = PROGRAMS_DIR / f"{name}.emb"
    python_file = PROGRAMS_DIR / f"{name}.py"

    if not ember_file.exists():
        print(f"error: Ember benchmark not found: {ember_file}", file=sys.stderr)
        sys.exit(1)

    if not python_file.exists():
        print(f"error: Python benchmark not found: {python_file}", file=sys.stderr)
        sys.exit(1)

    require_command(ember_cli)
    require_command(python)

    ember_times: list[float] = []
    python_times: list[float] = []

    last_ember_value: int | None = None
    last_python_value: int | None = None

    print(f"Benchmark: {name}")
    print(f"Input:     {input_value}")
    print(f"Runs:      {runs}")
    print()

    for index in range(runs):
        ember_result = run_ember_benchmark(name, input_value, ember_cli)
        python_result = run_python_benchmark(name, input_value, python)

        if verbose:
            print(f"--- Ember output, run {index + 1} ---")
            print(ember_result.combined_output.rstrip())
            print()
            print(f"--- Python output, run {index + 1} ---")
            print(python_result.combined_output.rstrip())
            print()

        if ember_result.returncode != 0:
            print("error: Ember benchmark failed", file=sys.stderr)
            print_command_result(ember_result)
            sys.exit(ember_result.returncode)

        if python_result.returncode != 0:
            print("error: Python benchmark failed", file=sys.stderr)
            print_command_result(python_result)
            sys.exit(python_result.returncode)

        ember_value = extract_last_integer(ember_result.combined_output)
        python_value = extract_last_integer(python_result.combined_output)

        ember_times.append(ember_result.elapsed_seconds)
        python_times.append(python_result.elapsed_seconds)

        last_ember_value = ember_value
        last_python_value = python_value

        print(f"Run {index + 1}:")
        print(f"  Ember:  {ember_result.elapsed_seconds:.6f}s, result={ember_value}")
        print(f"  Python: {python_result.elapsed_seconds:.6f}s, result={python_value}")
        print()

    ember_avg = sum(ember_times) / len(ember_times)
    python_avg = sum(python_times) / len(python_times)

    print("Summary:")
    print(f"  Ember average:  {ember_avg:.6f}s")
    print(f"  Python average: {python_avg:.6f}s")

    if python_avg > 0:
        ratio = ember_avg / python_avg
        print(f"  Ratio:          Ember is {ratio:.2f}x Python time")

    print(f"  Ember result:   {last_ember_value}")
    print(f"  Python result:  {last_python_value}")
    print(f"  Match:          {'yes' if last_ember_value == last_python_value else 'no'}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Compare Ember benchmarks with equivalent Python programs."
    )

    parser.add_argument(
        "benchmark",
        nargs="?",
        help="Benchmark name, for example: fibonacci_it",
    )

    parser.add_argument(
        "input",
        nargs="?",
        help="Input passed to both programs through stdin.",
    )

    parser.add_argument(
        "--list",
        action="store_true",
        help="List available benchmarks.",
    )

    parser.add_argument(
        "--runs",
        type=int,
        default=1,
        help="Number of times to run each benchmark. Default: 1.",
    )

    parser.add_argument(
        "--ember-cli",
        default="ember-cli",
        help="Path/name of ember-cli executable. Default: ember-cli.",
    )

    parser.add_argument(
        "--python",
        default=sys.executable,
        help=f"Python executable. Default: {sys.executable}.",
    )

    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Print program output for each run.",
    )

    args = parser.parse_args()

    if args.list:
        benchmarks = list_benchmarks()

        if not benchmarks:
            print("No benchmarks found.")
            return

        for benchmark in benchmarks:
            print(benchmark)

        return

    if args.benchmark is None or args.input is None:
        parser.error("expected: benchmark.py <benchmark> <input>")

    if args.runs < 1:
        parser.error("--runs must be at least 1")

    run_benchmark(
        name=args.benchmark,
        input_value=args.input,
        runs=args.runs,
        ember_cli=args.ember_cli,
        python=args.python,
        verbose=args.verbose,
    )


if __name__ == "__main__":
    main()