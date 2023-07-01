#include <stdio.h>

#include "String.h"
#include "Object.h"

int main() {
    struct Object obj = create_object();

    struct Object nest = create_object();

    String key = create_string("Hello");
    String value = create_string("World");

    set_value_for_key(&nest, key, JSON_TYPE_STRING, (void*)&value);
    set_value_for_key(&obj, key, JSON_TYPE_OBJECT, (void*)&nest);

    int32_t age = 12;
    set_value_for_key(&obj, create_string("age"), JSON_TYPE_INTEGER, (void*)&age);

    FILE* fp = fopen("test.json", "w");

    dump_json(obj, fp);
}
