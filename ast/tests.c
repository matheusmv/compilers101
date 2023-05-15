#include <stdio.h>
#include <stdlib.h>

#include "tests/smem/smem_test.h"
#include "tests/token/token_test.h"
#include "tests/utils/utils_test.h"
#include "tests/types/types_test.h"

int main(void) {
    run_smem_tests();
    run_token_tests();
    run_utils_tests();
    run_type_tests();

    return EXIT_SUCCESS;
}
