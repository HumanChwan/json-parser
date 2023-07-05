#include <stdio.h>

#include "Object.h"
#include "Array.h"
#include "String.h"

int main(void) {
    struct Object parent = create_object();
    Array arr = create_array(5);

    for (size_t i = 0; i < 5; ++i) {
        struct Object obj = create_object();
        set_string_for_key(&obj, IMM_STRING("hello"), IMM_STRING("world"));
        set_int32_t_for_key(&obj, IMM_STRING("number"), i);

        set_object_element(&arr, i, obj);

        delete_object(obj);
    }

    set_array_for_key(&parent, IMM_STRING("array"), arr);
    
    FILE* fp = fopen("test.json", "w");

    dump_json(fp, parent, 0, 4);
}
