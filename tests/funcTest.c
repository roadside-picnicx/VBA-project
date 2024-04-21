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


void provideInput(const char *input) {
    FILE *stream = fmemopen((void *)input, strlen(input), "r");
    if (stream == NULL) {
        CU_FAIL("Failed to open memory stream for input.");
        return;
    }
    stdin = stream;
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
    // CU_ASSERT_EQUAL(addKeyValue(&list, NULL, "value3"), 0);
    // CU_ASSERT_EQUAL(addKeyValue(&list, "key3", NULL), 0);

    freeKeyValueList(&list);
}


void test_freeKeyValueList() {
    // Test freeKeyValueList function with a null pointer
    freeKeyValueList(NULL);
    CU_PASS("Null pointer test passed");

    // Test freeKeyValueList function with a single node
    KeyValue *list2 = malloc(sizeof(KeyValue));
    list2->key = strdup("key");
    list2->value = strdup("value");
    list2->next = NULL;

    freeKeyValueList(&list2);
    CU_ASSERT_PTR_NULL(list2);

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

    freeKeyValueList(&list3);
    CU_ASSERT_PTR_NULL(list3);
}


void test_loadData(void) {
    int num_people = 0;

    // Test loading data from a valid JSON file (this is specific for testLoadData.json)
    Person *people = loadData("testLoadData.json", &num_people);
    CU_ASSERT_PTR_NOT_NULL_FATAL(people);
    CU_ASSERT_EQUAL(num_people, 2); //If you want to test different file, change the number to expected number of people

    // Clean up
    freePeople(people, num_people);

    int num_people2 = 0;
    Person *people2 = loadData("src/badfilename.json", &num_people2);
    CU_ASSERT_PTR_NULL_FATAL(people2);

    int num_people3 = 0;
    Person *people3 = loadData("src/testLoadData.jsom", &num_people3);

    CU_ASSERT_PTR_NULL_FATAL(people3);
}


void test_addNewData() {
    int num_people = 0;

    // Initialize test data
    Person *people = loadData("testLoadData.json", &num_people);

    // Fake user input in terminals
    provideInput("Most\nwaiter\n21000\ngaming\n21\nJames\n");

    // Call the function
    addNewData(&people, &num_people);

    // Verify the result
    CU_ASSERT_EQUAL(num_people, 3);
    CU_ASSERT_PTR_NOT_NULL(people);
    CU_ASSERT_PTR_NOT_NULL(people[2].data);   
    
    // Verify keys
    CU_ASSERT_STRING_EQUAL(people[2].data->key, "name");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->key, "age");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->key, "hobby");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->key, "salary");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->next->key, "job");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->next->next->key, "address");

    // Verify values
    CU_ASSERT_STRING_EQUAL(people[2].data->value, "James");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->value, "21");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->value, "gaming");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->value, "21000");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->next->value, "waiter");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->next->next->next->value, "Most");

    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(&(people[i].data));
    }
    free(people);
}


void test_printPersonData() {
    Person person;
    person.id = 1;
    person.data = NULL;
    
    addKeyValue(&(person.data), "Name", "John");
    addKeyValue(&(person.data), "Age", "30");
    addKeyValue(&(person.data), "City", "New York");

    freopen("test_printPersonData.txt", "w", stdout);
    // printPersonData(&person);
    // fclose(stdout);

    // FILE *fp = fopen("test_printPersonData.txt", "r");
    // if (fp == NULL) {
    //     CU_FAIL("Failed to open test_pritnPersonData.txt")
    //     return;
    // }
    
    // char buffer[100];

    // fgets(buffer, sizeof(buffer), fp);
    // CU_ASSERT_STRING_EQUAL(buffer, "Name: John\n");

    // fgets(buffer, sizeof(buffer), fp);
    // CU_ASSERT_STRING_EQUAL(buffer, "Age: 30\n");

    // fgets(buffer, sizeof(buffer), fp);
    // CU_ASSERT_STRING_EQUAL(buffer, "City: New York\n");

    // fgets(buffer, sizeof(buffer), fp);
    // CU_ASSERT_STRING_EQUAL(buffer, "\n");

    // fclose(fp);
    
}


void test_ModifyDataBasedOnID() {
    int num_people = 3;
    Person *people = malloc(num_people * sizeof(Person));
    if (people == NULL) {
        CU_FAIL("Memory allocation failed.");
        return;
    }
    for (int i = 0; i < num_people; ++i) {
        people[i].id = i + 1;
        people[i].data = NULL;
        addKeyValue(&(people[i].data), "Name", "John");
        addKeyValue(&(people[i].data), "Age", "30");
        addKeyValue(&(people[i].data), "City", "Brno");
    }


    provideInput("1\n2\nName\nJane\n");
    modifyDataBasedOnID(people, num_people);

    provideInput("2\n2\nAge\n31\n");
    modifyDataBasedOnID(people, num_people);

    provideInput("3\n2\nCity\nPraha\n");
    modifyDataBasedOnID(people, num_people);

    

    CU_ASSERT_STRING_EQUAL(people[0].data->next->next->value, "Jane");
    CU_ASSERT_STRING_EQUAL(people[0].data->next->value, "30");
    CU_ASSERT_STRING_EQUAL(people[0].data->value, "Brno");

    CU_ASSERT_STRING_EQUAL(people[1].data->next->next->value, "John");
    CU_ASSERT_STRING_EQUAL(people[1].data->next->value, "31");
    CU_ASSERT_STRING_EQUAL(people[1].data->value, "Brno");

    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->value, "John");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->value, "30");
    CU_ASSERT_STRING_EQUAL(people[2].data->value, "Praha");


    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(&(people[i].data));
    }
    free(people);
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
    CU_add_test(suite, "test_loadData", test_loadData); 
    CU_add_test(suite, "test_addNewData", test_addNewData);
    // CU_add_test(suite, "test_printPersonData", test_printPersonData);
    CU_add_test(suite, "test_ModifyDataBasedOnID", test_ModifyDataBasedOnID);

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    return CU_get_error();
}