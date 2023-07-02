#include <stdio.h>

#include "../String.h"
#include "../Object.h"

int main(void) {
    struct Object obj = create_object();

    set_string_for_key(&obj, IMM_STRING("name"), IMM_STRING("Dinesh Chukkala"));

    struct Object address = create_object();
    
    set_string_for_key(&address, IMM_STRING("repo"), IMM_STRING("json-parser"));
    set_string_for_key(&address, IMM_STRING("user"), IMM_STRING("HumanChwan"));
    set_string_for_key(&address, IMM_STRING("host"), IMM_STRING("github.com"));

    set_object_for_key(&obj, IMM_STRING("address"), address);

    FILE* fp = fopen("test.json", "w");

    dump_json(fp, obj, 0, 0);
}
