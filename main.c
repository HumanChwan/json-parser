#include <stdio.h>

#include "Array.h"

int main(void) {
    Array arr = create_array(5);

    set_char_element(&arr, 0, 'c');
    set_int32_t_element(&arr, 1, 1231231);
    set_char_element(&arr, 2, 'a');
    set_int32_t_element(&arr, 3, 100);
    set_char_element(&arr, 4, 'b');

    for (size_t i = 0; i < arr.size; ++i) {
        if (i % 2 == 0) {
            char x = get_char_element(&arr, i);
            printf("%c, ", x);
        } else {
            int32_t x = get_int32_t_element(&arr, i);
            printf("%d, ", x);
        }
    }
    printf("\n");
}
