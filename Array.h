#ifndef JSON_ARRAY_
#define JSON_ARRAY_

#include <stdint.h>
#include <stddef.h>

#include "types.h"
#include "String.h"

struct Object;
struct Element;

#define ARRAY_INIT_CAPACITY 128
#define ARRAY_M_FACTOR 2

struct Array {
    struct Element* a;
    size_t size, capacity;
};

typedef struct Array Array;


Array create_array_from_stack(size_t size, struct Element* arr);
Array create_array(size_t size);
Array create_empty_array();

void delete_array(Array array);

Array copy_array(Array other);

struct Element get_struct_array_element(Array* array, size_t index);
char get_char_element(Array *array, size_t index);
int32_t get_int32_t_element(Array *array, size_t index);
int64_t get_int64_t_element(Array *array, size_t index);
String get_string_element(Array *array, size_t index);
struct Object get_object_element(Array *array, size_t index);
Array get_array_element(Array* array, size_t index);

void set_struct_array_element(Array* array, size_t index, enum JSONType type, void* value);
void set_char_element(Array *array, size_t index, char x);
void set_int32_t_element(Array *array, size_t index, int32_t x);
void set_int64_t_element(Array *array, size_t index, int64_t x);
void set_string_element(Array *array, size_t index, String x);
void set_object_element(Array *array, size_t index, struct Object x);
void set_array_element(Array* array, size_t index, Array x);

void array_push_back(Array *array, enum JSONType type, void* value);
void array_char_push_back(Array *array, char x);
void array_int32_t_push_back(Array *array, int32_t x);
void array_int64_t_push_back(Array *array, int64_t x);
void array_string_push_back(Array *array, String x);
void array_object_push_back(Array *array, struct Object x);
void array_array_push_back(Array *array, Array x);
#endif // JSON_ARRAY_

