#ifndef JSON_PARSER_STRING_
#define JSON_PARSER_STRING_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define STRING_INIT_CAPACITY 128
#define STRING_M_FACTOR 2

#define FMT_STRING(...) (__VA_ARGS__).s

typedef struct  {
    size_t size, capacity;
    char* s;
} String;

String create_string(char* s);
String create_empty_string();
void delete_string(String s);

String copy_string(String s);

void append_string(String* str, String src);
void append_c_string(String* str, char* cat);
char pop_char_from_string(String* str);

bool string_equals(String a, String b);
String safe_string(String s);

#endif // JSON_PARSER_STRING_
