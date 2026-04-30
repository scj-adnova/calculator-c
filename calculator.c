#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Calculator CLI
 * Usage: ./calculator <number> <operator> <number>
 * Example: ./calculator 10 + 5
 */

enum calc_status {
    CALC_OK = 0,
    CALC_ERR_DIV_ZERO,
    CALC_ERR_UNKNOWN_OP,
};

double calculate(double a, char op, double b, int *status) {
    *status = CALC_OK;
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0.0) {
                *status = CALC_ERR_DIV_ZERO;
                return 0.0;
            }
            return a / b;
        default:
            *status = CALC_ERR_UNKNOWN_OP;
            return 0.0;
    }
}

static int parse_number(const char *s, double *out) {
    if (s == NULL || *s == '\0') return 0;
    errno = 0;
    char *end = NULL;
    double v = strtod(s, &end);
    if (end == s || *end != '\0') return 0;
    if (errno == ERANGE) return 0;
    *out = v;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <number> <operator> <number>\n", argv[0]);
        fprintf(stderr, "Example: %s 10 + 5\n", argv[0]);
        return 1;
    }

    double a, b;
    if (!parse_number(argv[1], &a)) {
        fprintf(stderr, "Error: '%s' is not a valid number\n", argv[1]);
        return 1;
    }
    if (!parse_number(argv[3], &b)) {
        fprintf(stderr, "Error: '%s' is not a valid number\n", argv[3]);
        return 1;
    }

    if (argv[2][0] == '\0' || argv[2][1] != '\0') {
        fprintf(stderr,
                "Error: operator must be a single character (+, -, *, /), got '%s'\n",
                argv[2]);
        return 1;
    }
    char op = argv[2][0];

    int status;
    double result = calculate(a, op, b, &status);
    switch (status) {
        case CALC_OK:
            break;
        case CALC_ERR_DIV_ZERO:
            fprintf(stderr, "Error: division by zero\n");
            return 1;
        case CALC_ERR_UNKNOWN_OP:
            fprintf(stderr,
                    "Error: unknown operator '%c' (supported: +, -, *, /)\n",
                    op);
            return 1;
    }

    printf("%.6g\n", result);

    return 0;
}
