#ifndef JSON_PARSER_STRING_
#define JSON_PARSER_STRING_

#include <stddef.h>

#define STRING_INIT_CAPACITY 128
#define STRING_M_FACTOR 2

#define FMT_STRING(s) (s).s

typedef struct  {
    size_t size, capacity;
    char* s;
} String;

String create_string(char* s);
String create_empty_string();
void append_string(String* str, String src);
void append_c_string(String* str, char* cat);
char pop_back(String* str);

#endif // JSON_PARSER_STRING_
