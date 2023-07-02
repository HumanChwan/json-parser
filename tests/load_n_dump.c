#include "../Object.h"
#include "../String.h"

#include <stdio.h>

int main(void) {
    FILE* rfp = fopen("test.json", "r");
    FILE* wfp = fopen("write.json", "w");

    struct Object obj = load_json(rfp);

    dump_json(wfp, obj, 0, 4);
}
