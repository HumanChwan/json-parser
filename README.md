# JSON Parser in C

## Quick Start

- Compile tests using:
    ```console
    $ make test
    ```
- Run `fmtjson` program with command line argument as an unformatted JSON file:
    ```console
    $ ./fmtjson <FILE>
    ```

## Usage

- It is assumed that all JSON objects start with the object notation, and doesn't have other types as the parent.

- There are two family of functions concerning object key-value addition or extraction.
    - Addition/Setting key-value pair (KVP):
        ```c
        void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value);

        void set_int32_t_for_key(struct Object* obj, String key, int x);
        void set_int64_t_for_key(struct Object* obj, String key, int64_t x);
        void set_double_for_key(struct Object* obj, String key, double x);
        void set_char_for_key(struct Object* obj, String key, char x);
        void set_string_for_key(struct Object* obj, String key, String x);
        void set_object_for_key(struct Object* obj, String key, struct Object x);
        void set_array_for_key(struct Object* obj, String key, Array x);
        void set_bool_for_key(struct Object* obj, String key, bool x);
        void set_null_for_key(struct Object* obj, String key);
        ```
        - PS: It is be noted that the above functions also allow modification of value for the key

    - Extraction of key-value pair (KVP):
        ```c
        void* get_value_for_key(struct Object* obj, String key);

        // DO NOT USE THESE FUNCTIONS IF NOT SURE ABOUT THE TYPE
        int get_int32_t_for_key(struct Object* obj, String key);
        int64_t get_int64_t_for_key(struct Object* obj, String key);
        double get_double_for_key(struct Object* obj, String key);
        char get_char_for_key(struct Object* obj, String key);
        String* get_string_for_key(struct Object* obj, String key);
        struct Object* get_object_for_key(struct Object* obj, String key);
        Array* get_array_for_key(struct Object* obj, String key);
        bool get_bool_for_key(struct Object* obj, String key);
        ```
### Loading and Dumping JSON (basically formatting JSON)

- Consider the file `unformatted.json`:
    ```json
    {"name":"Dinesh Chukkala","address":{"user":"HumanChwan","host":"github.com","repo":"json-parser"}}
    ```
- Compile and run the following code:
    ```c
    #include "Object.h"
    
    #include <stdio.h>

    int main(void) {
        FILE* fp = fopen("unformatted.json", "r");
        struct Object obj = load_json(fp);
        fclose(fp)

        fp = fopen("unformatted.json", "w");
        dump_json(fp, obj, 0,     4);
        //                 ^      ^
        //               depth  indent
        fclose(fp);
    }
    ```
    ```console
    $ gcc -o main main.c String.c Object.c Array.c
    $ ./main
    ```
- Find the file `unformatted.json`:
    ```json
    {
        "name": "Dinesh Chukkala",
        "address": {
            "user": "HumanChwan",
            "host": "github.com",
            "repo": "json-parser"
        }
    }
    ```

### Creating an Object

- Create an object using the following function:
    ```c
    struct Object obj = create_object();
    // Present state of object:
    /*
     * {}
     */
    ```

- Using the setter function family, new KVP can be added or existing Key's value can be modified.
    - Adding String value for a key:
    ```c
    set_string_for_key(&obj, IMM_STRING("name"), IMM_STRING("Dinesh Chukkala"));
    // Present state of object:
    /*
     * {
     *     "name": "Dinesh Chukkala"
     * }
     */
    ```
    - Adding a nested object:
    ```c
    struct Object address = create_object();
    
    set_string_for_key(&address, IMM_STRING("repo"), IMM_STRING("json-parser"));
    set_string_for_key(&address, IMM_STRING("user"), IMM_STRING("HumanChwan"));
    set_string_for_key(&address, IMM_STRING("host"), IMM_STRING("github.com"));

    set_object_for_key(&obj, IMM_STRING("address"), address);
    // Present state of object:
    /*
     * {
     *     "name": "Dinesh Chukkala",
     *     "address": {
     *         "repo": "json-parser",
     *         "user": "HumanChwan",
     *         "host": "github.com"
     *     }
     * }
     */
    ```
- Dump the object into a `json` file:
    ```c
    // Open a file with write mode
    FILE* fp = fopen("test.json", "w");

    dump_json(dp, obj, 0, 4);
    ```
