#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

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

#define HISTORY_MAX      10
#define HISTORY_LINE_MAX 128

typedef struct {
    char line[HISTORY_LINE_MAX];
} HistoryEntry;

static HistoryEntry g_history[HISTORY_MAX];
static int g_history_count = 0;

/* Baut den Pfad ~/.calculator_history.
 * Gibt NULL zurück wenn HOME nicht gesetzt ist. */
static const char *history_path(void) {
    static char buf[512];
    const char *home = getenv("HOME");
    if (home == NULL) return NULL;
    int n = snprintf(buf, sizeof(buf), "%s/.calculator_history", home);
    if (n < 0 || (size_t)n >= sizeof(buf)) return NULL;
    return buf;
}

/* Hängt line an g_history an; verdrängt ältesten Eintrag bei Überlauf. */
static void history_append(const char *line) {
    if (g_history_count < HISTORY_MAX) {
        strncpy(g_history[g_history_count].line, line, HISTORY_LINE_MAX - 1);
        g_history[g_history_count].line[HISTORY_LINE_MAX - 1] = '\0';
        g_history_count++;
    } else {
        memmove(&g_history[0], &g_history[1],
                sizeof(HistoryEntry) * (HISTORY_MAX - 1));
        strncpy(g_history[HISTORY_MAX - 1].line, line, HISTORY_LINE_MAX - 1);
        g_history[HISTORY_MAX - 1].line[HISTORY_LINE_MAX - 1] = '\0';
    }
}

/* Liest die letzten HISTORY_MAX Zeilen aus path in g_history. */
static void history_load(const char *path) {
    g_history_count = 0;
    if (path == NULL) return;

    FILE *f = fopen(path, "r");
    if (f == NULL) return;

    char line[HISTORY_LINE_MAX];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        history_append(line);
    }
    fclose(f);
}

/* Schreibt g_history atomar in path (via Tmp-Datei + rename). */
static void history_save(const char *path) {
    if (path == NULL) return;

    char tmp[512];
    int n = snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    if (n < 0 || (size_t)n >= sizeof(tmp)) return;

    FILE *f = fopen(tmp, "w");
    if (f == NULL) return;

    for (int i = 0; i < g_history_count; i++) {
        fprintf(f, "%s\n", g_history[i].line);
    }
    fclose(f);
    rename(tmp, path);
}

/* Gibt alle Einträge aus; bei leerem Verlauf: "Kein Verlauf vorhanden." */
static void history_print(void) {
    if (g_history_count == 0) {
        printf("Kein Verlauf vorhanden.\n");
        return;
    }
    for (int i = 0; i < g_history_count; i++) {
        printf("%s\n", g_history[i].line);
    }
}

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
    const char *path = history_path();

    if (argc == 2 && strcmp(argv[1], "--verlauf") == 0) {
        history_load(path);
        history_print();
        return 0;
    }

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

    /* Verlauf aktualisieren */
    history_load(path);
    char entry[HISTORY_LINE_MAX];
    int nw = snprintf(entry, sizeof(entry), "%.6g %c %.6g = %.6g", a, op, b, result);
    if (nw > 0 && (size_t)nw < sizeof(entry)) {
        history_append(entry);
        history_save(path);
    }

    return 0;
}
