#include "Object.h"
#include "Array.h"
#include "String.h"
#include "types.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <stdio.h>
#include <ctype.h>

#include <assert.h>

// Some JAVA code bruh moment, don't know shit about the math behind it.
// Something to do with the lower bits being not very random. 
uint32_t _hash_uniform(uint32_t h) {
    h ^= (h >> 20) ^ (h >> 12);

    return h ^ (h >> 7) ^ (h >> 4);
}

#define ROLL_HASH_PRIME_BASE 263
#define ROLL_HASH_PRIME_MOD 1000000009

uint32_t _hash_string(String s) {
    uint64_t m = 1, hash = 0;

    for (int32_t i = (int32_t)s.size - 1; i >= 0; --i) {
        hash = (hash + m * (int8_t)s.s[i]) % ROLL_HASH_PRIME_MOD;
        m = (m * ROLL_HASH_PRIME_BASE) % ROLL_HASH_PRIME_MOD;
    }

    return _hash_uniform(hash);
}

void _add_to_index(struct NodeKVP** arr, size_t index, struct NodeKVP* node) {
    struct NodeKVP** last = &arr[index];

    while (*last != NULL) {
        if (string_equals((*last)->NODE.key, node->NODE.key)) {
            node->NEXT = (*last)->NEXT;
            break;
        }
        last = &(*last)->NEXT;
    }

    *last = node;
}

void _rehash_object(struct Object* obj) {
    size_t arr_size = obj->arr_size * HASHMAP_M_FACTOR;
    struct NodeKVP** arr = malloc(sizeof(struct NodeKVP*) * arr_size);

    // Setting all elements to NULL just in case.
    for (size_t i = 0; i < arr_size; ++i) arr[i] = NULL;

    for (size_t i = 0; i < obj->arr_size; ++i) {
        struct NodeKVP* iter = obj->arr[i];
        while (iter != NULL) {
            uint32_t hash = _hash_string(iter->NODE.key);
            size_t index = hash & (arr_size - 1);

            struct NodeKVP* temp = iter->NEXT;

            iter->NEXT = NULL;
            _add_to_index(arr, index, iter);

            iter = temp;
        }
    }

    free(obj->arr);

    obj->arr_size = arr_size;
    obj->arr = arr;
}

struct Object create_object() {
    struct NodeKVP** arr = malloc(sizeof(struct NodeKVP*) * HASHMAP_INIT_SIZE);

    // Setting all elements to NULL just in case.
    for (size_t i = 0; i < HASHMAP_INIT_SIZE; ++i) arr[i] = NULL;

    return (struct Object){.arr=arr, .arr_size=HASHMAP_INIT_SIZE, .element_count=0};
}

struct Object copy_object(struct Object obj) {
    struct NodeKVP** arr = malloc(sizeof(struct NodeKVP*) * obj.arr_size);
    
    for (size_t i = 0; i < obj.arr_size; ++i)
        arr[i] = NULL;

    struct Object ret = obj;

    ret.arr = arr;

    for (size_t i = 0; i < obj.arr_size; ++i) {
        struct NodeKVP* iter = obj.arr[i];

        while (iter != NULL) {
            switch (iter->NODE.type) {
                case JSON_TYPE_INTEGER:
                    set_int32_t_for_key(&ret, iter->NODE.key, *(int*)iter->NODE.value);
                    break;
                case JSON_TYPE_LONG:
                    set_int64_t_for_key(&ret, iter->NODE.key, *(int64_t*)iter->NODE.value);
                    break;
                case JSON_TYPE_CHAR:
                    set_char_for_key(&ret, iter->NODE.key, *(char*)iter->NODE.value);
                    break;
                case JSON_TYPE_DOUBLE:
                    set_double_for_key(&ret, iter->NODE.key, *(double*)iter->NODE.value);
                    break;
                case JSON_TYPE_STRING:
                    set_string_for_key(&ret, iter->NODE.key, *(String*)iter->NODE.value);
                    break;
                case JSON_TYPE_OBJECT:
                    set_object_for_key(&ret, iter->NODE.key, *(struct Object*)iter->NODE.value);
                    break;
                case JSON_TYPE_ARRAY:
                    set_array_for_key(&ret, iter->NODE.key, *(Array*)iter->NODE.value);
                    break;
                default:
                    fprintf(stderr, "[ERROR]: Encountered JSON_TYPE_NAT or others");
                    exit(1);
                    break;
            }

            iter = iter->NEXT;
        }
    }
    return ret;
}

void delete_object(struct Object obj) {
    for (size_t i = 0; i < obj.arr_size; ++i) {
        struct NodeKVP* iter = obj.arr[i];

        while (iter != NULL) {
            if (iter->NODE.type == JSON_TYPE_OBJECT)
                delete_object(*(struct Object*)iter->NODE.value);
            else if (iter->NODE.type == JSON_TYPE_STRING)
                delete_string(*(String*)iter->NODE.value);
            else if (iter->NODE.type == JSON_TYPE_ARRAY)
                delete_array(*(Array*)iter->NODE.value);
            
            struct NodeKVP* temp = iter->NEXT;

            if (iter->NODE.value != NULL)
                free(iter->NODE.value);

            free(iter);

            iter = temp;
        }
    }
}

struct KeyValuePair* get_kvp_for_key(struct Object *obj, String key) {
    uint32_t hash = _hash_string(key);
    size_t index = hash & (obj->arr_size - 1);

    struct NodeKVP* iter = obj->arr[index];

    while (iter != NULL) {
        if (string_equals(iter->NODE.key, key)) {
            return &iter->NODE;
        }

        iter = iter->NEXT;
    }

    return NULL;
}

// Worst case O(N) when many collisions, but I guess should be fine enough.
void* get_value_for_key(struct Object* obj, String key) {
    struct KeyValuePair* kvp = get_kvp_for_key(obj, key);

    return (kvp != NULL ? kvp->value : NULL);
}

int get_int32_t_for_key(struct Object* obj, String key) { 
    return *(int*)get_value_for_key(obj, key);
}
int64_t get_int64_t_for_key(struct Object* obj, String key) { 
    return *(int64_t*)get_value_for_key(obj, key);
}
double get_double_for_key(struct Object* obj, String key) { 
    return *(double*)get_value_for_key(obj, key);
}
char get_char_for_key(struct Object* obj, String key) { 
    return *(char*)get_value_for_key(obj, key);
}
String* get_string_for_key(struct Object* obj, String key) { 
    return (String*)get_value_for_key(obj, key);
}
struct Object* get_object_for_key(struct Object* obj, String key) { 
    return (struct Object*)get_value_for_key(obj, key);
}
Array* get_array_for_key(struct Object* obj, String key) {
    return (Array*)get_value_for_key(obj, key);
}

enum JSONType get_type_for_key(struct Object *obj, String key) {
    struct KeyValuePair* kvp = get_kvp_for_key(obj, key);

    return (kvp != NULL ? kvp->type : JSON_TYPE_NAT);
}

void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value) {
    uint32_t hash = _hash_string(key);
    size_t index = hash & (obj->arr_size - 1);

    struct NodeKVP* node = malloc(sizeof(struct NodeKVP));
    node->NODE = (struct KeyValuePair){copy_string(key), type, value};
    node->NEXT = NULL;

    _add_to_index(obj->arr, index, node);
    obj->element_count++;

    if (obj->element_count >= REHASH_FACTOR * obj->arr_size) {
        _rehash_object(obj);
    }
}

void set_int32_t_for_key(struct Object* obj, String key, int32_t _x) {
    int32_t* x = malloc(sizeof(int32_t));
    *x = _x;

    set_value_for_key(obj, key, JSON_TYPE_INTEGER, (void*)x);
}
void set_int64_t_for_key(struct Object* obj, String key, int64_t _x) {
    int64_t* x = malloc(sizeof(int64_t));
    *x = _x;

    set_value_for_key(obj, key, JSON_TYPE_LONG, (void*)x);
}
void set_double_for_key(struct Object* obj, String key, double _x) {
    double* x = malloc(sizeof(double));
    *x = _x;

    set_value_for_key(obj, key, JSON_TYPE_DOUBLE, (void*)x);
}
void set_char_for_key(struct Object* obj, String key, char _x) {
    char* x = malloc(sizeof(char));
    *x = _x;

    set_value_for_key(obj, key, JSON_TYPE_CHAR, (void*)x);
}
void set_string_for_key(struct Object* obj, String key, String _x) {
    String* x = malloc(sizeof(String));
    *x = copy_string(_x);

    set_value_for_key(obj, key, JSON_TYPE_STRING, (void*)x);
}
void set_object_for_key(struct Object* obj, String key, struct Object _x) {
    struct Object *x = malloc(sizeof(struct Object));
    *x = copy_object(_x);

    set_value_for_key(obj, key, JSON_TYPE_OBJECT, (void*)x);
}
void set_array_for_key(struct Object* obj, String key, Array _x) {
    Array *x = malloc(sizeof(Array));
    *x = copy_array(_x);

    set_value_for_key(obj, key, JSON_TYPE_ARRAY, (void*)x);
}

void dump_array(FILE* fp, Array arr, size_t depth, size_t indent) {
    bool first = true;

    fprintf(fp, "[");

    for (size_t i = 0; i < arr.size; ++i) {
        struct ArrayElement el = get_struct_array_element(&arr, i);

        if (!first) fprintf(fp, ",");
        else first = false;

        if (indent != 0)
            fprintf(fp, "\n");

        for (size_t i = 0; i < (depth + 1) * indent; ++i)
            fprintf(fp, " ");

        switch (el.type) {
            case JSON_TYPE_OBJECT:
                dump_json(fp, *(struct Object*)el.value, depth + 1, indent);
                break;
            case JSON_TYPE_ARRAY:
                dump_array(fp, *(Array*)el.value, depth + 1, indent);
                break;
            case JSON_TYPE_CHAR:
                fprintf(fp, "\"%c\"", *(char*)el.value);
                break;
            case JSON_TYPE_INTEGER:
                fprintf(fp, "%d", *(int32_t*)el.value);
                break;
            case JSON_TYPE_LONG:
                fprintf(fp, "%lld", *(int64_t*)el.value);
                break;
            case JSON_TYPE_DOUBLE:
                fprintf(fp, "%lf", *(double*)el.value);
                break;
            case JSON_TYPE_STRING:
                fprintf(fp, "\"%s\"", FMT_STRING(safe_string(*(String*)el.value)));
                break;
            default:
                fprintf(stderr, "[ERROR]: NAT detected");
                break;


        }
    }

    if (indent != 0)
        fprintf(fp, "\n");

    for (size_t i = 0; i < depth * indent; ++i)
        fprintf(fp, " ");
    fprintf(fp, "]");
}


void dump_json(FILE* fp, struct Object obj, size_t depth, size_t indent) {
    bool first = true;

    fprintf(fp, "{");

    for (size_t i = 0; i < obj.arr_size; ++i) {
        struct NodeKVP* iter = obj.arr[i];

        while (iter != NULL) {
            if (!first) fprintf(fp, ",");
            else first = false;

            if (indent != 0)
                fprintf(fp, "\n");

            for (size_t i = 0; i < (depth + 1) * indent; ++i)
                fprintf(fp, " ");

            fprintf(fp, "\"%s\": ", FMT_STRING(safe_string(iter->NODE.key)));

            switch (iter->NODE.type) {
                case JSON_TYPE_OBJECT:
                    dump_json(fp, *(struct Object*)iter->NODE.value, depth + 1, indent);
                    break;
                case JSON_TYPE_ARRAY:
                    dump_array(fp, *(Array*)iter->NODE.value, depth + 1, indent);
                    break;
                case JSON_TYPE_CHAR:
                    fprintf(fp, "\"%c\"", *(char*)iter->NODE.value);
                    break;
                case JSON_TYPE_INTEGER:
                    fprintf(fp, "%d", *(int32_t*)iter->NODE.value);
                    break;
                case JSON_TYPE_LONG:
                    fprintf(fp, "%lld", *(int64_t*)iter->NODE.value);
                    break;
                case JSON_TYPE_DOUBLE:
                    fprintf(fp, "%lf", *(double*)iter->NODE.value);
                    break;
                case JSON_TYPE_STRING:
                    fprintf(fp, "\"%s\"", FMT_STRING(safe_string(*(String*)iter->NODE.value)));
                    break;
                default:
                    fprintf(stderr, "[ERROR]: NAT detected");
                    break;
            }


            iter = iter->NEXT;
        }
    }

    if (indent != 0)
        fprintf(fp, "\n");

    for (size_t i = 0; i < depth * indent; ++i)
        fprintf(fp, " ");
    fprintf(fp, "}");
}

String load_string(FILE* fp) {
    // first character is always '"'
    
    assert(fgetc(fp) == '"' && "Key string must have the first character as \".");

    String key = create_empty_string(); 
    bool escape_init = false;

    char c;
    for (c = fgetc(fp); c != EOF; c = fgetc(fp)) {
        if (escape_init) {
            // TODO: actually escape the characters
            escape_init = false;
        } else if (c == '\\') {
            escape_init = true;
            continue;
        } else if (c == '"') {
            break;
        }

        append_char_to_string(&key, c);
    }

    if (c == EOF) {
        fprintf(stderr,  "[ERROR]: The buffer reached EOF while parsing a key.");
        exit(1);
    }

    return key;
}

struct ArrayElement load_value(FILE* fp, char first_char) {
    // here the first character which would be gotten would not be space-like char. 
    if (first_char == '{') {
        struct Object *value = malloc(sizeof(struct Object));
        *value = load_json(fp);
        return (struct ArrayElement){JSON_TYPE_OBJECT, (void*)value};
    } else if (first_char == '[') {
        Array *value = malloc(sizeof(Array));
        *value = load_array(fp);
        return (struct ArrayElement){JSON_TYPE_ARRAY, (void*)value};
    } else if (first_char == '"') {
        String *value = malloc(sizeof(String));
        *value = load_string(fp);
        return (struct ArrayElement){JSON_TYPE_STRING, (void*)value};
    } else {
        double num = 0;
        bool got_decimal = false;
        double decimal_factor = 0.1;

        for (char c = fgetc(fp); c != EOF; c = fgetc(fp)) {
            if (c == '.') {
                if (got_decimal) {
                    fprintf(stderr, "[ERROR]: A decimal number must only contain single period char.\n");
                    exit(1);
                }
                got_decimal = true;
            } else if (!isdigit(c)) {
                (void)fseek(fp, -1L, SEEK_CUR);
                break;
            }

            short dig = c - '0';

            if (got_decimal) {
                num += dig * decimal_factor;
                decimal_factor /= 10.;
            } else {
                num *= 10;
                num += dig;
            }
        }

        if (got_decimal) {
            // DOUBLE
            double *value = malloc(sizeof(double));
            *value = num;

            return (struct ArrayElement){JSON_TYPE_DOUBLE, (void*)value};
        } else {
            // LONG LONG
            int64_t* value = malloc(sizeof(int64_t));
            *value = num;

            return (struct ArrayElement){JSON_TYPE_LONG, (void*)value};
        }
    }
}

Array load_array(FILE* fp) {
    Array array = create_empty_array();

    enum {
        START,
        ELEMENT_START,
        ELEMENT_END,
        END
    } status = START;

    for (char c = fgetc(fp); c != EOF; c = fgetc(fp)) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue; 

        switch (status) {
            case START:
                if (c != '[') {
                    fprintf(stderr, "[ERROR]: Array doesn't start with `[`.\n");
                    exit(1);
                }
                status = ELEMENT_START;
                break;
            case ELEMENT_START:
                (void)fseek(fp, -1L, SEEK_CUR);
                struct ArrayElement el = load_value(fp, c);
                array_push_back(&array, el.type, el.value);

                status = ELEMENT_END;
                break;
            case ELEMENT_END:
                if (c != ']' && c != ',') {
                    fprintf(stderr, "[ERROR]: Array elements must end with a `,` or `]`\n");
                    exit(1);
                }

                if (c == ']') status = END;
                else status = ELEMENT_START;
                break;
            case END:
                break;
            default:
                fprintf(stderr, "[ERROR]: Array parsing has reached unreachable state.\n Aborting.\n");
                exit(1);
        }

        if (status == END) break;
    }

    if (status != END) {
        fprintf(stderr, "[ERROR]: Array parsing got cancelled before reaching end.\n Aborting.\n");
        exit(1);
    }

    return array;
}

struct Object load_json(FILE *fp) {
    struct Object obj = create_object();

    enum {
        START,
        KEY_START,
        KEY_END,
        VALUE_START,
        VALUE_END,
        END
    } status = START;


    String key;
    for (char c = fgetc(fp); c != EOF; c = fgetc(fp)) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue; 

        switch (status) {
            case START:
                if (c != '{') {
                    fprintf(stderr, "[ERROR]: Object doesn't start with `{`.\n");
                    exit(1);
                }
                status = KEY_START;
                break;
            case KEY_START:
                if (c != '"') {
                    fprintf(stderr, "[ERROR]: Key must be string.\n");
                    exit(1);
                }
                (void)fseek(fp, -1L, SEEK_CUR);
                key = load_string(fp);
                status = KEY_END;
                break;
            case KEY_END:
                if (c != ':') {
                    fprintf(stderr, "[ERROR]: Key must be succeeded by `:`\n");
                    exit(1);
                }
                status = VALUE_START;
                break;
            case VALUE_START:
                (void)fseek(fp, -1L, SEEK_CUR);
                struct ArrayElement el = load_value(fp, c);
                set_value_for_key(&obj, key, el.type, el.value);
                status = VALUE_END;
                break;
            case VALUE_END:
                if (c != ',' && c != '}') {
                    fprintf(stderr, "[ERROR]: Value must be succeeded by `,` or `}`, Found: `%c`\n", c);
                    exit(1);
                }

                if (c == ',') {
                    status = KEY_START;
                    break;
                }

                status = END;
                if (c != '}') {
                    fprintf(stderr, "[ERROR]: End of the object must have `}`\n");
                    exit(1);
                }

                break;
            default:
                fprintf(stderr, "[ERROR]: This shouldn't be possible.\n");
                exit(1);
        }

        // TODO: the file should be checked for characters after the main object. 
        if (status == END) {
            break;
        }
    }

    if (status != END) {
        fprintf(stderr, "[ERROR]: Reached EOF before end of object\n");
        exit(1);
    }

    return obj;
}
