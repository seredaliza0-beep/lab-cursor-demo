/*
 * Лабораторна робота 5, варіант 8, завдання 1.
 * Ввести прізвище, ім'я та по батькові одним рядком.
 * Вилучити з прізвища всі букви а/А та о/О (також латинські a/A, o/O).
 */
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_BYTES 512

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

static int read_line(char *buf, int size)
{
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

/* Повертає кількість байтів одного UTF-8 символу і його код. */
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

static int is_a_or_o(unsigned cp)
{
    return cp == 'a' || cp == 'A' || cp == 'o' || cp == 'O' ||
           cp == 0x0430 || cp == 0x0410 || /* а А */
           cp == 0x043E || cp == 0x041E;   /* о О */
}

/* Перше слово рядка — прізвище. Пробіл в UTF-8 займає один байт. */
static void first_word(const char *src, char *dst, int dst_size)
{
    int i = 0;

    while (*src == ' ' || *src == '\t') {
        src++;
    }
    while (*src != '\0' && *src != ' ' && *src != '\t' && i + 1 < dst_size) {
        dst[i++] = *src++;
    }
    dst[i] = '\0';
}

static void remove_a_and_o(const char *src, char *dst)
{
    char *out = dst;

    while (*src != '\0') {
        unsigned cp;
        int len = utf8_decode(src, &cp);

        if (!is_a_or_o(cp)) {
            memcpy(out, src, (size_t)len);
            out += len;
        }
        src += len;
    }
    *out = '\0';
}

int main(void)
{
    char line[MAX_BYTES];
    char surname[MAX_BYTES];
    char result[MAX_BYTES];

    setup_console();
    printf("Введіть прізвище, ім'я та по батькові:\n");
    if (!read_line(line, (int)sizeof line)) {
        printf("Немає вхідних даних.\n");
        return 1;
    }

    first_word(line, surname, (int)sizeof surname);
    if (surname[0] == '\0') {
        printf("Не вдалося прочитати прізвище.\n");
        return 1;
    }

    remove_a_and_o(surname, result);
    printf("Прізвище: %s\n", surname)
    printf("Після вилучення букв а та о: %s\n", result);
    return 0;
}
