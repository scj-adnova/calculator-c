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
assert_fail "Nicht-numerischer Operand"  "ungültige Zeichen"                           abc + 5
assert_fail "Mehrzeichen-Operator"       "ungültige Zeichen"                           1 ++ 2
assert_fail "Leerer Operator"            "ungültige Zeichen"                           1 "" 2
assert_fail "Unbekannter Operator"       "ungültige Zeichen"                           1 a 2
assert_fail "Falsche Argumentanzahl"     "Verwendung:"                                  10 +
assert_fail "NaN-Eingabe abgelehnt"      "ungültige Zeichen"                           nan + 1
assert_fail "Sehr große Zahl (Overflow)" "ist keine gültige Zahl"                      1e500 + 0
assert_fail "Ergebnis-Overflow"          "Ergebnis ist zu groß"                        1e300 '*' 1e300

echo
echo "=== Golden Paths ==="
assert_ok "Addition"              "15"     10 + 5
assert_ok "Division"              "3"      9 / 3
assert_ok "Multiplikation"        "10"     2.5 '*' 4
assert_ok "Sehr große, gültige Zahl" "1e+300"  1e300 + 0

echo
echo "=== Verlauf ==="
rm -f "$HOME/.calculator_history"
assert_ok "Verlauf leer"            "Kein Verlauf vorhanden."   --verlauf
"$BIN" 7 + 3 > /dev/null 2>&1
assert_ok "Verlauf ein Eintrag"     "7 + 3 = 10"                --verlauf

# 10 weitere Berechnungen durchführen (insgesamt 11), sodass der erste Eintrag
# (7 + 3 = 10) aus dem FIFO-Puffer herausfällt
for i in $(seq 1 10); do "$BIN" "$i" + 0 > /dev/null 2>&1; done

# Verlauf auslesen und Zeilenanzahl prüfen
TOTAL_LINES=$("$BIN" --verlauf | wc -l | tr -d ' ')
TOTAL=$((TOTAL + 1))
if [[ "$TOTAL_LINES" -eq 10 ]]; then
    echo "  PASS  Verlauf genau 10 Einträge"
    PASS=$((PASS + 1))
else
    echo "  FAIL  Verlauf genau 10 Einträge"
    echo "        expected: 10 Zeilen"
    echo "        got:      $TOTAL_LINES Zeilen"
    FAIL=$((FAIL + 1))
fi

# Erster Eintrag (7 + 3 = 10) darf nicht mehr vorhanden sein
TOTAL=$((TOTAL + 1))
if ! "$BIN" --verlauf | grep -q "7 + 3 = 10"; then
    echo "  PASS  Verlauf ältester Eintrag verdrängt"
    PASS=$((PASS + 1))
else
    echo "  FAIL  Verlauf ältester Eintrag verdrängt"
    echo "        '7 + 3 = 10' sollte nicht mehr im Verlauf stehen"
    FAIL=$((FAIL + 1))
fi

echo
echo "$PASS/$TOTAL passed"
[[ $FAIL -eq 0 ]] || exit 1
