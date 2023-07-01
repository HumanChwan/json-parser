#include "Object.h"
#include "String.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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
    memset(arr, 0, arr_size);

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
    memset(arr, 0, HASHMAP_INIT_SIZE);

    return (struct Object){.arr=arr, .arr_size=HASHMAP_INIT_SIZE, .element_count=0};
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

int get_int_for_key(struct Object* obj, String key) { return *(int*)get_value_for_key(obj, key); }
double get_double_for_key(struct Object* obj, String key) { return *(double*)get_value_for_key(obj, key); }
char get_char_for_key(struct Object* obj, String key) { return *(char*)get_value_for_key(obj, key); }
String get_string_for_key(struct Object* obj, String key) { return *(String*)get_value_for_key(obj, key); }
struct Object get_object_for_key(struct Object* obj, String key) { return *(struct Object*)get_value_for_key(obj, key); }

enum JSONType get_type_for_key(struct Object *obj, String key) {
    struct KeyValuePair* kvp = get_kvp_for_key(obj, key);

    return (kvp != NULL ? kvp->type : JSON_TYPE_NAT);
}

void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value) {
    uint32_t hash = _hash_string(key);

    size_t index = hash & (obj->arr_size - 1);

    struct NodeKVP* node = malloc(sizeof(struct NodeKVP));
    node->NODE = (struct KeyValuePair){key, type, value};
    node->NEXT = NULL;

    _add_to_index(obj->arr, index, node);
    obj->element_count++;

    if (obj->element_count >= REHASH_FACTOR * obj->arr_size) {
        _rehash_object(obj);
    }
}

