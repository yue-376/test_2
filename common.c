#include "common.h"

void trim_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void read_line(const char *prompt, char *buf, int size) {
    if (prompt) printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

int read_int(const char *prompt, int min, int max) {
    char line[64];
    char *end;
    long value;
    while (1) {
        read_line(prompt, line, sizeof(line));
        value = strtol(line, &end, 10);
        if (*line != '\0' && *end == '\0' && value >= min && value <= max) {
            return (int)value;
        }
        printf("输入无效，请输入 %d ~ %d 的整数。\n", min, max);
    }
}

void pause_and_wait(void) {
    printf("\n按回车继续...");
    getchar();
}

int str_equal_ignore_case(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}
