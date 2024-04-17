#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "main.h" 

// Test suite initialization function
int init_suite(void) {
    return 0;
}

// Test suite cleanup function
int clean_suite(void) {
    return 0;
}

// Test cases for addKeyValue function
void test_addKeyValue(void) {
    // Test Case 1: Add a new key-value pair to an empty list
    KeyValue *list = NULL;
    CU_ASSERT_EQUAL(addKeyValue(&list, "key1", "value1"), 1);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_STRING_EQUAL(list->key, "key1");
    CU_ASSERT_STRING_EQUAL(list->value, "value1");
    CU_ASSERT_PTR_NULL(list->next);

    // Test Case 2: Add a new key-value pair to a non-empty list
    CU_ASSERT_EQUAL(addKeyValue(&list, "key2", "value2"), 1);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_STRING_EQUAL(list->key, "key2");
    CU_ASSERT_STRING_EQUAL(list->value, "value2");
    CU_ASSERT_PTR_NOT_NULL(list->next);
    CU_ASSERT_STRING_EQUAL(list->next->key, "key1");
    CU_ASSERT_STRING_EQUAL(list->next->value, "value1");

    // Test Case 3: Attempt to add a new key-value pair with NULL key or value
    CU_ASSERT_EQUAL(addKeyValue(&list, NULL, "value3"), 0);
    CU_ASSERT_EQUAL(addKeyValue(&list, "key3", NULL), 0);
}

// Main function that runs the test cases
int main() {
    CU_initialize_registry();

    // Add a suite to the registry
    CU_pSuite addKeyValueSuite = CU_add_suite("addKeyValue Suite", init_suite, clean_suite);
    CU_add_test(addKeyValueSuite, "test_addKeyValue", test_addKeyValue);

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Cleanup
    CU_cleanup_registry();

    return CU_get_error();
}