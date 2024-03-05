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
    FILE *file;
    cJSON *json;
    char *json_string;

    file = fopen("data.json", "r");
    if (file == NULL) {
        fprintf(stderr, "Error occured when trying to open file.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *file_content = (char *)malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    fclose(file);

    file_content[file_size] = '\0';

    json = cJSON_Parse(file_content);
    free(file_content);

    if (json == NULL) {
        fprintf(stderr, "Error when parsing JSON.\n");
        return 1;
    }

    // Getting data from json
    cJSON *person_json = cJSON_GetObjectItem(json, "person");
    Person person;
    person.name = strdup(cJSON_GetObjectItem(person_json, "name")->valuestring);
    person.age = cJSON_GetObjectItem(person_json, "age")->valueint;
    person.salary = cJSON_GetObjectItem(person_json, "salary")->valuedouble;


    // Print data
    printf("Name: %s\n", person.name);
    printf("Age: %d\n", person.age);
    printf("Salary: %.2f\n", person.salary);

    
    cJSON_Delete(json);

 
    cJSON *new_json = cJSON_CreateObject();
    cJSON_AddItemToObject(new_json, "person", cJSON_CreateObject());
    cJSON_AddStringToObject(new_json, "name", person.name);
    cJSON_AddNumberToObject(new_json, "age", person.age);
    cJSON_AddNumberToObject(new_json, "salary", person.salary);

    json_string = cJSON_Print(new_json);

    file = fopen("output.json", "w");
    if (file == NULL) {
        fprintf(stderr, "Error when trying to write into file.\n");
        return 1;
    }

    fprintf(file, "%s", json_string);

    // Releasing memory
    fclose(file);
    free(person.name);
    cJSON_Delete(new_json);
    free(json_string);

    return 0;
}
