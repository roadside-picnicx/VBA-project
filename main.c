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


int main() {
    int num_people;
    Person *people = NULL;

    int choice;
    char file_name[100];

    // Menu loop
    do {
        // Print menu options
        printf("Menu Options:\n");
        printf("1. Load data\n");
        printf("2. Modify data based on person ID\n");
        printf("3. Save data to output file\n");
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
                                printf("Delete data based on ID\n");
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
                // Modify data based on person ID
                int personID;
                printf("Enter the ID of the person to modify: ");
                scanf("%d", &personID);

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
                break;

            case 3:
                // Save data to output file
                printf("Before saveData\n");
                saveData("output.json", people, num_people);
                printf("After saveData\n");
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
    freePeople(people, num_people);

    return 0;
}
