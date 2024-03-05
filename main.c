#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"

typedef struct {
    char *name;
    int age;
    double salary;
} Person;

int main() {
    // Part of the code for reading JSON and getting data into memory
    FILE *file = fopen("data.json", "r");
    if (file == NULL) {
        fprintf(stderr, "Error occurred when trying to open file.\n");
        return 1;
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
        return 1;
    }

    cJSON *people_array = cJSON_GetObjectItem(json, "people");
    if (people_array == NULL || !cJSON_IsArray(people_array)) {
        fprintf(stderr, "Invalid or missing 'people' array in JSON.\n");
        cJSON_Delete(json);
        return 1;
    }

    int num_people = cJSON_GetArraySize(people_array);

    // Memory allocation for an array of people
    Person *people = (Person *)malloc(num_people * sizeof(Person));

    // Getting data from JSON into memory
    for (int i = 0; i < num_people; ++i) {
        cJSON *person_json = cJSON_GetArrayItem(people_array, i);

        people[i].name = strdup(cJSON_GetObjectItem(person_json, "name")->valuestring);
        people[i].age = cJSON_GetObjectItem(person_json, "age")->valueint;
        people[i].salary = cJSON_GetObjectItem(person_json, "salary")->valuedouble;
    }

    cJSON_Delete(json);

    cJSON *new_json = cJSON_CreateObject();
    cJSON *new_people_array = cJSON_CreateArray();

    // Adding individual persons to an array in JSON
    for (int i = 0; i < num_people; ++i) {
        cJSON *person_json = cJSON_CreateObject();
        cJSON_AddStringToObject(person_json, "name", people[i].name);
        cJSON_AddNumberToObject(person_json, "age", people[i].age);
        cJSON_AddNumberToObject(person_json, "salary", people[i].salary);
        cJSON_AddItemToArray(new_people_array, person_json);
    }

    // Adding a person field to the main JSON object
    cJSON_AddItemToObject(new_json, "people", new_people_array);

    // Conversion to text
    char *json_string = cJSON_Print(new_json);

    // Saving into file
    FILE *output_file = fopen("output.json", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error when trying to write into file.\n");
        return 1;
    }

    fprintf(output_file, "%s", json_string);

    // Releasing memory
    fclose(output_file);
    cJSON_Delete(new_json);
    free(json_string);

    for (int i = 0; i < num_people; ++i) {
        free(people[i].name);
    }
    free(people);

    return 0;
}
