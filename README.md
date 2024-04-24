# VBA Project

This is a simple C project that utilizes cJSON library to read and write JSON data.

## Assignment
Create a program that will read a text file in JSON format. It stores the data in dynamically allocated variables in memory. It will also allow you to save the data in memory to a JSON text file.

## Compile
To compile the project, you can choose to use `make` command or manually compile the files.

The `make` will create `VBA_projekt.exe` and `unitTests.exe` executables, so they can run on Windows and Linux. You can call from the root folder following commands:
```bash
# Compile only program
make 
make build
# Compile only tests
make build_tests
# Compile both program and tests
make build_all
# Remove only VBA_projekt.exe
make clean 
# Remove only unitTests.exe
make clean_tests
# Remove VBA_projekt.exe and unitTests.exe
make clean_all
```

To manually compile files use the following command:

```bash
# Make sure you are in root folder
gcc -o <output_file> src/main.c src/func.c cJSON/cJSON.o
# Command for compiling unit tests
gcc -o <test_output_file> tests/funcTest.c src/func.c cJSON/cJSON.o -lcunit
```

To compile on Windows 11, specifically with VS Code:
```bash
# Make sure you are in the root folder
gcc -o <output_file>.exe .\src\func.c .\src\main.c .\cJSON\cJSON.c  
# Command for compiling unit tests
gcc -o <test_output_file>.exe .\tests\funcTest.c .\src\func.c .\cJSON\cJSON.c -lcunit
```

## Gcov
To check code coverage using gcov on Windows 11, you need to add following flags while compiling to generate .gcno files:
```bash
gcc -o main.exe .\src\func.c .\src\main.c .\cJSON\cJSON.c -fprofile-arcs -ftest-coverage 
gcc -o test.exe .\tests\funcTest.c .\src\func.c .\cJSON\cJSON.c -lcunit -fprofile-arcs -ftest-coverage
```

Then you need to run executables, that will generate .gcda files.

To generate .gcov files, simply run gcov command with the source file and previously generated .gcno and .gcda files:
```bash
gcov .\src\main.c .\main-main.gcda .\main-main.gcno
gcov .\tests\funcTest.c .\test-funcTest.gcno .\test-funcTest.gcda
```

After this, you should have successfully generated .gcov files. By analyzing aspects of the .gcov files, you can gain valuable insights into the effectiveness of test suite and identify areas of code that may require further testing or optimization.

## Gcov Viewer
To check code coverage using gcov viewer, use the following commands:
```bash
gcc --coverage src/func.c tests/funcTest.c cJSON/cJSON.c -lcunit
./a.out
```
After that press: CTRL + SHIFT + P and execute: Gcov Viewer:Show.

## Structures
These two structures were created to facilitate efficient data organization and management within the scope of this project.

### KeyValue Structure

```c
typedef struct KeyValue {
    char *key;
    char *value;
    struct KeyValue *next;
} KeyValue;
```

The KeyValue structure is designed to represent dynamic key-value pairs, commonly used for storing and accessing associated data in a flexible manner. It consists of the following components:
* char *key: Represents the key associated with a particular value. It is a pointer to a character array.
* char *value: Represents the corresponding value associated with the key. Similar to the key, it is also a pointer to a character array.
* struct KeyValue *next: Points to the next KeyValue structure in a linked list, enabling the creation of chains of key-value pairs.

### Person Structure

```c
// Structure to represent a person
typedef struct {
    int id;         
    KeyValue *data;  
} Person;

```

The Person structure is intended to represent an individual and their associated data. It comprises the following elements:
* int id: An identifier unique to each person.
* KeyValue *data: A pointer to a linked list of key-value pairs, allowing for the storage of attributes associated with the person. 

## Functions
These functions enable operations such as creation, modification, and retrieval of information associated with individuals (Person structure) and their corresponding attributes (KeyValue structure).

### addKeyValue
```C 
int addKeyValue(KeyValue **list, const char *key, const char *value);
```

This function adds a new key-value pair to a linked list. It takes in three parameters: a pointer to a pointer to the head of the linked list, a key, and a value.

Process:
* Memory is allocated for a new node of type KeyValue.
* If memory allocation fails, an error message is printed, and the function returns 0.
* Memory is allocated for the key and value strings within the new node.
* If memory allocation for either the key or value string fails, an error message is printed, allocated memory is freed, and the function returns 0.
* The new node is linked to the existing list by setting its next pointer to the current head of the list.
* The head pointer is updated to point to the newly added node.
* The function returns 1 to indicate successful addition of the key-value pair.

### freeKeyValueList

```C 
void freeKeyValueList(KeyValue **list);
```

This function releases memory allocated for a linked list of key-value pairs. It takes a pointer to the head of the linked list as its parameter.

Process:
* The function iterates through the linked list.
* At each iteration:
    * It stores the current node in a temporary variable.
    * Advances the list pointer to the next node in the list.
    * Frees the memory allocated for the key and value strings within the temporary node.
    * Frees the memory allocated for the temporary node.
* The loop continues until the end of the list is reached.
* After all nodes have been freed, the function sets the list pointer to NULL to indicate that the list is empty.

### Person *loadData

```C
Person *loadData(const char *filename, int *num_people);
```

This function loads data from a file, parses it into memory, and returns an array of Person structures containing the parsed data. It takes two parameters: the filename to load and a pointer to an integer to store the number of people loaded from the file.

Process:
1. File Opening and Reading:
    * Attempts to open file in read mode.
    * If the file opening fails, it prints an error message to stderr and returns NULL.
    * Reads the content of the file into memory, dynamically allocating memory to store the file content.

2. JSON Parsing:
    * Parses the file content using the cJSON library.
    * If parsing fails, it prints an error message to stderr and returns NULL.

3. JSON Data Extraction:
    * Retrieves the "people" array from the parsed JSON.
    * If the "people" array is invalid or missing, it prints an error message to stderr, cleans up memory, and returns NULL.
    * Determines the number of people in the array and updates the num_people parameter.

4. Memory Allocation:
    * Allocates memory for an array of Person structures based on the number of people extracted from the JSON.
    * If memory allocation fails, it prints an error message to stderr, cleans up memory, and returns NULL.

5. Data Population:
    * Iterates through each person in the "people" array.
    * Initializes a linked list for key-value pairs associated with each person.
    * Extracts the person's identifier from the JSON and sets it in the Person structure.
    * Iterates through all items in the person's JSON object (excluding the "id" field).
    * Adds key-value pairs to the linked list for each attribute in the JSON object using the addKeyValue function.

6. Cleanup:
    * Deletes the parsed JSON object to free memory.
    * Returns the populated array of Person structures.

### addNewData

```C
void addNewData(Person **people, int *num_people);
```

This function allows for the addition of new data to the memory based on the keys present in the loaded data. It takes two parameters: a pointer to an array of Person structures and a pointer to an integer representing the number of people in the array.

Process:
1. Finding Unique Keys:
    * Iterates through the existing Person structures to find unique keys present in their associated key-value pairs.
    * Dynamically allocates memory to store unique keys in the keys array.

2. Creating a New Person Object:
    * Initializes a new Person object with an ID assigned based on the current number of people ((*num_people) + 1) in the existing data.
    * Sets the data field of the new Person object to NULL.

3. Prompting for Values:
    * Prompts the user to input values corresponding to each unique key found in the loaded data.
    * Adds the key-value pairs to the data field of the new Person object using the addKeyValue function.

4. Adding the New Person Object:
    * Increases the count of people (*num_people) by 1 to accommodate the new person.
    * Resizes the memory allocated for the people array using realloc.
    * Assigns the new Person object to the last position in the people array.

5. Memory Cleanup:
    * Frees the memory allocated for the keys array and its elements.

### printPersonData

```C
void printPersonData(const Person *person);
```

This function prints the data associated with a specific person. It takes a pointer to a Person structure as its parameter.

Process:
* Prints the identifier (ID) of the person (person->id).
* Iterates through the linked list of key-value pairs (person->data) associated with the person.
* Prints each key-value pair, where the key represents an attribute and the value represents the corresponding value.

### modifyPersonData

```C
void modifyPersonData(Person *person);
```

This function enables the modification of data associated with a specific person. It takes a pointer to a Person structure as its parameter.

Process:
1. Prompting for Modification Options:
    * Displays the available options for modification, including modifying the person's ID or modifying a key-value pair.
2. Handling User Choice:
    * Reads the user's choice of modification option.
    * Based on the choice:
        * If the user chooses to modify the ID (choice == 1), prompts for and updates the person's ID.
        * If the user chooses to modify a key-value pair (choice == 2), proceeds to modify the associated data.
            * Checks if there is data available for modification. If not, notifies the user.
            * Prompts the user for the key and new value to be modified.
            * Searches for the key in the linked list of key-value pairs associated with the person.
            * If the key is found, updates the corresponding value with the new value.
            * If the key is not found, notifies the user that the key was not found.
3. Invalid Choice Handling:
    * If the user enters an invalid choice, notifies the user.

### saveData

```C
void saveData(const char *filename, Person *people, int num_people);
```

This function saves modified data back to a file in JSON format. It takes three parameters:
* const char *filename: A string representing the filename to which the data will be saved.
* Person *people: A pointer to an array of Person structures containing the modified data.
* int num_people: An integer representing the number of people in the people array.

Process:
1. Creating JSON Structure:
    * Creates a new cJSON object representing the root of the JSON structure.
    * Creates a cJSON array to store individual person objects.
2. Creating Person Objects:
    * Iterates through the people array and creates cJSON objects for each person.
    * Adds the person's ID and associated key-value pairs to the cJSON objects.
3. Parsing Values:
    * Determines whether each value is numeric or a string.
    * Uses cJSON_CreateNumber for numeric values and cJSON_CreateString for string values, handling quotes if necessary.
4. Saving to File:
    * Prints the JSON string to the console.
    * Attempts to open the file specified by filename for writing.
    * If successful, writes the JSON string to the file.
    * Closes the file after writing.
5. Error Handling:
    * Handles errors related to file opening and writing, printing corresponding error messages.
6. Memory Cleanup:
    * Deletes the cJSON object and frees the memory allocated for the JSON string.

### freePeople

```C
void freePeople(Person *people, int num_people);
```

This function is responsible for releasing the memory allocated for an array of Person structures. It takes two parameters:
* Person *people: A pointer to the array of Person structures.
* int num_people: An integer representing the number of Person structures in the array.

Process:
1. Checking for NULL Pointer:
    * Checks if the people pointer is NULL. If so, returns early as there is nothing to free.
2. Freeing Key-Value Lists:
    * Iterates through each Person structure in the array.
    * Calls the freeKeyValueList function to release the memory allocated for the linked list of key-value pairs associated with each person.
3. Freeing Array Memory:
    * Frees the memory allocated for the array of Person structures.

### modifyDataBasedOnID

```C
void modifyDataBasedOnID(Person *people, int num_people);
```

This function allows for the modification of data associated with a person based on their ID. It takes two parameters:
* Person *people: A pointer to an array of Person structures containing the data.
* int num_people: An integer representing the number of people in the people array.

Process:
1. Prompting for ID:
    * Prompts the user to enter the ID of the person to modify. Entering 0 returns the user to the main menu.
2. Searching for Person:
    * Iterates through the people array to find the person with the specified ID.
    * If the person is found, retrieves a pointer to the corresponding Person structure.
3. Modifying Data:
    * If the person is found (targetPerson != NULL), calls the modifyPersonData function to allow the user to modify the person's data.
    * If the person is not found, prints a message indicating that the person with the specified ID was not found.
4. Continuation Condition:
    * Continues looping until the user chooses to return to the main menu by entering 0.

### deletePersonByID

```C
void deletePersonByID(Person *people, int *num_people, int id);
```

This function deletes a person from the dataset based on their ID. It takes three parameters:
* Person *people: A pointer to an array of Person structures containing the dataset.
* int *num_people: A pointer to an integer representing the number of people in the dataset.
* int id: An integer representing the ID of the person to be deleted.

Process:
1. Searching for Person:
    * Iterates through the people array to find the person with the specified ID.
    * If the person is found:
        * Frees the memory allocated for the key-value pairs associated with the person.
        * Moves the last person in the array to the position of the deleted person to maintain array continuity.
        * Decrements the count of the total number of people.
        * Prints a message confirming the deletion of the person.
        * Returns from the function.
2. Handling Not Found:
    * If the person with the specified ID is not found in the dataset, prints a message indicating that the person was not found.


## Tests

With one exeption, a unit test is written for all functions in the project. These tests need to be run from the root folder of project `vba_projekt`. They can be build and run using following commands:

```bash
# Make sure you are in the root folder of project
cd vba_projekt
# Building tests 
gcc -o <test_output_file> src/func.c tests/funcTest.c cJSON/cJSON.c -lcunit
# Running tests
./<test_output_file>
```

