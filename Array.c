#include "Array.h"

#include "types.h"
#include "Object.h"

#include <stdlib.h>

Array create_array_from_stack(size_t size, struct ArrayElement* arr) {
    return (Array){.size=size, .capacity=size, .a=arr};
}

Array create_array(size_t size) {
    struct ArrayElement* arr = malloc(sizeof(struct ArrayElement) * size);

    return (Array){.size=size, .capacity=size, .a=arr};
}

Array create_empty_array() {
    return create_array(ARRAY_INIT_CAPACITY);
}

void delete_array(Array array) {
    for (size_t i = 0; i < array.size; ++i) {
        if (array.a[i].type == JSON_TYPE_OBJECT)
            delete_object(*(struct Object*)array.a[i].value);
        else if (array.a[i].type == JSON_TYPE_STRING)
            delete_string(*(String*)array.a[i].value);
        else if (array.a[i].type == JSON_TYPE_ARRAY)
            delete_array(*(Array*)array.a[i].value);

        free(array.a[i].value);
    }

    if (array.a != NULL)
        free(array.a);
}

struct ArrayElement copy_array_element(struct ArrayElement element) {
    struct ArrayElement copy_element = element;
    void* value = NULL;
    switch(element.type) {
        case JSON_TYPE_OBJECT: {
            struct Object* x = malloc(sizeof(struct Object));
            *x  = copy_object(*(struct Object*)element.value);
            value = (void*)x;
            break;
        }
        case JSON_TYPE_CHAR: {
            char* x = malloc(sizeof(char));
            *x  = *(char*)element.value;
            value = (void*)x;
            break;
        }
        case JSON_TYPE_INTEGER: {
            int32_t* x = malloc(sizeof(int32_t));
            *x  = *(int32_t*)element.value;
            value = (void*)x;
            break;
        }
        case JSON_TYPE_LONG: {
            int64_t* x = malloc(sizeof(int64_t));
            *x  = *(int64_t*)element.value;
            value = (void*)x;
            break;
        }
        case JSON_TYPE_ARRAY: {
            Array* x = malloc(sizeof(Array));
            *x  = copy_array(*(Array*)element.value);
            value = (void*)x;
            break;
        }
        case JSON_TYPE_STRING: {
            String* x = malloc(sizeof(String));
            *x  = copy_string(*(String*)element.value);
            value = (void*)x;
            break;
        }
        default:
            fprintf(stderr, "[ERROR]: while copying array got NAT Type or others.\n");
            break;
    }

    copy_element.value = value;

    return copy_element;
}

Array copy_array(Array other) {
    struct ArrayElement* arr = malloc(sizeof(struct ArrayElement) * other.capacity);

    Array new_array = other;
    new_array.a = arr;

    for (size_t i = 0; i < other.size; ++i) {
        arr[i] = copy_array_element(other.a[i]);
    }

    return new_array;
}

struct ArrayElement get_struct_array_element(Array* array, size_t index) {
    if (index >= array->size) {
        fprintf(stderr, "Array overbound access tried. Aborting\n");
        exit(1);
    }

    return array->a[index];
}
char get_char_element(Array *array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(char*)el.value;
}
int32_t get_int32_t_element(Array *array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(int32_t*)el.value;
}
int64_t get_int64_t_element(Array *array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(int64_t*)el.value;
}
String get_string_element(Array *array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(String*)el.value;
}
struct Object get_object_element(Array *array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(struct Object*)el.value;
}
Array get_array_element(Array* array, size_t index) {
    struct ArrayElement el = get_struct_array_element(array, index);
    return *(Array*)el.value;
}

void array_push_back(Array *array, enum JSONType type, void* value) {
    if (array->size + 1 > array->capacity) {
        array->capacity *= ARRAY_M_FACTOR; 
        array->a = realloc(array->a, array->capacity);
    }

    if (array->a == NULL) {
        fprintf(stderr, "[ERROR]: Buy more RAM lol\n");
        exit(1);
    }

    array->a[array->size++] = (struct ArrayElement){type, value};
}
void array_char_push_back(Array *array, char _x) {
    char* x = malloc(sizeof(char));
    *x = _x;

    array_push_back(array, JSON_TYPE_CHAR, (void*)x);
}
void array_int32_t_push_back(Array *array, int32_t _x) {
    int32_t* x = malloc(sizeof(int32_t));
    *x = _x;

    array_push_back(array, JSON_TYPE_INTEGER, (void*)x);
}
void array_int64_t_push_back(Array *array, int64_t _x) {
    int64_t* x = malloc(sizeof(int64_t));
    *x = _x;

    array_push_back(array, JSON_TYPE_LONG, (void*)x);
}
void array_string_push_back(Array *array, String _x) {
    String* x = malloc(sizeof(String));
    *x = copy_string(_x);

    array_push_back(array, JSON_TYPE_STRING, (void*)x);
}
void array_object_push_back(Array *array, struct Object _x) {
    struct Object* x = malloc(sizeof(struct Object));
    *x = copy_object(_x);

    array_push_back(array, JSON_TYPE_OBJECT, (void*)x);
}
void array_array_push_back(Array *array, Array _x) {
    Array* x = malloc(sizeof(Array));
    *x = copy_array(_x);

    array_push_back(array, JSON_TYPE_OBJECT, (void*)x);
}


void set_struct_array_element(Array* array, size_t index, enum JSONType type, void* value) {
    if (index >= array->size) {
        fprintf(stderr, "[ERROR]: Array overbound access detected. Aborting.\n");
        exit(1);
    }

    array->a[index] = copy_array_element((struct ArrayElement){type, value});
}
void set_char_element(Array *array, size_t index, char x) {
    set_struct_array_element(array, index, JSON_TYPE_CHAR, (void*)&x);
}
void set_int32_t_element(Array *array, size_t index, int32_t x) {
    set_struct_array_element(array, index, JSON_TYPE_INTEGER, (void*)&x);
}
void set_int64_t_element(Array *array, size_t index, int64_t x) {
    set_struct_array_element(array, index, JSON_TYPE_LONG, (void*)&x);
}
void set_string_element(Array *array, size_t index, String x) {
    set_struct_array_element(array, index, JSON_TYPE_STRING, (void*)&x);
}
void set_object_element(Array *array, size_t index, struct Object x) {
    set_struct_array_element(array, index, JSON_TYPE_OBJECT, (void*)&x);
}
void set_array_element(Array* array, size_t index, Array x) {
    set_struct_array_element(array, index, JSON_TYPE_ARRAY, (void*)&x);
}
