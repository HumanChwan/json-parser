#ifndef JSON_PARSER_OBJECT_
#define JSON_PARSER_OBJECT_

#include <stdint.h>
#include <stdio.h>

#include "String.h"

#include "types.h"

#define HASHMAP_INIT_SIZE 16

// Be careful while changing this.
// Indexing is based on the masking because of this being 2 
#define HASHMAP_M_FACTOR 2

#define REHASH_FACTOR 0.75f

struct KeyValuePair {
    String key;    
    enum JSONType type;
    void *value;
};

struct NodeKVP {
    struct KeyValuePair NODE; 
    struct NodeKVP* NEXT;
};

struct Object {
    struct NodeKVP** arr;
    size_t element_count, arr_size;
};

// To be used by user, "API"
struct Object create_object();
void delete_object(struct Object obj);

struct Object copy_object(struct Object obj);

struct KeyValuePair* get_kvp_for_key(struct Object* obj, String key);
enum JSONType get_type_for_key(struct Object* obj, String key);

void* get_value_for_key(struct Object* obj, String key);

// DO NOT USE THESE FUNCTIONS IF NOT SURE ABOUT THE TYPE
int get_int_for_key(struct Object* obj, String key);
int64_t get_int64_t_for_key(struct Object* obj, String key);
double get_double_for_key(struct Object* obj, String key);
char get_char_for_key(struct Object* obj, String key);
String* get_string_for_key(struct Object* obj, String key);
struct Object* get_object_for_key(struct Object* obj, String key);

void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value);

// DO NOT USE THESE FUNCTIONS IF NOT SURE ABOUT THE TYPE
void set_int_for_key(struct Object* obj, String key, int x);
void set_int64_t_for_key(struct Object* obj, String key, int64_t x);
void set_double_for_key(struct Object* obj, String key, double x);
void set_char_for_key(struct Object* obj, String key, char x);
void set_string_for_key(struct Object* obj, String key, String x);
void set_object_for_key(struct Object* obj, String key, struct Object x);

void dump_json(FILE* fp, struct Object obj, size_t depth, size_t indent);
struct Object load_json(FILE* fp);

#endif // JSON_PARSER_OBJECT_
