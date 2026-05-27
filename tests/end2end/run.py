#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

try:
    import yaml
except ImportError:
    yaml = None


ROOT_DIR = Path(__file__).resolve().parents[2]
TESTS_DIR = Path(__file__).resolve().parent
DEFAULT_CASES_FILE = TESTS_DIR / "cases.yml"
DEFAULT_PROGRAMS_DIR = TESTS_DIR / "programs"
DEFAULT_BUILD_DIR = TESTS_DIR / ".build"


@dataclass
class CommandResult:
    command: list[str]
    returncode: int
    stdout: str
    stderr: str


@dataclass
class CaseOutcome:
    name: str
    status: str
    details: list[str]


class CasesParseError(RuntimeError):
    pass


class Colors:
    RESET = "\033[0m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    CYAN = "\033[36m"


def load_cases(cases_file: Path) -> list[dict[str, object]]:
    if yaml is None:
        raise CasesParseError(
            "PyYAML is not installed. Install tests/end2end/requirements.txt first."
        )

    try:
        parsed = yaml.safe_load(cases_file.read_text(encoding="utf-8"))
    except yaml.YAMLError as exc:
        raise CasesParseError(str(exc)) from exc

    if not isinstance(parsed, dict):
        raise CasesParseError("expected a top-level mapping")

    cases = parsed.get("cases")
    if not isinstance(cases, list):
        raise CasesParseError("expected top-level 'cases:' list")

    for index, case in enumerate(cases, start=1):
        if not isinstance(case, dict):
            raise CasesParseError(f"case #{index} must be a mapping")

    return cases


def find_default_ember_cli() -> str | None:
    env_cli = os.environ.get("EMBER_CLI")
    if env_cli:
        return env_cli

    local_candidates = [
        ROOT_DIR / "bin" / "Release" / "ember-cli",
        ROOT_DIR / "bin" / "Debug" / "ember-cli",
    ]

    for candidate in local_candidates:
        if candidate.exists():
            return str(candidate)

    path_cli = shutil.which("ember-cli")
    return path_cli


def run_command(command: list[str], stdin_text: str = "") -> CommandResult:
    completed = subprocess.run(
        command,
        input=stdin_text,
        text=True,
        capture_output=True,
        check=False,
    )

    return CommandResult(
        command=command,
        returncode=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )


def format_command(command: list[str]) -> str:
    return " ".join(command)


def should_use_color(mode: str) -> bool:
    if mode == "always":
        return True

    if mode == "never":
        return False

    return sys.stdout.isatty()


def colorize(text: str, color: str, enabled: bool) -> str:
    if not enabled:
        return text

    return f"{color}{text}{Colors.RESET}"


def print_run_header(
    ember_cli: str,
    cases: list[dict[str, object]],
    cases_file: Path,
    use_color: bool,
) -> None:
    version_result = run_command([ember_cli, "--version"])
    title = colorize("Running Ember End-to-End Test Cases", Colors.CYAN, use_color)

    print(title)
    print(f"Cases file: {cases_file}")
    print(f"Selected cases: {len(cases)}")
    print(f"Ember CLI: {ember_cli}")

    if version_result.returncode == 0:
        version_text = version_result.stdout.strip() or "(no output)"
        print(f"Ember CLI version: {version_text}")
    else:
        print(f"Ember CLI version: unavailable (exit code {version_result.returncode})")
        stderr_text = version_result.stderr.strip()
        if stderr_text:
            print(f"Version check stderr: {stderr_text}")

    print()


def validate_expected(result: CommandResult, expected: dict[str, object]) -> list[str]:
    errors: list[str] = []

    if "exit_code" in expected and result.returncode != expected["exit_code"]:
        errors.append(
            f"expected exit_code={expected['exit_code']}, got {result.returncode}"
        )

    if "stdout" in expected and result.stdout != expected["stdout"]:
        errors.append(
            "stdout mismatch\n"
            f"expected:\n{expected['stdout']!r}\n"
            f"actual:\n{result.stdout!r}"
        )

    if "stderr" in expected and result.stderr != expected["stderr"]:
        errors.append(
            "stderr mismatch\n"
            f"expected:\n{expected['stderr']!r}\n"
            f"actual:\n{result.stderr!r}"
        )

    if "stdout_contains" in expected and str(expected["stdout_contains"]) not in result.stdout:
        errors.append(f"stdout does not contain {expected['stdout_contains']!r}")

    if "stderr_contains" in expected and str(expected["stderr_contains"]) not in result.stderr:
        errors.append(f"stderr does not contain {expected['stderr_contains']!r}")

    return errors


def assemble_program(
    ember_cli: str,
    program_file: Path,
    build_dir: Path,
    case_name: str,
) -> tuple[CommandResult, Path]:
    build_dir.mkdir(parents=True, exist_ok=True)
    output_file = build_dir / f"{case_name}.emx"
    command = [ember_cli, "assemble", str(program_file), "-o", str(output_file)]
    return run_command(command), output_file


def normalize_case(case_data: dict[str, object]) -> tuple[str, str, str, dict[str, object], dict[str, object]]:
    name = str(case_data.get("name", "")).strip()
    program = str(case_data.get("program", "")).strip()
    stdin_text = str(case_data.get("stdin", ""))
    expected = case_data.get("expected", {})
    assemble = case_data.get("assemble", {})

    if not name:
        raise CasesParseError("case is missing a non-empty 'name'")

    if not program:
        raise CasesParseError(f"case '{name}' is missing a non-empty 'program'")

    if not isinstance(expected, dict):
        raise CasesParseError(f"case '{name}' has invalid 'expected' section")

    if not isinstance(assemble, dict):
        raise CasesParseError(f"case '{name}' has invalid 'assemble' section")

    return name, program, stdin_text, expected, assemble


def run_case(
    case_data: dict[str, object],
    ember_cli: str,
    programs_dir: Path,
    build_dir: Path,
) -> CaseOutcome:
    name, program, stdin_text, expected, assemble = normalize_case(case_data)
    program_file = programs_dir / program

    if not program_file.exists():
        return CaseOutcome(
            name=name,
            status="SKIP",
            details=[f"program not found: {program_file}"],
        )

    assemble_result, executable = assemble_program(ember_cli, program_file, build_dir, name)
    expect_assemble_failure = bool(assemble.get("expect_failure", False))

    if expect_assemble_failure:
        errors = validate_expected(assemble_result, expected)
        if errors:
            details = [f"assemble command: {format_command(assemble_result.command)}"]
            details.extend(errors)
            details.append(f"stdout: {assemble_result.stdout!r}")
            details.append(f"stderr: {assemble_result.stderr!r}")
            return CaseOutcome(name=name, status="FAIL", details=details)

        return CaseOutcome(
            name=name,
            status="PASS",
            details=[f"assemble failure matched expectations ({assemble_result.returncode})"],
        )

    if assemble_result.returncode != 0:
        return CaseOutcome(
            name=name,
            status="FAIL",
            details=[
                f"assemble command failed: {format_command(assemble_result.command)}",
                f"exit_code={assemble_result.returncode}",
                f"stdout={assemble_result.stdout!r}",
                f"stderr={assemble_result.stderr!r}",
            ],
        )

    run_result = run_command([ember_cli, "run", str(executable)], stdin_text=stdin_text)
    errors = validate_expected(run_result, expected)

    if errors:
        details = [f"run command: {format_command(run_result.command)}"]
        details.extend(errors)
        details.append(f"stdout: {run_result.stdout!r}")
        details.append(f"stderr: {run_result.stderr!r}")
        return CaseOutcome(name=name, status="FAIL", details=details)

    return CaseOutcome(name=name, status="PASS", details=[])


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Run Ember end-to-end test cases from a YAML cases file."
    )
    parser.add_argument(
        "--cases",
        type=Path,
        default=DEFAULT_CASES_FILE,
        help=f"Path to the cases file. Default: {DEFAULT_CASES_FILE}",
    )
    parser.add_argument(
        "--programs-dir",
        type=Path,
        default=DEFAULT_PROGRAMS_DIR,
        help=f"Directory containing .emb programs. Default: {DEFAULT_PROGRAMS_DIR}",
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=DEFAULT_BUILD_DIR,
        help=f"Directory for assembled .emx files. Default: {DEFAULT_BUILD_DIR}",
    )
    parser.add_argument(
        "--ember-cli",
        default=find_default_ember_cli(),
        help="Path to ember-cli. Defaults to EMBER_CLI, bin/Release, bin/Debug, or PATH.",
    )
    parser.add_argument(
        "--case",
        action="append",
        dest="case_filters",
        default=[],
        help="Run only the named case. Can be repeated.",
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="List case names and exit.",
    )
    parser.add_argument(
        "--keep-build",
        action="store_true",
        help="Keep assembled .emx files after the run.",
    )
    parser.add_argument(
        "--color",
        choices=["auto", "always", "never"],
        default="auto",
        help="Colorize output. Default: auto.",
    )
    return parser


def main() -> int:
    parser = build_argument_parser()
    args = parser.parse_args()

    if not args.cases.exists():
        print(f"error: cases file not found: {args.cases}", file=sys.stderr)
        return 2

    if not args.ember_cli:
        print(
            "error: could not find ember-cli. Use --ember-cli or set EMBER_CLI.",
            file=sys.stderr,
        )
        return 2

    try:
        cases = load_cases(args.cases)
    except CasesParseError as exc:
        print(f"error: failed to parse cases file: {exc}", file=sys.stderr)
        return 2

    if args.case_filters:
        wanted = set(args.case_filters)
        cases = [case for case in cases if str(case.get("name", "")) in wanted]

    if args.list:
        for case in cases:
            print(case.get("name", "<unnamed>"))
        return 0

    if not cases:
        print("No matching cases found.", file=sys.stderr)
        return 1

    use_color = should_use_color(args.color)
    print_run_header(args.ember_cli, cases, args.cases, use_color)
    pass_count = 0
    fail_count = 0
    skip_count = 0

    for case_data in cases:
        outcome = run_case(case_data, args.ember_cli, args.programs_dir, args.build_dir)

        if outcome.status == "PASS":
            status_label = colorize("[PASS]", Colors.GREEN, use_color)
        elif outcome.status == "FAIL":
            status_label = colorize("[FAIL]", Colors.RED, use_color)
        else:
            status_label = colorize("[SKIP]", Colors.YELLOW, use_color)

        print(f"{status_label} {outcome.name}")

        for detail in outcome.details:
            print(f"  {colorize(detail, Colors.CYAN, use_color)}")

        if outcome.status == "PASS":
            pass_count += 1
        elif outcome.status == "FAIL":
            fail_count += 1
        else:
            skip_count += 1

    if not args.keep_build:
        shutil.rmtree(args.build_dir, ignore_errors=True)

    print()
    summary = (
        f"Summary: {pass_count} passed, {fail_count} failed, {skip_count} skipped"
    )
    summary_color = Colors.RED if fail_count else Colors.GREEN
    print(colorize(summary, summary_color, use_color))

    return 1 if fail_count else 0


if __name__ == "__main__":
    sys.exit(main())
