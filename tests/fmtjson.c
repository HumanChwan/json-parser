#include "../Object.h"
#include "../String.h"
#include "../Array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(void) {
    printf("USAGE:\n\tfmtjson FILE\n\tFILE: a json file");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        exit(1);
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        usage();
        exit(0);
    }

    FILE* fp = fopen(argv[1], "r");
    struct Object obj = load_json(fp);
    fclose(fp);

    printf("Read and parsed `%s`\n", argv[1]);

    fp = fopen(argv[1], "w");
    dump_json(fp, obj, 0, 4);
    fclose(fp);
}