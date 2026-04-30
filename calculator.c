#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>

/*
 * Calculator CLI
 * Usage: ./calculator <number> <operator> <number>
 * Example: ./calculator 10 + 5
 */

enum calc_status {
    CALC_OK = 0,
    CALC_ERR_DIV_ZERO,
    CALC_ERR_UNKNOWN_OP,
    CALC_ERR_OVERFLOW,
};

static enum calc_status calculate(double a, char op, double b, double *result) {
    switch (op) {
        case '+': *result = a + b; break;
        case '-': *result = a - b; break;
        case '*': *result = a * b; break;
        case '/':
            if (b == 0.0) return CALC_ERR_DIV_ZERO;
            *result = a / b;
            break;
        default:
            return CALC_ERR_UNKNOWN_OP;
    }
    if (!isfinite(*result)) return CALC_ERR_OVERFLOW;
    return CALC_OK;
}

static bool parse_number(const char *input, double *result) {
    if (input == NULL || *input == '\0') {
        return false;
    }

    errno = 0;
    char *end = NULL;
    double value = strtod(input, &end);

    bool no_digits_consumed = (end == input);
    bool has_trailing_chars = (*end != '\0');
    bool out_of_range       = (errno == ERANGE);
    bool not_finite         = !isfinite(value);

    if (no_digits_consumed || has_trailing_chars || out_of_range || not_finite) {
        return false;
    }

    *result = value;
    return true;
}

/*
 * Entry point: parses three CLI arguments (number, operator, number),
 * validates them, calls calculate(), and prints the result to stdout.
 * On any input or computation error, prints a message to stderr and
 * exits with status 1.
 */
int main(int argc, char *argv[]) {
    const char *prog = (argc > 0 && argv[0]) ? argv[0] : "calculator";

    if (argc != 4) {
        fprintf(stderr, "Verwendung: %s <zahl> <operator> <zahl>\n", prog);
        fprintf(stderr, "Beispiel: %s 10 + 5\n", prog);
        return 1;
    }

    double a, b;
    if (!parse_number(argv[1], &a)) {
        fprintf(stderr, "Fehler: '%s' ist keine gültige Zahl\n", argv[1]);
        return 1;
    }
    if (!parse_number(argv[3], &b)) {
        fprintf(stderr, "Fehler: '%s' ist keine gültige Zahl\n", argv[3]);
        return 1;
    }

    if (argv[2][0] == '\0' || argv[2][1] != '\0') {
        fprintf(stderr,
                "Fehler: Operator muss ein einzelnes Zeichen sein (+, -, *, /), erhalten: '%s'\n",
                argv[2]);
        return 1;
    }
    char op = argv[2][0];

    double result;
    switch (calculate(a, op, b, &result)) {
        case CALC_OK:
            break;
        case CALC_ERR_DIV_ZERO:
            fprintf(stderr, "Fehler: Division durch null\n");
            return 1;
        case CALC_ERR_UNKNOWN_OP:
            fprintf(stderr,
                    "Fehler: Unbekannter Operator '%c' (unterstützt: +, -, *, /)\n",
                    op);
            return 1;
        case CALC_ERR_OVERFLOW:
            fprintf(stderr, "Fehler: Ergebnis ist zu groß oder ungültig\n");
            return 1;
    }

    printf("%.6g\n", result);

    return 0;
}
