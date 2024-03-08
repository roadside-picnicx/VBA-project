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
        fprintf(stderr, "Error occurred when trying to open file.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *file_content = (char *)malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (json == NULL) {
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

// FModify data for a specific person
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

int main() {
    int num_people;
    Person *people = NULL;

    // Load data from a file
    people = loadData("data.json", &num_people);
    if (people == NULL) {
        return 1;
    }

    int choice;

    // Menu loop
    do {
        // Print menu options
        printf("Menu Options:\n");
        printf("1. See loaded data\n");
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
                // See loaded data
                for (int i = 0; i < num_people; ++i) {
                    printf("Person %d:\n", i + 1);
                    printPersonData(&people[i]);
                }
                break;

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
    for (int i = 0; i < num_people; ++i) {
        freeKeyValueList(people[i].data);
    }
    free(people);

    return 0;
}
