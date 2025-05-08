#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libjson/closure.h>
#include <libjson/linked_list.h>

// A simple structure to store in our linked list
struct Person
{
    char name[64];
    int age;
};

// Pretty-print a person
void print_person(void *data)
{
    struct Person *person = (struct Person *)data;
    printf("Person: %s, Age: %d\n", person->name, person->age);
}

// Create a new Person
struct Person *create_person(const char *name, int age)
{
    struct Person *person = malloc(sizeof(struct Person));
    strcpy(person->name, name);
    person->age = age;
    return person;
}

// Custom closure function that checks if a person has a specific age
static int person_with_age(void *value, void *ctx)
{
    struct Person *person = (struct Person *)value;
    int target_age = *(int *)ctx;
    return person->age == target_age;
}

// Custom closure function that checks if a person has a specific name
static int person_with_name(void *value, void *ctx)
{
    struct Person *person = (struct Person *)value;
    const char *target_name = (const char *)ctx;
    return strcmp(person->name, target_name) == 0;
}

// Create a closure for finding a person with a specific age
struct closure *age_equals(int age)
{
    int *age_ptr = malloc(sizeof(int));
    *age_ptr = age;
    return closure_new(person_with_age, age_ptr);
}

// Create a closure for finding a person with a specific name
struct closure *name_equals(const char *name)
{
    char *name_copy = strdup(name);
    return closure_new(person_with_name, name_copy);
}

int main()
{
    // Create a linked list of people
    struct Person *john = create_person("John", 30);
    struct Person *jane = create_person("Jane", 25);
    struct Person *bob = create_person("Bob", 40);
    struct Person *alice = create_person("Alice", 25);

    struct linked_list *people = linked_list_new(john);
    linked_list_insert(people, jane);
    linked_list_insert(linked_list_next(people), bob);
    linked_list_insert(linked_list_next(linked_list_next(people)), alice);

    printf("All people in the list:\n");
    linked_list_foreach(people, print_person);
    printf("\n");

    // Find people by age using closures
    struct linked_list *prev = NULL;
    struct closure *age25 = age_equals(25);
    struct linked_list *found = linked_list_find(people, age25, &prev);

    if (found)
    {
        printf("Found person with age 25:\n");
        print_person(linked_list_value(found));

        // Find the next person with age 25
        found = linked_list_find(linked_list_next(found), age25, &prev);
        if (found)
        {
            printf("Found another person with age 25:\n");
            print_person(linked_list_value(found));
        }
    }
    closure_free(age25);

    // Find a person by name using the cleaner style you requested
    prev = NULL;
    found = linked_list_find(people, name_equals("Bob"), &prev);
    if (found)
    {
        printf("\nFound person named Bob:\n");
        print_person(linked_list_value(found));
    }

    // Clean up
    linked_list_foreach(people, free); // Free all Person structs
    linked_list_free(people);          // Free the linked list

    return 0;
}