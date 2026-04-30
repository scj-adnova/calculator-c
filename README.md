# Calculator – C

Ein einfacher Kommandozeilen-Rechner in C. Liest drei Argumente (zwei Zahlen
und einen Operator), führt die Rechnung aus und gibt das Ergebnis auf stdout
aus. Fehler werden auf stderr gemeldet, der Exit-Code signalisiert Erfolg
(`0`) oder Fehler (`1`).

Im Fokus steht robuste Eingabe- und Ergebnis-Validierung: ungültige Zahlen,
NaN/Inf, Overflow bei Eingabe und Ergebnis sowie unbekannte Operatoren werden
strikt abgelehnt — kein stilles Fehlverhalten.

## Voraussetzungen

- **GCC** oder **Clang** (C11)
- **GNU Make**
- **Bash** (nur für Tests)
- **libm** (Standardbibliothek; wird via `-lm` gelinkt)

Keine externen Abhängigkeiten.

## Build

```bash
make
```

Compiliert mit `-Wall -Wextra -pedantic -Werror -std=c11`. Erzeugt das Binary
`./calculator` im Projekt-Root.

```bash
make clean
```

Entfernt das Binary.

## Verwendung

```bash
./calculator <zahl> <operator> <zahl>
```

**Unterstützte Operatoren:** `+`, `-`, `*`, `/`

**Hinweis:** `*` muss in den meisten Shells gequotet werden, sonst expandiert
die Shell es als Glob-Pattern.

### Beispiele

```bash
./calculator 10 + 5
# 15

./calculator 9 / 3
# 3

./calculator 2.5 '*' 4
# 10

./calculator -5 + 3
# -2

./calculator 1.5 - 4.25
# -2.75
```

Das Ausgabeformat ist `%.6g` — bis zu sechs signifikante Stellen, sonst
wissenschaftliche Notation:

```bash
./calculator 1 / 3
# 0.333333

./calculator 1e300 + 0
# 1e+300
```

## Fehler

Alle Fehlermeldungen gehen auf **stderr**, der Exit-Code ist `1`. stdout
bleibt im Fehlerfall leer.

| Eingabe | Meldung | Ursache |
|---|---|---|
| Falsche Argumentanzahl | `Verwendung: ./calculator <zahl> <operator> <zahl>` | weniger oder mehr als 3 Argumente |
| `./calculator abc + 5` | `Fehler: 'abc' ist keine gültige Zahl` | Operand ist nicht numerisch |
| `./calculator 5xyz + 3` | `Fehler: '5xyz' ist keine gültige Zahl` | Trailing-Zeichen nach der Zahl |
| `./calculator nan + 1` | `Fehler: 'nan' ist keine gültige Zahl` | NaN-Eingabe wird abgelehnt |
| `./calculator inf + 1` | `Fehler: 'inf' ist keine gültige Zahl` | Inf-Eingabe wird abgelehnt |
| `./calculator 1e500 + 0` | `Fehler: '1e500' ist keine gültige Zahl` | Zahl ist zu groß (Overflow) |
| `./calculator 1 ++ 2` | `Fehler: Operator muss ein einzelnes Zeichen sein (+, -, *, /), erhalten: '++'` | Operator hat mehr als ein Zeichen |
| `./calculator 1 "" 2` | `Fehler: Operator muss ein einzelnes Zeichen sein (+, -, *, /), erhalten: ''` | Operator ist leer |
| `./calculator 1 a 2` | `Fehler: Unbekannter Operator 'a' (unterstützt: +, -, *, /)` | Operator nicht in der Liste |
| `./calculator 1 / 0` | `Fehler: Division durch null` | Division durch null |
| `./calculator 1e300 '*' 1e300` | `Fehler: Ergebnis ist zu groß oder ungültig` | Ergebnis überschreitet `double`-Range |

### Verhalten in Skripten

Da Fehler über stdout/stderr und Exit-Code sauber getrennt sind, eignet sich
das Tool für Pipelines:

```bash
result=$(./calculator 10 + 5) && echo "Ergebnis: $result"
# Ergebnis: 15

if ! ./calculator 1 / 0 2>/dev/null; then
    echo "Berechnung fehlgeschlagen"
fi
```

## Tests

```bash
make test
```

Führt eine End-to-End-Test-Suite gegen das gebaute Binary aus
(`tests/run_tests.sh`, Bash, keine externen Abhängigkeiten). Geprüft werden
Fehlerfälle (Argumentanzahl, ungültige Operanden, NaN, Overflow,
Operator-Validierung, Division durch null, Ergebnis-Overflow) sowie
Golden-Path-Regressionen für alle vier Operatoren.

Der Runner prüft pro Testfall Exit-Code, exakten stdout (bei Erfolg) bzw.
stderr-Substring (bei Fehler) und die Trennung der Streams.

## Projektstruktur

```
calculator-c/
├── calculator.c        # Single-File-Implementierung
├── Makefile            # Build und Test-Target
├── tests/
│   └── run_tests.sh    # End-to-End-Test-Runner
└── README.md
```
