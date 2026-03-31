#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LEN 32
#define PHONE_LEN 24
#define DATE_LEN 16
#define SMALL_LEN 16
#define TEXT_LEN 128
#define LONG_LEN 128
#define MAX_LINE 512

typedef enum { FALSE = 0, TRUE = 1 } Bool;

void trim_newline(char *s);
void clear_input_buffer(void);
void read_line(const char *prompt, char *buf, int size);
void safe_copy(char *dst, const char *src, size_t n);
int read_int(const char *prompt, int minv, int maxv);
double read_double(const char *prompt, double minv, double maxv);
void read_string(const char *prompt, char *buf, size_t n, int allow_empty);
int ask_yes_no(const char *prompt);
int starts_with(const char *s, const char *prefix);
void pause_and_wait(void);
int str_equal_ignore_case(const char *a, const char *b);

#endif
