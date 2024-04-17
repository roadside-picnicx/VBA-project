#include "../inc/func.h" 

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>


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

    freeKeyValueList(list);

}


void test_freeKeyValueList() {

    // Test freeKeyValueList function with a null pointer
    freeKeyValueList(NULL);
    // If the function doesn't crash, it's considered a pass
    CU_PASS("Null pointer test passed");

    // Test freeKeyValueList function with an empty list
    KeyValue *list = NULL;
    freeKeyValueList(list);
    CU_ASSERT_PTR_NULL(list);


    // Test freeKeyValueList function with a single node
    KeyValue *list2 = malloc(sizeof(KeyValue));
    list2->key = strdup("key");
    list2->value = strdup("value");
    list2->next = NULL;

    freeKeyValueList(list2);
    CU_ASSERT_PTR_NULL(list2->next);
    CU_ASSERT_PTR_NULL(list2->key);
    CU_ASSERT_PTR_NULL(list2->value);



    // Test freeKeyValueList function with multiple nodes
    KeyValue *list3 = malloc(sizeof(KeyValue));
    list3->key = strdup("key1");
    list3->value = strdup("value1");
    list3->next = malloc(sizeof(KeyValue));
    list3->next->key = strdup("key2");
    list3->next->value = strdup("value2");
    list3->next->next = malloc(sizeof(KeyValue));
    list3->next->next->key = strdup("key3");
    list3->next->next->value = strdup("value3");
    list3->next->next->next = NULL;

    freeKeyValueList(list3);
    CU_ASSERT_PTR_NULL(list3->next);
    CU_ASSERT_PTR_NULL(list3->key);
    CU_ASSERT_PTR_NULL(list3->value);



}


// Main function that runs the tests
int main() {
    CU_initialize_registry();

    // Add a suite to the registry
    CU_pSuite suite = CU_add_suite("Suite", init_suite, clean_suite);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add tests to the suite
    CU_add_test(suite, "test_addKeyValue", test_addKeyValue);
    CU_add_test(suite, "test_freeKeyValueList", test_freeKeyValueList);  

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();


}