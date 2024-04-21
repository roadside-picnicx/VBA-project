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
      
    // Initialize test data
    Person person;
    person.id = 1;
    person.data = (KeyValue *)malloc(sizeof(KeyValue));
    person.data->key = strdup("Name");
    person.data->value = strdup("John");
    person.data->next = (KeyValue *)malloc(sizeof(KeyValue));
    person.data->next->key = strdup("Age");
    person.data->next->value = strdup("25");
    person.data->next->next = NULL;

    FILE *output_file = freopen("test_printPersonData.txt", "w", stdout);
    if (output_file == NULL) {
        perror("Failed to redirect stdout");
        return;
    }
    printPersonData(&person);
    fflush(stdout);
    fclose(output_file);
    freopen("/dev/tty", "w", stdout);

    FILE *fp = fopen("./test_printPersonData.txt", "r");
    if (fp == NULL) {
        CU_FAIL("Failed to open test output file");
        return;
    }
    
    char buffer[100];

    // Check the expected output
    CU_ASSERT_PTR_NOT_NULL(fgets(buffer, sizeof(buffer), fp));
    CU_ASSERT_STRING_EQUAL(buffer, "Person ID: 1\n");

    CU_ASSERT_PTR_NOT_NULL(fgets(buffer, sizeof(buffer), fp));
    CU_ASSERT_STRING_EQUAL(buffer, "Data:\n");

    CU_ASSERT_PTR_NOT_NULL(fgets(buffer, sizeof(buffer), fp));
    CU_ASSERT_STRING_EQUAL(buffer, "  Name: John\n");

    CU_ASSERT_PTR_NOT_NULL(fgets(buffer, sizeof(buffer), fp));
    CU_ASSERT_STRING_EQUAL(buffer, "  Age: 25\n");

    fclose(fp);
    
}


void test_modifyDataBasedOnID() {
    // Initialize test data
    int num_people = 4;
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

    provideInput("4\n1\n5\n");
    modifyDataBasedOnID(people, num_people);    

    CU_ASSERT_STRING_EQUAL(people[0].data->next->next->value, "Jane");
    CU_ASSERT_STRING_EQUAL(people[0].data->next->value, "30");
    CU_ASSERT_STRING_EQUAL(people[0].data->value, "Brno");
    CU_ASSERT_EQUAL(people[0].id, 1);

    CU_ASSERT_STRING_EQUAL(people[1].data->next->next->value, "John");
    CU_ASSERT_STRING_EQUAL(people[1].data->next->value, "31");
    CU_ASSERT_STRING_EQUAL(people[1].data->value, "Brno");
    CU_ASSERT_EQUAL(people[1].id, 2);

    CU_ASSERT_STRING_EQUAL(people[2].data->next->next->value, "John");
    CU_ASSERT_STRING_EQUAL(people[2].data->next->value, "30");
    CU_ASSERT_STRING_EQUAL(people[2].data->value, "Praha");
    CU_ASSERT_EQUAL(people[2].id, 3);

    CU_ASSERT_STRING_EQUAL(people[3].data->next->next->value, "John");
    CU_ASSERT_STRING_EQUAL(people[3].data->next->value, "30");
    CU_ASSERT_STRING_EQUAL(people[3].data->value, "Brno");
    CU_ASSERT_EQUAL(people[3].id, 5);


    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(&(people[i].data));
    }
    free(people);
}


void test_saveData() {
    // Prepare test data
    const char *filename = "test_output.json"; // Temporary file name for testing
    int num_people = 0;

    // Initialize test data
    Person *people = loadData("testLoadData.json", &num_people);
    provideInput("2\n2\nname\nJames\n");
    modifyDataBasedOnID(people, num_people);

    // Call the function
    saveData(filename, people, num_people);

    // Open the temp file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        CU_FAIL("Failed to open test output file");
        return;
    }

    // Read the content of temp file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = (char *)malloc((file_size + 1) * sizeof(char));
    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';

    // Close temp file
    fclose(fp);

    // Read test_saveData.json
    const char *expected_filename = "test_saveData.json"; // File containing the expected JSON
    fp = fopen(expected_filename, "r");
    if (fp == NULL) {
        CU_FAIL("Failed to open expected output file");
        free(buffer);
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    long expected_file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *expected_buffer = (char *)malloc((expected_file_size + 1) * sizeof(char));
    fread(expected_buffer, 1, expected_file_size, fp);
    expected_buffer[expected_file_size] = '\0';

    // Close the test_saveData.json
    fclose(fp);

    // Compare the generated file with expected (test_saveData.json)
    printf("Buffer:\n%s\n", buffer);
    printf("Expected JSON:\n%s\n", expected_buffer);
    CU_ASSERT_STRING_EQUAL(buffer, expected_buffer);

    // Clean up
    free(buffer);
    free(expected_buffer);
}


void test_freePeople() {
    int num_people = 0;

    // Initialize test data
    Person *people = loadData("testLoadData.json", &num_people);
    if (people == NULL) {
        CU_FAIL("Memory allocation failed");
        return;
    }

    // Call the function
    freePeople(people, num_people);

    // Run assertions
    CU_ASSERT_PTR_NULL(people);
    CU_ASSERT_PTR_NOT_NULL(people);
}


void test_deletePersonByID() {
    // Prepare test data
    int num_people = 3;
    Person *people = (Person *)malloc(num_people * sizeof(Person));
    if (people == NULL) {
        CU_FAIL("Memory allocation failed");
        return;
    }

    // Initialize test data
    for (int i = 0; i < num_people; ++i) {
        people[i].id = i + 1;
        people[i].data = NULL; // Add your test data here
    }

    // Call the function to delete a person with ID 2
    int id_to_delete = 2;
    deletePersonByID(people, &num_people, id_to_delete);

    // Person with ID 2 is deleted
    int found = 0;
    for (int i = 0; i < num_people; ++i) {
        if (people[i].id == id_to_delete) {
            found = 1;
            break;
        }
    }
    CU_ASSERT_EQUAL(found, 0);
    // There are two people left
    CU_ASSERT_EQUAL(num_people, 2);

    // Clean up
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
    // CU_add_test(suite, "test_printPersonData", test_printPersonData); // Not working correctly
    CU_add_test(suite, "test_modifyDataBasedOnID", test_modifyDataBasedOnID);
    CU_add_test(suite, "test_saveData", test_saveData); 
    CU_add_test(suite, "test_freePeople", test_freePeople); // Failing, not sure if can be tested by unit test
    CU_add_test(suite, "test_deletePersonByID", test_deletePersonByID);

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    return CU_get_error();
}