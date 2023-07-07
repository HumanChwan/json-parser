#ifndef JSON_TYPES_
#define JSON_TYPES_

enum JSONType {
    JSON_TYPE_CHAR,         // 0
    JSON_TYPE_STRING,       // 1
    JSON_TYPE_INTEGER,      // 2
    JSON_TYPE_LONG,         // 3
    JSON_TYPE_DOUBLE,       // 4 
    JSON_TYPE_OBJECT,       // 5
    JSON_TYPE_ARRAY,        // 6
    JSON_TYPE_BOOLEAN,      // 7
    JSON_TYPE_NULL,         // 8
    JSON_TYPE_NAT,          // 9 -- NOT A TYPE
    JSON_TYPE_TYPE_COUNT
};

#endif // JSON_TYPES_

