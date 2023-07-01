#include <stdio.h>

#include "String.h"
#include "Object.h"

int main() {
    struct Object obj = create_object();

    String key = create_string("Hello");
    String value = create_string("World");

    set_value_for_key(&obj, key, JSON_TYPE_STRING, (void*)&value);

    String s = get_string_for_key(&obj, key);

    printf("String: %s\n", FMT_STRING(s));
}
