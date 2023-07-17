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

// #define ERROR(x) fprintf(stderr, "%zu:%zu [ERROR]: "x"\n", row, col)
#define ERROR(x) fprintf(stderr, "%zu:%zu [ERROR]: "x"\n", row, col + 1)


size_t row, col;

void reset_read() {
    row = 1, col = 0;
}
void ack_read(char c) {
    if (c == '\n') row++, col = 0;
    else col++;
}

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

    return (struct Object){
        .arr=arr,
        .arr_size=HASHMAP_INIT_SIZE,
        .element_count=0,
        ._count=0,
        ._iter=-1,
        ._ordered=NULL
    };
}

int _compare_kvp(const void* _A, const void* _B) {
    struct KeyValuePair *A = (struct KeyValuePair*)_A, *B = (struct KeyValuePair*)_B;
    return A->_stamp - B->_stamp;
}
struct KeyValuePair* next_element(struct Object* obj) {
    if (obj->_iter != -1) {
        obj->_iter++;
        if (obj->_iter == (int32_t)obj->element_count) {
            free(obj->_ordered);
            obj->_ordered = NULL;
            obj->_iter = -1;

            return NULL;
        }
    } else {
        if (obj->element_count == 0)
            return NULL;
        obj->_ordered = malloc(sizeof(struct KeyValuePair) * obj->element_count);

        size_t idx = 0;
        for (size_t i = 0; i < obj->arr_size; ++i) {
            struct NodeKVP* node_iter = obj->arr[i];

            while (node_iter != NULL) {
                obj->_ordered[idx++] = node_iter->NODE;
                node_iter = node_iter->NEXT;
            }
        }
        assert(idx == obj->element_count && "Total elements must be equal to element_count");
        qsort(obj->_ordered, obj->element_count, sizeof(struct KeyValuePair), _compare_kvp);
        obj->_iter = 0;
    }

    return &obj->_ordered[obj->_iter];
}

struct Object copy_object(struct Object obj) {
    struct NodeKVP** arr = malloc(sizeof(struct NodeKVP*) * obj.arr_size);
    
    for (size_t i = 0; i < obj.arr_size; ++i)
        arr[i] = NULL;

    struct Object ret = obj;

    ret.arr = arr;

    for (size_t i = 0; i < obj.arr_size; ++i) {
        struct NodeKVP* iter = obj.arr[i];

        struct Element element = iter->NODE.element;

        while (iter != NULL) {
            switch (element.type) {
                case JSON_TYPE_INTEGER:
                    set_int32_t_for_key(&ret, iter->NODE.key, *(int*)element.value);
                    break;
                case JSON_TYPE_LONG:
                    set_int64_t_for_key(&ret, iter->NODE.key, *(int64_t*)element.value);
                    break;
                case JSON_TYPE_CHAR:
                    set_char_for_key(&ret, iter->NODE.key, *(char*)element.value);
                    break;
                case JSON_TYPE_DOUBLE:
                    set_double_for_key(&ret, iter->NODE.key, *(double*)element.value);
                    break;
                case JSON_TYPE_STRING:
                    set_string_for_key(&ret, iter->NODE.key, *(String*)element.value);
                    break;
                case JSON_TYPE_OBJECT:
                    set_object_for_key(&ret, iter->NODE.key, *(struct Object*)element.value);
                    break;
                case JSON_TYPE_ARRAY:
                    set_array_for_key(&ret, iter->NODE.key, *(Array*)element.value);
                    break;
                case JSON_TYPE_BOOLEAN:
                    set_bool_for_key(&ret, iter->NODE.key, *(bool*)element.value);
                    break;
                case JSON_TYPE_NULL:
                    set_null_for_key(&ret, iter->NODE.key);
                    break;
                default:
                    ERROR("Encountered NAT or others");
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

        struct Element element = iter->NODE.element;

        while (iter != NULL) {
            if (element.type == JSON_TYPE_OBJECT)
                delete_object(*(struct Object*)element.value);
            else if (element.type == JSON_TYPE_STRING)
                delete_string(*(String*)element.value);
            else if (element.type == JSON_TYPE_ARRAY)
                delete_array(*(Array*)element.value);
            
            struct NodeKVP* temp = iter->NEXT;

            if (element.value != NULL)
                free(element.value);

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

    return (kvp != NULL ? kvp->element.value : NULL);
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
bool get_bool_for_key(struct Object* obj, String key) {
    return *(bool*)get_value_for_key(obj, key);
}

enum JSONType get_type_for_key(struct Object *obj, String key) {
    struct KeyValuePair* kvp = get_kvp_for_key(obj, key);

    return (kvp != NULL ? kvp->element.type : JSON_TYPE_NAT);
}

void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value) {
    uint32_t hash = _hash_string(key);
    size_t index = hash & (obj->arr_size - 1);

    struct NodeKVP* node = malloc(sizeof(struct NodeKVP));
    node->NODE = (struct KeyValuePair){copy_string(key), (struct Element){type, value}, ._stamp=obj->_count++};
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
void set_bool_for_key(struct Object* obj, String key, bool _x) {
    bool *x = malloc(sizeof(bool));
    *x = _x;

    set_value_for_key(obj, key, JSON_TYPE_BOOLEAN, (void*)x);
}
void set_null_for_key(struct Object* obj, String key) {
    set_value_for_key(obj, key, JSON_TYPE_NULL, NULL);
}

#define FILE_READER_BUF 1000
typedef struct {
    FILE* fp; 
    char buffer[FILE_READER_BUF];
    size_t buffer_size;
    size_t buffer_pointer;
} FileReader;

void dump_object(FILE* fp, struct Object obj, size_t depth, size_t indent);
struct Object load_object(FileReader* fr);

void dump_array(FILE* fp, Array arr, size_t depth, size_t indent);
Array load_array(FileReader* fr);

void dump_value(FILE* fp, struct Element el, size_t depth, size_t indent);
struct Element load_value(FileReader* fr, char first_char);

void dump_value(FILE* fp, struct Element el, size_t depth, size_t indent) {
        switch (el.type) {
            case JSON_TYPE_OBJECT:
                dump_object(fp, *(struct Object*)el.value, depth, indent);
                break;
            case JSON_TYPE_ARRAY:
                dump_array(fp, *(Array*)el.value, depth, indent);
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
            case JSON_TYPE_STRING: {
                String safe = safe_string(*(String*)el.value);
                fprintf(fp, "\"%s\"", FMT_STRING(safe));
                delete_string(safe);
                break;
            }
            case JSON_TYPE_BOOLEAN:
                fprintf(fp, "%s", *(bool*)el.value ? "true" : "false");
                break;
            case JSON_TYPE_NULL:
                fprintf(fp, "null");
                break;
            default:
                ERROR("NAT detected");
                exit(1);
                break;


        }
}

void dump_array(FILE* fp, Array arr, size_t depth, size_t indent) {
    bool first = true;

    fprintf(fp, "[");

    for (size_t i = 0; i < arr.size; ++i) {
        struct Element el = get_struct_array_element(&arr, i);

        if (!first) fprintf(fp, ",");
        else first = false;

        if (indent != 0)
            fprintf(fp, "\n");

        for (size_t i = 0; i < (depth + 1) * indent; ++i)
            fprintf(fp, " ");
        
        dump_value(fp, el, depth + 1, indent);
    }

    if (indent != 0)
        fprintf(fp, "\n");

    for (size_t i = 0; i < depth * indent; ++i)
        fprintf(fp, " ");
    fprintf(fp, "]");
}


void dump_object(FILE* fp, struct Object obj, size_t depth, size_t indent) {
    bool first = true;

    fprintf(fp, "{");

    for (struct KeyValuePair *iter = next_element(&obj); iter != NULL; iter = next_element(&obj)) {
        if (!first) fprintf(fp, ",");
        else first = false;

        if (indent != 0)
            fprintf(fp, "\n");

        for (size_t i = 0; i < (depth + 1) * indent; ++i)
            fprintf(fp, " ");

        {
            String safe = safe_string(iter->key);
            fprintf(fp, "\"%s\": ", FMT_STRING(safe));
            delete_string(safe);
        }

        dump_value(fp, iter->element, depth + 1, indent);
    }

    if (indent != 0)
        fprintf(fp, "\n");

    for (size_t i = 0; i < depth * indent; ++i)
        fprintf(fp, " ");
    fprintf(fp, "}");
}


void initialise_file_reader(FileReader* fr, FILE* fp) {
    fr->buffer_pointer = 0;
    fr->fp = fp;
    fr->buffer_size = fread(fr->buffer, sizeof(char), FILE_READER_BUF, fr->fp);
}

char file_reader_getc(FileReader* fr) {
    if (fr->buffer_pointer < fr->buffer_size) {
        return fr->buffer[fr->buffer_pointer++];
    }
    fr->buffer[0] =  fr->buffer[fr->buffer_size - 1];
    fr->buffer_size = fread(fr->buffer + 1, sizeof(char), FILE_READER_BUF - 1, fr->fp) + 1;
    fr->buffer_pointer = 1;

    return file_reader_getc(fr);
}

void move_pointer_back(FileReader* fr) {
    if (fr->buffer_pointer == 0) {
        fprintf(stderr, "[ERROR]: Requested to move File Reader's pointer behind the buffer start.\n");
        exit(1);
    }
    fr->buffer_pointer--;
}


String load_string(FileReader* fr) {
    // first character is always '"'
    
    if (file_reader_getc(fr) != '"') {
        ERROR("String should have started with `\"` character.");
        exit(1);
    }
    ack_read('"');

    String key = create_empty_string(); 
    bool escape_init = false;

    char c;
    for (c = file_reader_getc(fr); c != EOF; c = file_reader_getc(fr)) {
        ack_read(c);

        if (escape_init) {
            if (c == 'n') append_char_to_string(&key, '\n');
            else if (c == 'b') append_char_to_string(&key, '\b');
            else if (c == 't') append_char_to_string (&key, '\t');
            else if (c == 'f') append_char_to_string(&key, '\f');
            else if (c == 'r') append_char_to_string(&key, '\r');
            else if (c == '\\') append_char_to_string(&key, '\\');
            else if (c == '"') append_char_to_string(&key, '"');
            else {
                append_char_to_string(&key, '\\');
                append_char_to_string(&key, c);
            }
            escape_init = false;
            continue;
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

bool literal_check(FileReader* fr, const char* literal, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        char c = file_reader_getc(fr);
        if (c != literal[i]) {
            ack_read(c);
            return false;
        }
    }

    return true;
}

struct Element load_value(FileReader* fr, char first_char) {
    // here the first character which would be gotten would not be space-like char. 
    if (first_char == '{') {
        struct Object *value = malloc(sizeof(struct Object));
        *value = load_object(fr);
        return (struct Element){JSON_TYPE_OBJECT, (void*)value};
    } else if (first_char == '[') {
        Array *value = malloc(sizeof(Array));
        *value = load_array(fr);
        return (struct Element){JSON_TYPE_ARRAY, (void*)value};
    } else if (first_char == '"') {
        String *value = malloc(sizeof(String));
        *value = load_string(fr);
        return (struct Element){JSON_TYPE_STRING, (void*)value};
    } else if (first_char == 'f') {
        // This must be the literal `false`
        if (!literal_check(fr, "false", 5)) {
            ERROR("Unknown literal found. Aborting.");
            exit(1);
        }

        bool* value = malloc(sizeof(bool));
        *value = false;
        return (struct Element){JSON_TYPE_BOOLEAN, (void*)value};
    } else if (first_char == 't') {
        // This must be the literal `true`
        if (!literal_check(fr, "true", 4)) {
            ERROR("Unknown literal found. Aborting.");
            exit(1);
        }

        bool* value = malloc(sizeof(bool));
        *value = true;
        return (struct Element){JSON_TYPE_BOOLEAN, (void*)value};
    } else if (first_char == 'n') {
        // This must be the literal `null`
        if (!literal_check(fr, "null", 4)) {
            ERROR("Unknown literal found. Aborting.");
            exit(1);
        }

        return (struct Element){JSON_TYPE_NULL, NULL};
    } else {
        double num = 0;
        bool got_decimal = false;
        bool negative = false;
        double DECIMAL_FACTOR = 0.1;

        if (first_char == '-') {
            assert(file_reader_getc(fr) == '-' && "[ERROR]: Wow, that was definitely not expected\n");
            ack_read('-');
            negative = true;
        }

        for (char c = file_reader_getc(fr); c != EOF; c = file_reader_getc(fr)) {
            if (c == '.') {
                ack_read(c);

                if (got_decimal) {
                    ERROR("A decimal number must only contain single period char.");
                    exit(1);
                }
                got_decimal = true;
                continue;
            } else if (!isdigit(c)) {
                move_pointer_back(fr);
                break;
            }

            ack_read(c);

            short dig = c - '0';

            if (got_decimal) {
                num += dig * DECIMAL_FACTOR;
                DECIMAL_FACTOR /= 10.;
            } else {
                num *= 10;
                num += dig;
            }
        }

        if (negative) num *= -1.;

        if (got_decimal) {
            // DOUBLE
            double *value = malloc(sizeof(double));
            *value = num;

            return (struct Element){JSON_TYPE_DOUBLE, (void*)value};
        } else {
            // LONG LONG
            int64_t* value = malloc(sizeof(int64_t));
            *value = num;

            return (struct Element){JSON_TYPE_LONG, (void*)value};
        }
    }
}

Array load_array(FileReader* fr) {
    Array array = create_empty_array();

    enum {
        START,
        ELEMENT_START,
        ELEMENT_END,
        END
    } status = START;

    bool first_element = true;
    for (char c = file_reader_getc(fr); c != EOF; c = file_reader_getc(fr)) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            ack_read(c);
            continue;
        }

        
        switch (status) {
            case START:
                ack_read(c);
                if (c != '[') {
                    ERROR("Array doesn't start with `[`.");
                    exit(1);
                }
                status = ELEMENT_START;
                break;
            case ELEMENT_START:
                if (first_element && c == ']') {
                    status = END;
                    break;
                }
                first_element = false;
                move_pointer_back(fr);
                struct Element el = load_value(fr, c);
                array_push_back(&array, el.type, el.value);

                status = ELEMENT_END;
                break;
            case ELEMENT_END:
                ack_read(c);
                if (c != ']' && c != ',') {
                    ERROR("Array elements must end with a `,` or `]`.");
                    exit(1);
                }

                if (c == ']') status = END;
                else status = ELEMENT_START;
                break;
            case END:
                break;
            default:
                ERROR("Array parsing has reached unreachable state. Aborting.");
                exit(1);
        }

        if (status == END) break;
    }

    if (status != END) {
        ERROR("Array parsing got cancelled before reaching end. Aborting.");
        exit(1);
    }

    return array;
}

struct Object load_object(FileReader* fr) {
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
    bool first_element = true;
    for (char c = file_reader_getc(fr); c != EOF; c = file_reader_getc(fr)) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            ack_read(c);
            continue;
        }

        switch (status) {
            case START:
                ack_read(c);
                if (c != '{') {
                    ERROR("Object doesn't start with `{`.");
                    exit(1);
                }
                status = KEY_START;
                break;
            case KEY_START:
                if (first_element && c == '}') {
                    status = END;
                    break;
                }
                first_element = false;
                if (c != '"') {
                    ERROR("Key must be string.");
                    exit(1);
                }
                move_pointer_back(fr);
                key = load_string(fr);
                status = KEY_END;
                break;
            case KEY_END:
                ack_read(c);
                if (c != ':') {
                    ERROR("Key must be succeeded by `:`");
                    exit(1);
                }
                status = VALUE_START;
                break;
            case VALUE_START:
                move_pointer_back(fr);
                struct Element el = load_value(fr, c);
                set_value_for_key(&obj, key, el.type, el.value);
                status = VALUE_END;
                break;
            case VALUE_END:
                ack_read(c);
                if (c != ',' && c != '}') {
                    ERROR("Value must be succeeded by `,` or `}`.");
                    exit(1);
                }

                if (c == ',') {
                    status = KEY_START;
                    break;
                }

                status = END;
                if (c != '}') {
                    ERROR("End of the object must have `}`");
                    exit(1);
                }

                break;
            default:
                ERROR("This shouldn't be possible.");
                exit(1);
        }

        // TODO: the file should be checked for characters after the main object. 
        if (status == END) {
            break;
        }
    }

    if (status != END) {
        ERROR("Reached EOF before end of object\n");
        exit(1);
    }

    return obj;
}

void dump_json(FILE* fp, struct Element el, size_t indent) {
    reset_read();
    dump_value(fp, el, 0, indent);
}

struct Element load_json(FILE* fp) { 
    reset_read();

    FileReader fr;
    initialise_file_reader(&fr, fp);

    char c = file_reader_getc(&fr);
    move_pointer_back(&fr);

    return load_value(&fr, c);
}
