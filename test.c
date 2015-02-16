#include "ral2/ral.h"

int main() {   

    ral_array_t * A = ral_new(ralArray);

    fprintf(stdout, "%i %i\n", ral_is_a(A, ralObject), ral_is_a(A, ralHash));
    
    ral_array_t * row = ral_new(ralArray);
    ral_insert(row, ral_integer_new(13), ral_integer_new(1));
    ral_insert(A, row, ral_integer_new(0));

    row = ral_new(ralArray);
    ral_insert(row, ral_integer_new(22), ral_integer_new(2));
    ral_insert(A, row, ral_integer_new(1));

    row = ral_new(ralArray);
    ral_insert(row, ral_integer_new(31), ral_integer_new(0));
    ral_insert(A, row, ral_integer_new(2));

    fprintf(stdout, "%s, %i\n", ral_as_string(A), ral_number_of_elements(A));

    ral_delete(A);
    fprintf(stdout, "ok\n");

    return 0;
}
