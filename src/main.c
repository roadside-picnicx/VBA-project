#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../inc/func.h"



int main() {
    int num_people = 0;
    Person *people = NULL;
    char file_name[100];
    int choice;

    do {
        // Print menu options
        printf("Menu Options:\n");
        printf("1. Load data from json file\n");
        printf("2. Print data\n");
        printf("3. Modify data based on ID\n");
        printf("4. Delete data based on ID\n");
        printf("5. Create new data\n");
        printf("6. Save data to file\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        
        // Get user choice
        scanf("%d", &choice);

        // Consume the newline character left in the input buffer
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                printf("Choose file to load: ");
                scanf("%99s", file_name);
                // Load data from a file
                people = loadData(file_name, &num_people);
                if (!people) {
                    printf("Loading data failed\n");
                    return 1;
                } else {
                    printf("Data loaded succesfully\n");
                }
                break; 

            case 2:
                for (int i = 0; i < num_people; ++i) {
                    printf("Person %d:\n", i + 1);
                    printPersonData(&people[i]);
                }
                break;

            case 3:
                printf("Modify data based on ID\n");
                modifyDataBasedOnID(people, num_people);
                break;

            case 4:
                int personID;
                if (num_people == 0) {
                    printf("No data to delete.\n");
                    break;
                }

                printf("Enter the ID of the person to delete: ");
                scanf("%d", &personID);

                deletePersonByID(people, &num_people, personID);
                break;

            case 5:
                addNewData(&people, &num_people);
                break;

            case 6:
                printf("Save data to file.\n");
                saveData(file_name, people, num_people);
                break;

            case 7:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice. Please enter a number between 1 and 7.\n");
                break;
        }

    } while (choice != 7);

    // Free the allocated memory for the 'people' array
    for (int i = 0; i < num_people; ++i) {
        free(people[i].data);
    }
    free(people);
    return 0;
}

