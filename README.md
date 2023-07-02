# JSON Parser in C

## Quick Start

- Compile tests using:
    ```console
    $ make test
    ```
- Run `simple_dump` program to produce `test.json`:
    ```console
    $ ./simple_dump.exe
    ```

## Usage

- It is assumed that all JSON objects start with the object notation, and doesn't have other types as the parent.

- There are two family of functions concerning object key-value addition or extraction.
    - Addition/Setting key-value pair (KVP):
        ```c
        void set_value_for_key(struct Object* obj, String key, enum JSONType type, void* value);

        void set_int_for_key(struct Object* obj, String key, int* x);
        void set_int64_t_for_key(struct Object* obj, String key, int64_t* x);
        void set_double_for_key(struct Object* obj, String key, double* x);
        void set_char_for_key(struct Object* obj, String key, char* x);
        void set_string_for_key(struct Object* obj, String key, String* x);
        void set_object_for_key(struct Object* obj, String key, struct Object* x);
        ```
        - PS: It is be noted that the above functions also allow modification of value for the key

    - Extraction of key-value pair (KVP):
        ```c
        void* get_value_for_key(struct Object* obj, String key);

        // DO NOT USE THESE FUNCTIONS IF NOT SURE ABOUT THE TYPE
        int get_int_for_key(struct Object* obj, String key);
        int64_t get_int64_t_for_key(struct Object* obj, String key);
        double get_double_for_key(struct Object* obj, String key);
        char get_char_for_key(struct Object* obj, String key);
        String* get_string_for_key(struct Object* obj, String key);
        struct Object* get_object_for_key(struct Object* obj, String key);
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

    dump_json(obj, fp);
    ```
