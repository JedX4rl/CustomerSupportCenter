//
// Created by Никита Третьяков on 24.02.2024.
//
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>

typedef long long ll;
typedef unsigned long long ull;

void print_error(status_code code, int nl_cnt)
{
    fprint_error(stdout, code, nl_cnt);
}

void fprint_error(FILE* file, status_code code, int nl_cnt)
{
    switch (code)
    {
        case OK:
            return;
        case NULL_ARG:
            fprintf(file, "Null argument");
            return;
        case INVALID_INPUT:
            fprintf(file, "Invalid input");
            return;
        case INVALID_ARG:
            fprintf(file, "Invalid argument");
            return;
        case INVALID_FLAG:
            fprintf(file, "Invalid flag");
            return;
        case INVALID_NUMBER:
            fprintf(file, "Invalid number");
            return;
        case INVALID_BASE:
            fprintf(file, "Invalid base");
            return;
        case INVALID_EPSILON:
            fprintf(file, "Invalid epsilon");
            return;
        case INVALID_CMD:
            fprintf(file, "Invalid command");
            return;
        case INVALID_BRACKET_ORDER:
            fprintf(file, "Invalid bracket order");
            return;
        case FILE_OPENING_ERROR:
            fprintf(file, "File cannot be opened");
            return;
        case FILE_INVALID_CONTENT:
            fprintf(file, "File content is invalid");
            return;
        case FILE_END:
            fprintf(file, "Unexpected end of file was found");
            return;
        case OVERFLOWED:
            fprintf(file, "Attempting to overflow");
            return;
        case UNINITIALIZED_USAGE:
            fprintf(file, "Attempting to access uninitialized variable");
            return;
        case DIVISION_BY_ZERO:
            fprintf(file, "Attempting to divide by zero");
            return;
        case ZERO_POWERED_ZERO:
            fprintf(file, "Attempting to raize zero in the power zero");
            return;
        case BAD_ALLOC:
            fprintf(file, "Memory lack occurred");
            return;
        case BAD_ACCESS:
            fprintf(file, "Attempting to access incorrect memory");
            return;
        default:
            fprintf(file, "Unexpected error occurred");
            return;
    }
    for (int i = 0; i < nl_cnt; ++i)
    {
        printf("\n");
    }
}


int pair_str_double_comparator(const void* ptr_1, const void* ptr_2)
{
    const pair_str_double* pair_1 = (const pair_str_double*) ptr_1;
    const pair_str_double* pair_2 = (const pair_str_double*) ptr_2;
    return strcmp(pair_1->str, pair_2->str);
}


status_code fread_line(FILE* file, char** line)
{
    if (file == NULL || line == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* line_tmp = (char*) malloc(sizeof(char) * size);
    if (line_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    if (feof(file))
    {
        free(line_tmp);
        return FILE_END;
    }
    while (!feof(file) && ch != '\n')
    {
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = realloc(line_tmp, size);
            if (tmp == NULL)
            {
                free(line_tmp);
                return BAD_ALLOC;
            }
            line_tmp = tmp;
        }
        line_tmp[iter++] = ch;
        ch = getc(file);
    }
    line_tmp[iter] = '\0';
    *line = line_tmp;
    return OK;
}

status_code read_line(char** str)
{
    return fread_line(stdin, str);
}

status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str)
{
    if (src == NULL || delims == NULL || str == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (flags[(int) *ptr] && *ptr != '\0')
    {
        if (cnt + 2 == size)
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        str_tmp[cnt++] = *ptr;
        ++ptr;
    }
    if (end_ptr != NULL)
    {
        *end_ptr = ptr;
    }
    if (inclusive_flag && *ptr != '\0')
    {
        str_tmp[cnt++] = *ptr;
    }
    str_tmp[cnt] = '\0';
    *str = str_tmp;
    return OK;
}


status_code skip_multi_line_comment(FILE* file)
{
    if (file == NULL)
    {
        return NULL_ARG;
    }
    char ch = '{';
    int comment_nesting = 1;
    while (comment_nesting > 0 && !feof(file))
    {
        ch = getc(file);
        if (ch == '{')
        {
            ++comment_nesting;
        }
        else if (ch == '}')
        {
            --comment_nesting;
        }
    }
    if (comment_nesting > 0)
    {
        return FILE_INVALID_CONTENT;
    }
    return OK;
}

status_code fread_cmd(FILE* file, char** str)
{
    if (file == NULL || str == NULL)
    {
        return INVALID_INPUT;
    }
    ull iter = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    int skip_flag = 1;
    while (skip_flag)
    {
        while (ch == ' ' || ch == '\t' || ch == '\n')
        {
            ch = getc(file);
        }
        skip_flag = 0;
        if (ch == '%')
        {
            while (ch != '\n' && !feof(file))
            {
                ch = getc(file);
            }
            ch = getc(file);
            skip_flag = 1;
        }
        else if (ch == '{')
        {
            status_code err_code = skip_multi_line_comment(file);
            if (err_code)
            {
                free(str_tmp);
                return err_code;
            }
            ch = getc(file);
            skip_flag = 1;
        }
    }
    if (feof(file))
    {
        free(str_tmp);
        return FILE_END;
    }
    while (ch != ';' && !feof(file))
    {
        if (iter + 2 == size)
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        str_tmp[iter++] = ch;
        ch = getc(file);
        if (ch == '{')
        {
            status_code err_code = skip_multi_line_comment(file);
            if (err_code)
            {
                free(str_tmp);
                return err_code;
            }
            ch = getc(file);
        }
    }
    if (ch != ';')
    {
        free(str_tmp);
        return FILE_INVALID_CONTENT;
    }
    str_tmp[iter++] = ';';
    str_tmp[iter] = '\0';
    *str = str_tmp;
    return OK;
}

status_code validate_var_name(const char* var_name)
{
    if (var_name == NULL)
    {
        return NULL_ARG;
    }
    if (var_name[0] == '\0' || isdigit(var_name[0]))
    {
        return INVALID_INPUT;
    }
    for (ull i = 0; var_name[i]; ++i)
    {
        if (!isalnum(var_name[i]))
        {
            return INVALID_INPUT;
        }
    }
    return OK;
}

status_code parse_dict_str_double(const char* src, ull* cnt, pair_str_double** dict)
{
    if (src == NULL || cnt == NULL || dict == NULL)
    {
        return NULL_ARG;
    }
    *cnt = 0;
    for (ull i = 0; src[i]; ++i)
    {
        if (src[i] == ':')
        {
            ++(*cnt);
        }
    }
    *dict = (pair_str_double*) malloc(sizeof(pair_str_double) * *cnt);
    if (*dict == NULL)
    {
        return BAD_ALLOC;
    }
    for (ull i = 0; i < *cnt; ++i)
    {
        (*dict)[i].str = NULL;
    }
    // format reminder: {x:-1.11,y:2.1,z:0.81}
    const char* ptr = src;
    status_code err_code = *(ptr++) == '{' ? OK : INVALID_INPUT;
    for (ull i = 0; i < *cnt && !err_code; ++i)
    {
        char* raw_double = NULL;
        err_code = err_code ? err_code : sread_until(ptr, ":", 0, &ptr, &(*dict)[i].str);
        err_code = err_code ? err_code : validate_var_name((*dict)[i].str);
        err_code = err_code ? err_code : (*(ptr++) != '\0' ? OK : INVALID_INPUT);
        err_code = err_code ? err_code : sread_until(ptr, ",}", 0, &ptr, &raw_double);
        err_code = err_code ? err_code : (*(ptr) != '\0' ? OK : INVALID_INPUT);
        ptr = (i + 1 < *cnt) ? (ptr + 1) : ptr; // i + 1 < *cnt => ',' is expected to be skipped
        err_code = err_code ? err_code : parse_double(raw_double, &(*dict)[i].val);
        free(raw_double);
    }
    err_code = err_code ? err_code : (*(ptr++) == '}' ? OK : INVALID_INPUT);
    err_code = err_code ? err_code : (*ptr == '\0' ? OK : INVALID_INPUT);
    if (err_code)
    {
        for (ull i = 0; i < *cnt; ++i)
        {
            free((*dict)[i].str);
        }
        free(*dict);
        *cnt = 0;
        *dict = NULL;
    }
    return err_code;
}


int is_word(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    for (ull i = 0; str[i]; ++i)
    {
        if (!isalpha(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_number(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    if (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')
    {
        return 0;
    }
    if ((str[0] == '-' || str[0] == '+') && str[1] == '\0')
    {
        return 0;
    }
    for (ull i = 1; str[i]; ++i)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int ctoi(char ch)
{
    if (isdigit(ch))
    {
        return ch - '0';
    }
    else if (isalpha(ch))
    {
        return toupper(ch) - 'A' + 10;
    }
    return -1;
}

char itoc(int number)
{
    if (number >= 0 && number < 10)
    {
        return '0' + number;
    }
    else if (number >= 10 && number < 36)
    {
        return 'A' + number - 10;
    }
    return '\0';
}

status_code tolowern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? tolower(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code touppern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? toupper(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code erase_delims(const char* src, const char* delims, char** res)
{
    if (src == NULL || delims == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (*ptr != '\0')
    {
        if (cnt + 2 == size && flags[(int) *ptr])
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        if (flags[(int) *ptr])
        {
            str_tmp[cnt++] = *ptr;
        }
        ++ptr;
    }
    str_tmp[cnt] = '\0';
    *res = str_tmp;
    return OK;
}

status_code parse_llong(const char* src, int base, ll* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoll(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return OVERFLOW;
    }
    return OK;
}

status_code parse_ullong(const char* src, int base, ull* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoull(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return OVERFLOW;
    }
    return OK;
}

status_code parse_double(const char* src, double* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    char* ptr;
    *number = strtod(src, &ptr);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    return OK;
}

status_code convert_ullong(ull number, int base, char res[65])
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base < 2 || base > 36)
    {
        return INVALID_BASE;
    }
    for (ll i = 63; i >= 0; --i)
    {
        res[i] = itoc(number % base);
        number /= base;
    }
    res[64] = '\0';
    return OK;
}

int str_comparator(const void* ptr_1, const void* ptr_2)
{
    const char** str_ptr_1 = (const char**) ptr_1;
    const char** str_ptr_2 = (const char**) ptr_2;
    return strcmp(*str_ptr_1, *str_ptr_2);
}


int sign(ll number)
{
    return number == 0 ? 0 : (number > 0 ? 1 : -1);
}

status_code add_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_1 > LLONG_MAX - arg_2))
    {
        return OVERFLOW;
    }
    if (arg_2 < 0 && (arg_2 < LLONG_MIN - arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 + arg_2;
    return OK;
}

status_code sub_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_2 < LLONG_MIN + arg_2))
    {
        return OVERFLOW;
    }
    if (arg_2 < 0 && (arg_1 > LLONG_MAX + arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 - arg_2;
    return OK;
}

status_code mult_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if ((arg_1 == LLONG_MIN && arg_2 != 1) || (arg_2 == LLONG_MIN && arg_1 != 1))
    {
        return OVERFLOW;
    }
    if (llabs(arg_1) > LLONG_MAX / llabs(arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 * arg_2;
    return OK;
}

status_code div_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 == 0)
    {
        return DIVISION_BY_ZERO;
    }
    *res = arg_1 / arg_2;
    return OK;
}

status_code bpow_safely(ll base, ll pow, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (pow < 0)
    {
        return INVALID_INPUT;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    ll res_tmp = 1;
    ll mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            status_code err_code = mult_safely(res_tmp, mult, &res_tmp);
            if (err_code)
            {
                return err_code;
            }
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = res_tmp;
    return OK;
}

status_code fbpow_safely(double base, ll pow, double* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    if (pow == 0)
    {
        *res = 1;
        return OK;
    }
    int pow_sign = sign(pow);
    pow = llabs(pow);
    double res_tmp = 1;
    double mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            res_tmp *= mult;
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = pow_sign == 1 ? res_tmp : 1.0 / res_tmp;
    return OK;
}

size_t calc_default_str_hash(const char* str)
{
    if (str == NULL)
    {
        return 0;
    }
    size_t res = 0;
    for (ull i = 0; str[i]; ++i)
    {
        res *= DEFAULT_HASH_PARAM;
        res += ctoi(str[i]);
    }
    return res;
}

status_code iso_time_add(const char time[21], ull add_s, char res[21])
{
    if (time == NULL || res == NULL)
    {
        return NULL_ARG;
    }

    char time_tmp[21];
    strcpy(time_tmp, time);
    time_tmp[4] = time_tmp[7] = time_tmp[10] = time_tmp[13] = time_tmp[16] = time_tmp[19] = '\0';
    struct tm t;
    t.tm_sec = atoi(time_tmp + 17);
    t.tm_min = atoi(time_tmp + 14);
    t.tm_hour = atoi(time_tmp + 11);
    t.tm_mday = atoi(time_tmp + 8);
    t.tm_mon = atoi(time_tmp + 5) - 1;
    t.tm_year = atoi(time_tmp) - 1900;

    t.tm_sec += add_s;
    mktime(&t);

    res[0] = '0' + ((t.tm_year + 1900) / 1000);
    res[1] = '0' + ((t.tm_year + 1900) / 100 % 10);
    res[2] = '0' + ((t.tm_year + 1900) / 10 % 100);
    res[3] = '0' + ((t.tm_year + 1900) / 1 % 1000);

    res[5] = '0' + ((t.tm_mon + 1) / 10);
    res[6] = '0' + ((t.tm_mon + 1) % 10);

    res[8] = '0' + (t.tm_mday / 10);
    res[9] = '0' + (t.tm_mday % 10);

    res[11] = '0' + (t.tm_hour / 10);
    res[12] = '0' + (t.tm_hour % 10);

    res[14] = '0' + (t.tm_min / 10);
    res[15] = '0' + (t.tm_min % 10);

    res[17] = '0' + (t.tm_sec / 10);
    res[18] = '0' + (t.tm_sec % 10);

    res[4] = res[7] = '-';
    res[10] = 'T';
    res[13] = res[16] = ':';
    res[19] = 'Z';
    res[20] = '\0';

    return OK;
}

