# VBA Project

This is a simple C project that utilizes cJSON library to read and write JSON data.

## Assignment
Create a program that will read a text file in JSON format. It stores the data in dynamically allocated variables in memory. It will also allow you to save the data in memory to a JSON text file.

## Compile
To compile the project, use the following command:

```bash
gcc -o VBA_projekt main.c cJSON/cJSON.o
gcc -o test_person tests/testing.c main.c cJSON/cJSON.o -lcunit
```

To compile on Windows 11, specifically with VS Code:
```bash
gcc -o main.exe .\src\func.c .\src\main.c .\cJSON\cJSON.c  
gcc -o test.exe .\tests\funcTest.c .\src\func.c .\cJSON\cJSON.c -lcunit
```

## Gcov
To check code coverage using gcov on Windows 11, you need to add following flags while compiling to generate .gcno files:
```bash
gcc -o main.exe .\src\func.c .\src\main.c .\cJSON\cJSON.c -fprofile-arcs -ftest-coverage 
gcc -o test.exe .\tests\funcTest.c .\src\func.c .\cJSON\cJSON.c -lcunit -fprofile-arcs -ftest-coverage
```

Then you need to run your executables, that will generate .gcda files.

To generate .gcov files, simply run gcov command with the source file and previously generated .gcno and .gcda files:
```bash
gcov .\src\main.c .\main-main.gcda .\main-main.gcno
gcov .\tests\funcTest.c .\test-funcTest.gcno .\test-funcTest.gcda
```

After this, you should have successfully generated your .gcov files. By analyzing aspects of the .gcov files, you can gain valuable insights into the effectiveness of your test suite and identify areas of your code that may require further testing or optimization.

## Structures
These two structures were created to facilitate efficient data organization and management within the scope of this project.

### KeyValue Structure
The KeyValue structure is designed to represent dynamic key-value pairs, commonly used for storing and accessing associated data in a flexible manner. It consists of the following components:

char *key: Represents the key associated with a particular value. It is a pointer to a character array.

char *value: Represents the corresponding value associated with the key. Similar to the key, it is also a pointer to a character array.

struct KeyValue *next: Points to the next KeyValue structure in a linked list, enabling the creation of chains of key-value pairs.

```C
typedef struct KeyValue {
    char *key;
    char *value;
    struct KeyValue *next;
} KeyValue;
```

### Person Structure
The Person structure is intended to represent an individual and their associated data. It comprises the following elements:

int id: An identifier unique to each person, facilitating efficient retrieval and management of individual records.

KeyValue *data: A pointer to a linked list of key-value pairs, allowing for the storage of various attributes or properties associated with the person. This design offers flexibility in accommodating diverse sets of information for each individual.

```C
typedef struct {
    int id;
    KeyValue *data; 
} Person;
```

## Functions
With the structures defined, the project now proceeds to describe the functions responsible for manipulating data stored within these structures. These functions enable operations such as creation, modification, and retrieval of information associated with individuals (Person structure) and their corresponding attributes (KeyValue structure).

### addKeyValue
This function adds a new key-value pair to a linked list. It takes in three parameters: a pointer to a pointer to the head of the linked list, a key, and a value.

```C
int addKeyValue(KeyValue **list, const char *key, const char *value) {
    KeyValue *newNode = malloc(sizeof(KeyValue));
    if (!newNode) {
        perror("Memory allocation failed for new KeyValue node");
        return 0;
    }

    newNode->key = strdup(key);
    newNode->value = strdup(value);
    if (!newNode->key || !newNode->value) {
        perror("Memory allocation failed for key or value string");
        free(newNode->key);
        free(newNode->value);
        free(newNode);
        return 0;
    }

    newNode->next = *list;
    *list = newNode;

    return 1;
}
```

Process:
* Memory is allocated for a new node of type KeyValue.
* If memory allocation fails, an error message is printed, and the function returns 0.
* Memory is allocated for the key and value strings within the new node.
* If memory allocation for either the key or value string fails, an error message is printed, allocated memory is freed, and the function returns 0.
* The new node is linked to the existing list by setting its next pointer to the current head of the list.
* The head pointer is updated to point to the newly added node.
* The function returns 1 to indicate successful addition of the key-value pair.

### freeKeyValueList
This function releases memory allocated for a linked list of key-value pairs. It takes a pointer to the head of the linked list as its parameter.

```C
void freeKeyValueList(KeyValue *list) {
    while (list) {
        KeyValue *temp = list;
        list = list->next;
        free(temp->key);
        free(temp->value);
        free(temp);
    }
}
```

Process:
* The function iterates through the linked list.
* At each iteration:
    * It stores the current node in a temporary variable.
    * Advances the list pointer to the next node in the list.
    * Frees the memory allocated for the key and value strings within the temporary node.
    * Frees the memory allocated for the temporary node.
* The loop continues until the end of the list is reached.

### Person *loadData
This function loads data from a specified file, parses it into memory, and returns an array of Person structures containing the parsed data. It takes two parameters: the filename to load and a pointer to an integer to store the number of people loaded from the file.

```C
Person *loadData(const char *filename, int *num_people) {
    // File Opening and Reading
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error occurred when trying to open file '%s'.\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *file_content = malloc(file_size + 1);
    if (!file_content) {
        perror("Memory allocation failed for file content");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    printf("File content: %s\n", file_content);

    // JSON Parsing
    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        fprintf(stderr, "Error when parsing JSON.\n");
        return NULL;
    }

    // JSON Data Extraction
    cJSON *people_array = cJSON_GetObjectItem(json, "people");
    if (!people_array || !cJSON_IsArray(people_array)) {
        fprintf(stderr, "Invalid or missing 'people' array in JSON.\n");
        cJSON_Delete(json);
        return NULL;
    }

    *num_people = cJSON_GetArraySize(people_array);

    // Memory Allocation
    Person *people = malloc((*num_people) * sizeof(Person));

    if (!people) {
        fprintf(stderr, "Memory allocation failed.\n");
        cJSON_Delete(json);
        return NULL;
    }

    // Data Population
    for (int i = 0; i < *num_people; ++i) {
        cJSON *person_json = cJSON_GetArrayItem(people_array, i);

        // Initialize a linked list for key-value pairs
        people[i].data = NULL;

        // Set the person's identifier (id)
        cJSON *id_item = cJSON_GetObjectItem(person_json, "id");
        people[i].id = (id_item != NULL && cJSON_IsNumber(id_item)) ? id_item->valueint : -1;

        // Iterate through all items in the person's JSON object
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, person_json) {
            // Exclude the "id" field from being added to the linked list
            if (strcmp(item->string, "id") != 0) {
                // Add key-value pair to the linked list
                addKeyValue(&(people[i].data), item->string, cJSON_Print(item));
            }
        }
    }

    // Cleanup
    cJSON_Delete(json);
    return people;
}
```

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
    * Deletes the parsed JSON object to free up memory.
    * Returns the populated array of Person structures.

### addNewData
This function allows for the addition of new data to the memory based on the keys present in the loaded data. It takes two parameters: a pointer to an array of Person structures and a pointer to an integer representing the number of people in the array.

```C
void addNewData(Person **people, int *num_people) {
    // Find all unique keys present in the loaded data
    char **keys = NULL;
    int num_keys = 0;

    // Iterate through existing people to find unique keys
    for (int i = 0; i < *num_people; ++i) {
        KeyValue *current = (*people)[i].data;
        while (current) {
            // Check if the key is already present in the keys array
            int found = 0;
            for (int j = 0; j < num_keys; ++j) {
                if (strcmp(current->key, keys[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            // If the key is not already present, add it to the keys array
            if (!found) {
                num_keys++;
                keys = realloc(keys, num_keys * sizeof(char *));
                keys[num_keys - 1] = strdup(current->key);
            }
            current = current->next;
        }
    }

    // Create a new person object
    Person new_person;
    new_person.id = *num_people + 1;
    new_person.data = NULL;

    // Prompt the user for values
    for (int j = 0; j < num_keys; ++j) {
        char value[100];
        printf("Enter value for key %s: ", keys[j]);
        scanf("%99s", value);
        addKeyValue(&(new_person.data), keys[j], value);
    }

    // Add the new person object to the existing people array
    *num_people += 1;
    *people = realloc(*people, *num_people * sizeof(Person));
    (*people)[*num_people - 1] = new_person;

    // Free the memory allocated for keys
    for (int i = 0; i < num_keys; ++i) {
        free(keys[i]);
    }
    free(keys);
}
```

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
This function prints the data associated with a specific person. It takes a pointer to a Person structure as its parameter.

```C
void printPersonData(const Person *person) {
    printf("Person ID: %d\nData:\n", person->id);

    const KeyValue *key_value = person->data;
    while (key_value) {
        printf("  %s: %s\n", key_value->key, key_value->value);
        key_value = key_value->next;
    }
    printf("\n");
}
```

Process:
1. Printing Data:
    * Prints the identifier (ID) of the person (person->id).
    * Iterates through the linked list of key-value pairs (person->data) associated with the person.
    * Prints each key-value pair, where the key represents an attribute and the value represents the corresponding value.

### modifyPersonData
This function enables the modification of data associated with a specific person. It takes a pointer to a Person structure as its parameter.

```C
void modifyPersonData(Person *person) {
    int choice;

    printf("Available options for modification:\n");
    printf("1. Modify ID\n");
    printf("2. Modify Key-Value Pair\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("Enter new ID: ");
            scanf("%d", &person->id);
            break;

        case 2:
            if (!person->data) {
                printf("No data available for modification.\n");
                break;
            }

            char key[100], value[100];
            printf("Enter key to modify: ");
            scanf("%99s", key);
            printf("Enter new value: ");
            scanf("%99s", value);

            // Find the key in the linked list and modify its value
            KeyValue *key_value = person->data;
            while (key_value && strcmp(key_value->key, key) != 0) {
                key_value = key_value->next;
            }

            if (key_value) {
                // Update the linked list node with the new value
                free(key_value->value);
                key_value->value = strdup(value);
            } else {
                printf("Key not found.\n");
            }
            break;

        default:
            printf("Invalid choice.\n");
            break;
    }
}
```

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
This function saves modified data back to a file in JSON format. It takes three parameters:

const char *filename: A string representing the filename to which the data will be saved.

Person *people: A pointer to an array of Person structures containing the modified data.

int num_people: An integer representing the number of people in the people array.

```C
void saveData(const char *filename, Person *people, int num_people) {
    // Creating JSON Strucure
    cJSON *new_json = cJSON_CreateObject();
    cJSON *new_people_array = cJSON_CreateArray();

    // Creating Person Objects
    for (int i = 0; i < num_people; ++i) {
        cJSON *person_json = cJSON_CreateObject();

        // Add ID to the person's JSON object
        cJSON_AddNumberToObject(person_json, "id", people[i].id);

        // Parsing Values
        KeyValue *key_value = people[i].data;
        while (key_value) {
            cJSON *value_item;

            // Check if the value is numeric or a string
            double numeric_value;
            if (sscanf(key_value->value, "%lf", &numeric_value) == 1) {
                value_item = cJSON_CreateNumber(numeric_value);
            } else {
                char *value = key_value->value;
                size_t len = strlen(value);
                if (value[0] == '\"' && value[len - 1] == '\"') {
                    value[len - 1] = '\0';
                    value_item = cJSON_CreateString(value + 1);
                } else if (value[0] == '\"') {
                    value_item = cJSON_CreateString(value + 1); 
                } else {
                    value_item = cJSON_CreateString(value);
                }
            }

            cJSON_AddItemToObject(person_json, key_value->key, value_item);
            key_value = key_value->next;
        }

        cJSON_AddItemToArray(new_people_array, person_json);
    }

    cJSON_AddItemToObject(new_json, "people", new_people_array);

    char *json_string = cJSON_Print(new_json);

    // Saving to File
    FILE *output_file = fopen(filename, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error when trying to open file for writing.\n");
        cJSON_Delete(new_json);
        free(json_string);
        return;
    }

    printf("JSON String:\n%s\n", json_string);

    // Attempt to write to the file
    if (fprintf(output_file, "%s", json_string) < 0) {
        fprintf(stderr, "Error when trying to write into file.\n");
    } else {
        printf("Data successfully saved to %s.\n", filename);
    }

    // Close the file
    fclose(output_file);
    cJSON_Delete(new_json);
    free(json_string);
}
```

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
    * Prints the JSON string to the console for debugging purposes.
    * Attempts to open the file specified by filename for writing.
    * If successful, writes the JSON string to the file.
    * Closes the file after writing.

5. Error Handling:
    * Handles errors related to file opening and writing, printing corresponding error messages.

6. Memory Cleanup:
    * Deletes the cJSON object and frees the memory allocated for the JSON string.

### freePeople
This function is responsible for releasing the memory allocated for an array of Person structures. It takes two parameters:

Person *people: A pointer to the array of Person structures.

int num_people: An integer representing the number of Person structures in the array.

```C
void freePeople(Person *people, int num_people) {
    if (!people) return;

    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(people[i].data);
    }

    free(people);
}
```

Process:
1. Checking for NULL Pointer:
    * Checks if the people pointer is NULL. If so, returns early as there is nothing to free.

2. Freeing Key-Value Lists:
    * Iterates through each Person structure in the array.
    * Calls the freeKeyValueList function to release the memory allocated for the linked list of key-value pairs associated with each person.

3. Freeing Array Memory:
    * Frees the memory allocated for the array of Person structures.

### modifyDataBasedOnID
This function allows for the modification of data associated with a person based on their ID. It takes two parameters:

Person *people: A pointer to an array of Person structures containing the data.

int num_people: An integer representing the number of people in the people array.

```C
void modifyDataBasedOnID(Person *people, int num_people) {
    // Modify data based on person ID
    int personID;
    do {
        printf("Enter the ID of the person to modify (or enter 0 to return to the main menu): ");
        scanf("%d", &personID);
        getchar();

        if (personID == 0) {
            return;
        }
        for (int i = 0; i < num_people; ++i) {
            if (people[i].id == personID) {
                modifyPersonData(&people[i]);
                return;
            }
        }

        printf("Person with ID %d not found.\n", personID);
    } while (1);
}
```

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
This function deletes a person from the dataset based on their ID. It takes three parameters:

Person *people: A pointer to an array of Person structures containing the dataset.

int *num_people: A pointer to an integer representing the number of people in the dataset.

int id: An integer representing the ID of the person to be deleted.

```C
void deletePersonByID(Person *people, int *num_people, int id) {
    for (int i = 0; i < *num_people; ++i) {
        if (people[i].id == id) {
            // Free the key-value pairs associated with the person
            freeKeyValueList(people[i].data);

            // Move the last person in the array to the position of the deleted person
            people[i] = people[*num_people - 1];

            // Decrement the number of people
            (*num_people)--;

            printf("Person with ID %d deleted.\n", id);
            return;
        }
    }
    printf("Person with ID %d not found.\n", id);
}
```

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
