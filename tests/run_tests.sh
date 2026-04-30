#!/usr/bin/env bash
# Test-Runner für ./calculator
# Führt End-to-End-Tests gegen die gebaute Binary aus.

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BIN="$ROOT_DIR/calculator"

if [[ ! -x "$BIN" ]]; then
    echo "Binary fehlt: $BIN — bitte vorher 'make' ausführen." >&2
    exit 2
fi

PASS=0
FAIL=0
TOTAL=0

# assert_ok <name> <expected_stdout> <args...>
assert_ok() {
    local name="$1"; shift
    local expected="$1"; shift
    TOTAL=$((TOTAL + 1))
    local out err rc
    out=$("$BIN" "$@" 2>/tmp/calc_stderr.$$)
    rc=$?
    err=$(cat /tmp/calc_stderr.$$); rm -f /tmp/calc_stderr.$$
    if [[ $rc -eq 0 && "$out" == "$expected" && -z "$err" ]]; then
        echo "  PASS  $name"
        PASS=$((PASS + 1))
    else
        echo "  FAIL  $name"
        echo "        args:     $*"
        echo "        expected: '$expected' (rc=0, stderr leer)"
        echo "        got:      '$out' (rc=$rc, stderr='$err')"
        FAIL=$((FAIL + 1))
    fi
}

# assert_fail <name> <expected_stderr_substring> <args...>
assert_fail() {
    local name="$1"; shift
    local needle="$1"; shift
    TOTAL=$((TOTAL + 1))
    local out err rc
    out=$("$BIN" "$@" 2>/tmp/calc_stderr.$$)
    rc=$?
    err=$(cat /tmp/calc_stderr.$$); rm -f /tmp/calc_stderr.$$
    if [[ $rc -eq 1 && "$err" == *"$needle"* && -z "$out" ]]; then
        echo "  PASS  $name"
        PASS=$((PASS + 1))
    else
        echo "  FAIL  $name"
        echo "        args:     $*"
        echo "        expected: stderr enthält '$needle' (rc=1, stdout leer)"
        echo "        got:      stdout='$out' rc=$rc stderr='$err'"
        FAIL=$((FAIL + 1))
    fi
}

echo "=== Fehlerfälle ==="
assert_fail "Division durch null"        "Fehler: Division durch null"                 1 / 0
assert_fail "Nicht-numerischer Operand"  "ist keine gültige Zahl"                      abc + 5
assert_fail "Mehrzeichen-Operator"       "Operator muss ein einzelnes Zeichen sein"    1 ++ 2
assert_fail "Leerer Operator"            "Operator muss ein einzelnes Zeichen sein"    1 "" 2
assert_fail "Unbekannter Operator"       "Unbekannter Operator"                         1 a 2
assert_fail "Falsche Argumentanzahl"     "Verwendung:"                                  10 +
assert_fail "NaN-Eingabe abgelehnt"      "ist keine gültige Zahl"                      nan + 1
assert_fail "Sehr große Zahl (Overflow)" "ist keine gültige Zahl"                      1e500 + 0

echo
echo "=== Golden Paths ==="
assert_ok "Addition"              "15"     10 + 5
assert_ok "Division"              "3"      9 / 3
assert_ok "Multiplikation"        "10"     2.5 '*' 4
assert_ok "Sehr große, gültige Zahl" "1e+300"  1e300 + 0

echo
echo "$PASS/$TOTAL passed"
[[ $FAIL -eq 0 ]] || exit 1
