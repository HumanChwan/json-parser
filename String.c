#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "String.h"

String create_string(char* s) {
    size_t s_size = strlen(s) + 1;
    char* new_str = malloc(sizeof(char) * s_size);

    strncpy(new_str, s, s_size);

    return (String){.size=s_size - 1, .capacity=s_size, .s=new_str};
}

String create_string_from_stack(char *s) {
    size_t s_size = strlen(s) + 1;

    return (String){.size=s_size - 1, .capacity=s_size, .s=s};
}

String create_empty_string() {
    char* new_str = malloc(sizeof(char) * STRING_INIT_CAPACITY);
    new_str[0] = '\0';
    return (String){.size=0, .capacity=STRING_INIT_CAPACITY, .s=new_str};
}

void delete_string(String s) {
    // WARN: Could raise an error if the string is created on stck
    if (s.s != NULL)
        free(s.s);
}

String copy_string(String s) {
    char* new_str = malloc(s.capacity * sizeof(char));
    strncpy(new_str, s.s, s.capacity);

    return (String){.size=s.size, .capacity=s.capacity, .s=new_str};
}

void append_string(String* str, String src) {
    short change = 0; 
    while (src.size + str->size >= str->capacity) {
        str->capacity *= STRING_M_FACTOR;
        change = 1;
    }

    if (change)
        str->s = realloc(str->s, str->capacity);

    if (str->s == NULL)
        exit(1);

    strncat(str->s, src.s, src.size);
    str->size += src.size;
}

void append_char_to_string(String* str, char c) {
    if (1 + str->size >= str->capacity) {
        str->capacity *= STRING_M_FACTOR;
        str->s = realloc(str->s, str->capacity);
    }

    if (str->s == NULL) {
        fprintf(stderr, "[ERROR]: Buy RAM lol\n");
        exit(1);
    }

    str->s[str->size] = c;
    str->s[++str->size] = '\0';
}

void append_c_string(String* str, char* cat) {
    append_string(str, IMM_STRING(cat));
}

char pop_char_from_string(String* str) {
    if (str->size <= 0) return '\0';

    char element = str->s[str->size - 1];

    str->s[--str->size] = '\0';

    return element;
}

bool string_equals(String a, String b) {
    return strcmp(a.s, b.s) == 0;
}

String safe_string(String s) {
    String safe = create_empty_string();
    for (size_t i = 0; i < s.size; ++i) {
        char c = s.s[i];
        switch (c) {
            case '\\':
                append_c_string(&safe, "\\\\");
                break;
            case '"':
                append_c_string(&safe, "\\\"");
                break;
            case '\n':
                append_c_string(&safe, "\\n");
                break;
            case '\b':
                append_c_string(&safe, "\\b");
                break;
            case '\f':
                append_c_string(&safe, "\\f");
                break;
            case '\r': 
                append_c_string(&safe, "\\r");
                break;
            case '\t':
                append_c_string(&safe, "\\t");
                break;
            default:
                append_char_to_string(&safe, c);
        }
    }
    return safe;
}

