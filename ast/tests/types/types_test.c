#include "types_test.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../../src/types.h"


void test_atomic_type_equals(void) {
    Type* intType1 = NEW_INT_TYPE();
    Type* intType2 = NEW_INT_TYPE();
    Type* floatType = NEW_FLOAT_TYPE();
    Type* charType = NEW_CHAR_TYPE();

    assert(type_equals(&intType1, &intType2) == true);
    assert(type_equals(&intType1, &floatType) == false);
    assert(type_equals(&intType1, &charType) == false);

    type_free(&intType1);
    type_free(&intType2);
    type_free(&floatType);
    type_free(&charType);
}

void test_custom_type_equals(void) {
    Type* type1 = NEW_CUSTOM_TYPE(0, "User");
    Type* type2 = NEW_CUSTOM_TYPE(0, "User");
    Type* type3 = NEW_CUSTOM_TYPE(0, "Context");

    assert(type_equals(&type1, &type2) == true);
    assert(type_equals(&type1, &type3) == false);
    assert(type_equals(&type2, &type3) == false);

    type_free(&type1);
    type_free(&type2);
    type_free(&type3);
}

void test_named_type_equals(void) {
    Type* usernameType = NEW_NAMED_TYPE("username", NEW_STRING_TYPE());
    Type* passwordType = NEW_NAMED_TYPE("password", NEW_STRING_TYPE());
    Type* idType = NEW_NAMED_TYPE("id", NEW_INT_TYPE());

    assert(type_equals(&usernameType, &usernameType) == true);
    assert(type_equals(&passwordType, &passwordType) == true);
    assert(type_equals(&idType, &idType) == true);

    assert(type_equals(&usernameType, &passwordType) == false);
    assert(type_equals(&usernameType, &idType) == false);
    assert(type_equals(&passwordType, &idType) == false);

    type_free(&usernameType);
    type_free(&passwordType);
    type_free(&idType);
}

void test_struct_type_equals(void) {
    Type* userStruct = NEW_STRUCT_TYPE(0);
    STRUCT_TYPE_ADD_FIELDS(userStruct,
        NEW_NAMED_TYPE("id", NEW_INT_TYPE()),
        NEW_NAMED_TYPE("username", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("password", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("email", NEW_STRING_TYPE())
    );

    Type* clientStruct = NEW_STRUCT_TYPE(0);
    STRUCT_TYPE_ADD_FIELDS(clientStruct,
        NEW_NAMED_TYPE("id", NEW_INT_TYPE()),
        NEW_NAMED_TYPE("username", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("password", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("email", NEW_STRING_TYPE())
    );

    Type* addressStruct = NEW_STRUCT_TYPE(0);
    STRUCT_TYPE_ADD_FIELDS(addressStruct,
        NEW_NAMED_TYPE("number", NEW_INT_TYPE()),
        NEW_NAMED_TYPE("street", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("zip", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("city", NEW_STRING_TYPE())
    );

    assert(type_equals(&userStruct, &userStruct) == true);
    assert(type_equals(&clientStruct, &clientStruct) == true);
    assert(type_equals(&addressStruct, &addressStruct) == true);

    assert(type_equals(&userStruct, &clientStruct) == true);
    assert(type_equals(&userStruct, &addressStruct) == false);
    assert(type_equals(&clientStruct, &addressStruct) == false);

    type_free(&userStruct);
    type_free(&clientStruct);
    type_free(&addressStruct);
}

void test_array_dimension_equals(void) {
    Type* a = NEW_ARRAY_DIMENSION(1);
    Type* b = NEW_ARRAY_DIMENSION(1);
    Type* c = NEW_ARRAY_DIMENSION(3);
    Type* d = NEW_ARRAY_UNDEFINED_DIMENSION();
    Type* e = NEW_ARRAY_DIMENSION(0);

    assert(type_equals(&a, &b) == true);
    assert(type_equals(&a, &c) == false);
    assert(type_equals(&b, &c) == false);
    assert(type_equals(&d, &e) == true);

    type_free(&a);
    type_free(&b);
    type_free(&c);
    type_free(&d);
    type_free(&e);
}

void test_array_type_equals(void) {
    Type* array1 = NEW_ARRAY_TYPE(NEW_INT_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array1,
        NEW_ARRAY_DIMENSION(1)
    );

    Type* array2 = NEW_ARRAY_TYPE(NEW_INT_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array2,
        NEW_ARRAY_DIMENSION(1)
    );

    Type* array3 = NEW_ARRAY_TYPE(NEW_INT_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array3,
        NEW_ARRAY_DIMENSION(3)
    );

    Type* array4 = NEW_ARRAY_TYPE(NEW_STRING_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array4,
        NEW_ARRAY_DIMENSION(3),
        NEW_ARRAY_DIMENSION(3),
    );

    Type* array5 = NEW_ARRAY_TYPE(NEW_STRING_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array5,
        NEW_ARRAY_DIMENSION(3),
        NEW_ARRAY_DIMENSION(3),
    );

    Type* array6 = NEW_ARRAY_TYPE(NEW_STRING_TYPE());
    ARRAY_TYPE_ADD_DIMENSIONS(array6,
        NEW_ARRAY_DIMENSION(2),
        NEW_ARRAY_DIMENSION(3),
    );

    assert(type_equals(&array1, &array2) == true);
    assert(type_equals(&array1, &array3) == false);
    assert(type_equals(&array2, &array3) == false);

    assert(type_equals(&array1, &array4) == false);
    assert(type_equals(&array2, &array4) == false);
    assert(type_equals(&array3, &array4) == false);

    assert(type_equals(&array4, &array5) == true);
    assert(type_equals(&array4, &array6) == false);
    assert(type_equals(&array5, &array6) == false);

    type_free(&array1);
    type_free(&array2);
    type_free(&array3);
    type_free(&array4);
    type_free(&array5);
    type_free(&array6);
}

void test_function_type_equals(void) {
    Type* funcType1 = NEW_FUNCTION_TYPE_WITH_RETURN(NEW_VOID_TYPE());
    FUNCTION_TYPE_ADD_PARAMS(funcType1,
        NEW_CUSTOM_TYPE(0, "User")
    );

    Type* funcType2 = NEW_FUNCTION_TYPE_WITH_RETURN(NEW_VOID_TYPE());
    FUNCTION_TYPE_ADD_PARAMS(funcType2,
        NEW_CUSTOM_TYPE(0, "User")
    );

    assert(type_equals(&funcType1, &funcType2) == true);

    Type* funcType3 = NEW_FUNCTION_TYPE();
    FUNCTION_TYPE_ADD_PARAMS(funcType3,
        NEW_CUSTOM_TYPE(0, "User")
    );

    assert(type_equals(&funcType1, &funcType3) == true);

    Type* funcType4 = NEW_FUNCTION_TYPE_WITH_RETURN(NEW_VOID_TYPE());
    FUNCTION_TYPE_ADD_PARAMS(funcType4,
        NEW_NAMED_TYPE("username", NEW_STRING_TYPE())
    );

    assert(type_equals(&funcType1, &funcType4) == false);
    assert(type_equals(&funcType2, &funcType4) == false);
    assert(type_equals(&funcType3, &funcType4) == false);

    Type* funcType5 = NEW_FUNCTION_TYPE_WITH_RETURN(NEW_NAMED_TYPE("Token", NEW_STRING_TYPE()));
    FUNCTION_TYPE_ADD_PARAMS(funcType5,
        NEW_NAMED_TYPE("username", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("password", NEW_STRING_TYPE())
    );

    Type* funcType6 = NEW_FUNCTION_TYPE_WITH_RETURN(NEW_NAMED_TYPE("Token", NEW_STRING_TYPE()));
    FUNCTION_TYPE_ADD_PARAMS(funcType6,
        NEW_NAMED_TYPE("username", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("password", NEW_STRING_TYPE())
    );

    assert(type_equals(&funcType5, &funcType6) == true);

    type_free(&funcType1);
    type_free(&funcType2);
    type_free(&funcType3);
    type_free(&funcType4);
    type_free(&funcType5);
    type_free(&funcType6);
}

void run_type_tests(void) {
    test_atomic_type_equals();
    test_custom_type_equals();
    test_named_type_equals();
    test_struct_type_equals();
    test_array_dimension_equals();
    test_array_type_equals();
    test_function_type_equals();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
