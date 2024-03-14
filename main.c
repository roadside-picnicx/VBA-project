#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"

// Structure to represent dynamic key-value pairs
typedef struct KeyValue {
    char *key;
    char *value;
    struct KeyValue *next;
} KeyValue;

// Structure to represent a person
typedef struct {
    int id;          // Identifier for each person
    KeyValue *data;  // Linked list of key-value pairs
} Person;

// Add a new key-value pair to the linked list
void addKeyValue(KeyValue **list, const char *key, const char *value) {
    KeyValue *newNode = (KeyValue *)malloc(sizeof(KeyValue));
    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->next = *list;
    *list = newNode;
}

// Release memory allocated for the linked list
void freeKeyValueList(KeyValue *list) {
    while (list != NULL) {
        KeyValue *temp = list;
        list = list->next;
        free(temp->key);
        free(temp->value);
        free(temp);
    }
}

// Function to load data from a file and parse it into memory
Person *loadData(const char *filename, int *num_people) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error occurred when trying to open file '%s'.\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *file_content = (char *)malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    printf("File content: %s\n", file_content); // Diagnostic print

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        fprintf(stderr, "Error when parsing JSON.\n");
        return NULL;
    }

    cJSON *people_array = cJSON_GetObjectItem(json, "people");
    if (people_array == NULL || !cJSON_IsArray(people_array)) {
        fprintf(stderr, "Invalid or missing 'people' array in JSON.\n");
        cJSON_Delete(json);
        return NULL;
    }

    *num_people = cJSON_GetArraySize(people_array);

    // Memory allocation for an array of people
    Person *people = (Person *)malloc((*num_people) * sizeof(Person));

    if (people == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        cJSON_Delete(json);
        return NULL;
    }

    // Getting data from JSON into memory
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

    cJSON_Delete(json);

    return people;
}



// Print data of specific person
void printPersonData(const Person *person) {
    printf("Person ID: %d\n", person->id);
    printf("Data:\n");

    KeyValue *key_value = person->data;
    while (key_value != NULL) {
        printf("  %s: %s\n", key_value->key, key_value->value);
        key_value = key_value->next;
    }
    printf("\n");
}

// Modify data for a specific person
void modifyPersonData(Person *person) {
    int choice;

    // Print available options for modification
    printf("Available options for modification:\n");
    printf("1. Modify ID\n");
    printf("2. Modify Key-Value Pair\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            // Modify ID
            printf("Enter new ID: ");
            scanf("%d", &person->id);
            break;

        case 2:
            // Modify Key-Value Pair
            if (person->data == NULL) {
                printf("No data available for modification.\n");
                break;
            }

            char key[100], value[100];
            printf("Enter key to modify: ");
            scanf("%s", key);
            printf("Enter new value: ");
            scanf("%s", value);

            // Find the key in the linked list and modify its value
            KeyValue *key_value = person->data;
            while (key_value != NULL && strcmp(key_value->key, key) != 0) {
                key_value = key_value->next;
            }

            if (key_value != NULL) {
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

// Function to save modified data back to a file
void saveData(const char *filename, Person *people, int num_people) {
    cJSON *new_json = cJSON_CreateObject();
    cJSON *new_people_array = cJSON_CreateArray();

    for (int i = 0; i < num_people; ++i) {
        cJSON *person_json = cJSON_CreateObject();

        // Add ID to the person's JSON object
        cJSON_AddNumberToObject(person_json, "id", people[i].id);

        KeyValue *key_value = people[i].data;
        while (key_value != NULL) {
            cJSON *value_item;

            // Check if the value is numeric or a string
            double numeric_value;
            if (sscanf(key_value->value, "%lf", &numeric_value) == 1) {
                // If it's numeric, use cJSON_CreateNumber
                value_item = cJSON_CreateNumber(numeric_value);
            } else {
                // If it's not numeric, manually remove extra quotes
                size_t len = strlen(key_value->value);
                if (key_value->value[0] == '\"' && key_value->value[len - 1] == '\"') {
                    key_value->value[len - 1] = '\0';
                    value_item = cJSON_CreateString(key_value->value + 1);
                } else {
                    value_item = cJSON_CreateString(key_value->value);
                }
            }

            cJSON_AddItemToObject(person_json, key_value->key, value_item);
            key_value = key_value->next;
        }

        cJSON_AddItemToArray(new_people_array, person_json);
    }

    cJSON_AddItemToObject(new_json, "people", new_people_array);

    char *json_string = cJSON_Print(new_json);

    FILE *output_file = fopen(filename, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error when trying to open file for writing.\n");
        cJSON_Delete(new_json);
        free(json_string);
        return;
    }

    // Print the JSON string to the console for debugging
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

// Function to free memory allocated for Person array
void freePeople(Person *people, int num_people) {
    if (people == NULL) {
        return; // Nothing to free
    }

    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(people[i].data);
    }

    free(people);
}

void modifyDataBasedOnID(Person *people, int num_people) {
    // Modify data based on person ID
    int personID;
    do {
        printf("Enter the ID of the person to modify (or enter 0 to return to the main menu): ");
        scanf("%d", &personID);

        // Consume the newline character left in the input buffer
        getchar();

        if (personID == 0) {
            // Return to main menu
            return;
        }

        // Find the person with the specified ID
        Person *targetPerson = NULL;
        for (int i = 0; i < num_people; ++i) {
            if (people[i].id == personID) {
                targetPerson = &people[i];
                break;
            }
        }

        if (targetPerson != NULL) {
            modifyPersonData(targetPerson);
        } else {
            printf("Person with ID %d not found.\n", personID);
        }
    } while (personID != 0);  // Continue until user chooses to return to main menu
}

// Function to delete a person based on their ID
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

void createNewJSON(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error occurred when trying to create file '%s'.\n", filename);
        return;
    }
    fclose(file);
    printf("New JSON file '%s' created.\n", filename);
}

// Function to create new keys in the JSON file
void createNewKeys(cJSON *json) {
    char key[100];
    int num_keys;

    printf("Enter the number of keys to add: ");
    if (scanf("%d", &num_keys) != 1) {
        printf("Error: Invalid input for number of keys.\n");
        return;
    }
    getchar(); // Consume newline character

    for (int i = 0; i < num_keys; ++i) {
        printf("Enter key %d: ", i + 1);
        if (fgets(key, sizeof(key), stdin) == NULL) {
            printf("Error: Unable to read input.\n");
            return;
        }
        key[strcspn(key, "\n")] = '\0'; // Remove trailing newline character

        // Add key to the JSON object with cJSON_NULL value
        cJSON_AddItemToObject(json, key, cJSON_CreateNull());
        if (json == NULL) {
            printf("Error: Unable to add key '%s' to JSON object.\n", key);
            return;
        }
    }

    printf("New keys added to the JSON object.\n");
}

int personIDCount = 1;
void createPersonData(cJSON *json) {
    if (json == NULL) {
        printf("Error: cJSON object is NULL.\n");
        return;
    }

    cJSON *person_json = cJSON_CreateObject();
    if (person_json == NULL) {
        printf("Error: Failed to create cJSON object for person data.\n");
        return;
    }

    cJSON *key = NULL;
    cJSON_ArrayForEach(key, json) {
        if (strcmp(key->string, "people") != 0) {
            const char *keyName = key->string;
            char value[100];

            printf("Enter value for key '%s': ", keyName);
            if (scanf("%s", value) != 1) {
                printf("Error: Invalid input for value.\n");
                cJSON_Delete(person_json); // Cleanup cJSON object
                return;
            }

            cJSON_AddStringToObject(person_json, keyName, value);
        }
    }

    // Add ID to cJSON object (in lowercase)
    cJSON_AddNumberToObject(person_json, "id", personIDCount++);
    
    cJSON *people_array = cJSON_GetObjectItem(json, "people");
    if (people_array == NULL) {
        people_array = cJSON_AddArrayToObject(json, "people");
        if (people_array == NULL) {
            printf("Error: Failed to create cJSON array for people.\n");
            cJSON_Delete(person_json); // Cleanup cJSON object
            return;
        }
    }

    cJSON_AddItemToArray(people_array, person_json);

    printf("Person data added to the JSON object.\n");
}

int main() {
    int num_people;
    Person *people = NULL;

    int choice;
    char file_name[100];

    // Menu loop
    do {
        // Print menu options
        printf("Menu Options:\n");
        printf("1. Load data from json file\n");
        printf("2. Create new json file\n");
        printf("3. Delete json file\n");
        printf("4. Exit\n");

        // Get user choice
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Consume the newline character left in the input buffer
        while (getchar() != '\n');

        switch (choice) {
            case 1:

                printf("Choose file to load: ");
                scanf("%s", file_name);

                // Load data from a file
                people = loadData(file_name, &num_people);
                if (people == NULL) {
                    return 1;
                } else {
                    int sub_choice;
                    do {
                        printf("\nLoad data - menu:\n");
                        printf("1. Print data\n");
                        printf("2. Modify data based on ID\n");
                        printf("3. Delete data based on ID\n");
                        printf("4. Save data to file\n");
                        printf("5. Return to Main menu\n");
                        printf("Enter your choice: ");
                        scanf("%d", &sub_choice);

                        switch (sub_choice) {
                            case 1:
                                for (int i = 0; i < num_people; ++i) {
                                    printf("Person %d:\n", i + 1);
                                    printPersonData(&people[i]);
                                }
                                break;
                            case 2:
                                printf("Modify data based on ID\n");
                                modifyDataBasedOnID(people, num_people);
                                break;
                            case 3:
                                int personID;
                                if (num_people == 0) {
                                    printf("No data to delete.\n");
                                    break;
                                }

                                printf("Enter the ID of the person to delete: ");
                                scanf("%d", &personID);

                                deletePersonByID(people, &num_people, personID);
                                break;
                            case 4:
                                printf("Save data to file.");
                                saveData(file_name, people, num_people);
                                break;
                            case 5:
                                // Return to Main menu
                                break;
                            default:
                                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                                break;
                        }
                    } while (sub_choice != 5);
                }
                freePeople(people, num_people);
                num_people = 0;
                break;  // Don't forget to break after each case

            case 2:
                    // Create new JSON file and provide menu for manipulating data
                    printf("Enter the name for the new JSON file: ");
                    scanf("%s", file_name);

                    createNewJSON(file_name); // Function to create new JSON file

                    cJSON *json = cJSON_CreateObject(); // Create cJSON object for the new JSON file

                    if (json == NULL) {
                        printf("Error creating cJSON object.\n");
                        break;
                    }

                    int sub_choice;
                    do {
                        // Print menu options
                        printf("\nJSON File Manipulation Options:\n");
                        printf("1. Print data\n");
                        printf("2. Create new keys\n");
                        printf("3. Create person data\n");
                        printf("4. Edit person data\n");
                        printf("5. Sabe data to json file\n");
                        printf("6. Return to Main menu\n");
                        printf("Enter your choice: ");
                        scanf("%d", &sub_choice);

                        // Consume the newline character left in the input buffer
                        while (getchar() != '\n');

                        switch (sub_choice) {
                            case 1:
                                // Print data
                                // Implement function to print data
                                break;
                            case 2:
                                // Create new keys
                                createNewKeys(json);
                                break;
                            case 3:
                                // Create person data
                                createPersonData(json);
                                break;
                            case 4:
                                // Edit person data
                                // Implement function to edit person data
                                //editPersonData(json);
                                break;
                            case 5:
                                // Save data
                                FILE *output_file = fopen(file_name, "w");
                                if (output_file == NULL) {
                                    printf("Error: Unable to open file for writing.\n");
                                    return 1;
                                }
                                fprintf(output_file, "%s", cJSON_Print(json));
                                fclose(output_file);
                                printf("Data saved to json file.\n");
                                break;
                            case 6:
                                break;
                            default:
                                printf("Invalid choice. Please enter a number between 1 and 5.\n");
                                break;
                        }
                    } while (sub_choice != 6);

                    // Free cJSON object
                    cJSON_Delete(json);
                    break;

            case 3:
                break;
            case 4:
                // Exit
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                break;
        }
    } while (choice != 4);

    // Release memory
    // freePeople(people, num_people);

    return 0;
}
