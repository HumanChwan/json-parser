#include <stdio.h>

#include "Object.h"
#include "Array.h"
#include "String.h"
#include "types.h"

int main(void) {
    struct Element el;
    struct Object parent;
    Array arr;
    FILE* fp;

    fp = fopen("test.json", "w");
    parent = create_object();
    arr = create_array(5);

    printf("Created empty JSON object, adding to array...\n");

    for (size_t i = 0; i < 5; ++i) {
        struct Object obj = create_object();

        set_string_for_key(&obj, IMM_STRING("hello"), IMM_STRING("world"));
        set_int32_t_for_key(&obj, IMM_STRING("number"), i);

        set_object_element(&arr, i, obj);

        delete_object(obj);
    }

    set_array_for_key(&parent, IMM_STRING("array"), arr);
    
    el.type = JSON_TYPE_OBJECT;
    el.value = &parent;

    printf("Object created! Saving to file...\n");

    dump_json(fp, el, 4);

    fclose(fp);
}
