# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

- `make` — build the `calculator` binary (gcc, `-Wall -Wextra -pedantic -std=c11`, linked with `-lm`).
- `make test` — build + run the end-to-end test suite (`tests/run_tests.sh`).
- `make clean` — remove the binary.
- Run a single test: there is no per-test selector. Either edit `tests/run_tests.sh` to comment out cases, or invoke the binary directly, e.g. `./calculator 1 / 0`. Tests assert exit code, stderr substring, and stdout exactly.

## Architecture

Single-file CLI program (`calculator.c`) plus a Bash-based end-to-end test runner. No external dependencies beyond libc and libm.

**Error-handling convention (consistent across both functions):** status is the return value, the computed value is written to an out-parameter.

- `parse_number(const char *input, double *result) → bool` — uses `strtod` with endpointer + `errno == ERANGE` + `isfinite()` to reject garbage, trailing chars, overflow, and NaN/Inf. Never use `atof` here (silently returns 0 on garbage).
- `calculate(double a, char op, double b, double *result) → enum calc_status` — returns `CALC_OK`, `CALC_ERR_DIV_ZERO`, or `CALC_ERR_UNKNOWN_OP`. `main` switches on the return value to print a message and exit 1.

Operator validation lives in `main`: `argv[2]` must be exactly one character. This check is intentionally outside `calculate()` so the function only deals with already-validated single-char operators.

**User-facing strings are German.** All `fprintf(stderr, ...)` messages and the `Verwendung:` usage line are localized. The test runner asserts on German substrings — adding a new error path means adding both the German message and a matching assertion in `tests/run_tests.sh`.

## Testing

`tests/run_tests.sh` is a dependency-free Bash runner with two helpers:
- `assert_ok <name> <expected_stdout> <args...>` — expects rc=0, exact stdout match, empty stderr.
- `assert_fail <name> <expected_stderr_substring> <args...>` — expects rc=1, stderr contains the substring, empty stdout.

When adding behavior, add both an error case and a golden-path regression test. The `*` operator must be quoted in test args (`'*'`) to avoid shell glob expansion.
