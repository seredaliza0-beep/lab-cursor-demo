/*
 * Лабораторна робота 5, варіант 8, завдання 2.
 * Ввести рядки S1 та S2 (до 100 символів).
 * Відсортувати символи S1 за зростанням, S2 — за спаданням.
 */
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_CHARS 100
#define MAX_BYTES 512

typedef struct {
    unsigned cp;
    int len;
    char bytes[4];
} Glyph;

static void setup_console(void)
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
}

static void strip_newline(char *s)
{
    size_t n = strlen(s);

    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static int read_line(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        return 0;
    }
    if (strchr(buf, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
    strip_newline(buf);
    return 1;
}

static int utf8_decode(const char *s, unsigned *cp)
{
    const unsigned char *u = (const unsigned char *)s;

    if (u[0] < 0x80) {
        *cp = u[0];
        return 1;
    }
    if ((u[0] & 0xE0) == 0xC0 && (u[1] & 0xC0) == 0x80) {
        *cp = ((unsigned)(u[0] & 0x1F) << 6) | (u[1] & 0x3F);
        return 2;
    }
    if ((u[0] & 0xF0) == 0xE0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80) {
        *cp = ((unsigned)(u[0] & 0x0F) << 12) |
              ((unsigned)(u[1] & 0x3F) << 6) |
              (u[2] & 0x3F);
        return 3;
    }
    if ((u[0] & 0xF8) == 0xF0 && (u[1] & 0xC0) == 0x80 &&
        (u[2] & 0xC0) == 0x80 && (u[3] & 0xC0) == 0x80) {
        *cp = ((unsigned)(u[0] & 0x07) << 18) |
              ((unsigned)(u[1] & 0x3F) << 12) |
              ((unsigned)(u[2] & 0x3F) << 6) |
              (u[3] & 0x3F);
        return 4;
    }

    *cp = u[0];
    return 1;
}

static int split_glyphs(const char *s, Glyph *glyphs, int max_count, int *truncated)
{
    int count = 0;

    *truncated = 0;
    while (*s != '\0') {
        unsigned cp;
        int len = utf8_decode(s, &cp);

        if (count >= max_count) {
            *truncated = 1;
            break;
        }
        glyphs[count].cp = cp;
        glyphs[count].len = len;
        memcpy(glyphs[count].bytes, s, (size_t)len);
        count++;
        s += len;
    }
    return count;
}

static void sort_glyphs(Glyph *glyphs, int count, int ascending)
{
    int i, j;

    for (i = 0; i < count; i++) {
        for (j = 0; j + 1 < count - i; j++) {
            int out_of_order = ascending
                                   ? glyphs[j].cp > glyphs[j + 1].cp
                                   : glyphs[j].cp < glyphs[j + 1].cp;
            if (out_of_order) {
                Glyph tmp = glyphs[j];
                glyphs[j] = glyphs[j + 1];
                glyphs[j + 1] = tmp;
            }
        }
    }
}

static void join_glyphs(const Glyph *glyphs, int count, char *out)
{
    int i;
    char *p = out;

    for (i = 0; i < count; i++) {
        memcpy(p, glyphs[i].bytes, (size_t)glyphs[i].len);
        p += glyphs[i].len;
    }
    *p = '\0';
}

static void sort_line(const char *src, char *dst, int ascending)
{
    Glyph glyphs[MAX_CHARS];
    int truncated = 0;
    int count = split_glyphs(src, glyphs, MAX_CHARS, &truncated);

    sort_glyphs(glyphs, count, ascending);
    join_glyphs(glyphs, count, dst);
    if (truncated) {
        printf("Увага: враховано лише перші %d символів.\n", MAX_CHARS);
    }
}

int main(void)
{
    char s1[MAX_BYTES];
    char s2[MAX_BYTES];
    char sorted1[MAX_BYTES];
    char sorted2[MAX_BYTES];

    setup_console();
    if (!read_line("Введіть S1 (до 100 символів):\n", s1, (int)sizeof s1) ||
        !read_line("Введіть S2 (до 100 символів):\n", s2, (int)sizeof s2)) {
        printf("Немає вхідних даних.\n");
        return 1;
    }

    sort_line(s1, sorted1, 1);
    sort_line(s2, sorted2, 0);

    printf("S1 за зростанням: %s\n", sorted1);
    printf("S2 за спаданням: %s\n", sorted2);
    return 0;
}
