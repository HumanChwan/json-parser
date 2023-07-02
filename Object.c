#include "Object.h"
#include "String.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <stdio.h>

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
        printf("hello World\n");

        while (iter != NULL) {
            printf("hello World\n");
            switch (iter->NODE.type) {
                case JSON_TYPE_INTEGER:
                    set_int_for_key(&ret, iter->NODE.key, *(int*)iter->NODE.value);
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
            // TODO: Handle Arrays differently
            if (iter->NODE.type == JSON_TYPE_OBJECT)
                delete_object(*(struct Object*)iter->NODE.value);
            
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

int get_int_for_key(struct Object* obj, String key) { 
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

enum JSONType get_type_for_key(struct Object *obj, String key) {
    struct KeyValuePair* kvp = get_kvp_for_key(obj, key);

    return (kvp != NULL ? kvp->type : JSON_TYPE_NAT);
}

void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value) {
    uint32_t hash = _hash_string(key);
    size_t index = hash & (obj->arr_size - 1);

    String copy_key = copy_string(key);

    struct NodeKVP* node = malloc(sizeof(struct NodeKVP));
    node->NODE = (struct KeyValuePair){copy_key, type, value};
    node->NEXT = NULL;

    _add_to_index(obj->arr, index, node);
    obj->element_count++;

    if (obj->element_count >= REHASH_FACTOR * obj->arr_size) {
        _rehash_object(obj);
    }
}

void set_int_for_key(struct Object* obj, String key, int32_t _x) {
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


void dump_json(struct Object obj, FILE* fp) {
    bool first = true;

    fprintf(fp, "{");

    for (size_t i = 0; i < obj.arr_size; ++i) {
        struct NodeKVP* iter = obj.arr[i];

        while (iter != NULL) {
            if (!first) fprintf(fp, ",");
            else first = false;

            fprintf(fp, "\"%s\":", FMT_STRING(safe_string(iter->NODE.key)));

            switch (iter->NODE.type) {
                case JSON_TYPE_OBJECT:
                    dump_json(*(struct Object*)iter->NODE.value, fp);
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

    fprintf(fp, "}");
}
