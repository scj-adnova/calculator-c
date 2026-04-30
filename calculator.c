#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Calculator CLI
 * Usage: ./calculator <number> <operator> <number>
 * Example: ./calculator 10 + 5
 */

double calculate(double a, char op, double b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            return a / b;
        default:
            return 0.0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <number> <operator> <number>\n", argv[0]);
        fprintf(stderr, "Example: %s 10 + 5\n", argv[0]);
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[3]);
    char op = argv[2][0];

    double result = calculate(a, op, b);

    printf("%.6g\n", result);

    return 0;
}
