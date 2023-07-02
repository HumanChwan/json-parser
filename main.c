#include <stdio.h>

#include "String.h"
#include "Object.h"

int main() {
    struct Object obj = create_object();
    struct Object nest = create_object();

    String key = create_string("Hello");
    String value = create_string("World");

    set_string_for_key(&obj, key, &value);
    set_string_for_key(&nest, key, &value);

    pop_char_from_string(&key);

    set_object_for_key(&obj, key, &nest);

    int32_t age = 12;
    set_int_for_key(&obj, create_string("age"), &age);

    FILE* fp = fopen("test.json", "w");

    dump_json(obj, fp);
}
