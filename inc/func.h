#ifndef FUNC_H
#define FUNC_H

// Structure to represent dynamic key-value pairs
typedef struct KeyValue {
    char *key;
    char *value;
    struct KeyValue *next;
} KeyValue;

// Structure to represent a person
typedef struct {
    int id;         
    KeyValue *data;  
} Person;

int addKeyValue(KeyValue **list, const char *key, const char *value);
void freeKeyValueList(KeyValue **list);
Person *loadData(const char *filename, int *num_people);
void addNewData(Person **people, int *num_people);
void printPersonData(const Person *person);
void modifyPersonData(Person *person);
void saveData(const char *filename, Person *people, int num_people);
void freePeople(Person *people, int num_people);
void modifyDataBasedOnID(Person *people, int num_people);
void deletePersonByID(Person *people, int *num_people, int id);

#endif /* FUNC_H */
