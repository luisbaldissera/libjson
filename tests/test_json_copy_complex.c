#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Create complex nested structure for copying
    struct json *original = json_object(
        (struct json_key_value){"person", json_object(
            (struct json_key_value){"name", json_string("Bob")},
            (struct json_key_value){"age", json_number(42)},
            (struct json_key_value){"hobbies", json_array(
                json_string("reading"),
                json_string("coding"),
                json_object((struct json_key_value){"sport", json_string("tennis")})
            )}
        )},
        (struct json_key_value){"metadata", json_object(
            (struct json_key_value){"version", json_number(1.0)},
            (struct json_key_value){"active", json_true()},
            (struct json_key_value){"tags", json_array(
                json_string("important"),
                json_null(),
                json_false()
            )}
        )}
    );

    // Make a deep copy
    struct json *copy = json_copy(original);
    assert(copy != NULL);
    assert(copy != original); // Different objects

    // Verify structure matches
    assert(json_is_object(copy));
    assert(json_object_length(copy) == 2);

    // Check person object
    struct json *person = json_object_get(copy, "person");
    struct json *orig_person = json_object_get(original, "person");
    assert(person != orig_person); // Different objects
    assert(json_is_object(person));
    assert(json_object_length(person) == 3);

    struct json *name = json_object_get(person, "name");
    struct json *orig_name = json_object_get(orig_person, "name");
    assert(name != orig_name); // Different string objects
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Bob") == 0);

    struct json *age = json_object_get(person, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 42);

    // Check hobbies array
    struct json *hobbies = json_object_get(person, "hobbies");
    struct json *orig_hobbies = json_object_get(orig_person, "hobbies");
    assert(hobbies != orig_hobbies); // Different arrays
    assert(json_is_array(hobbies));
    assert(json_array_length(hobbies) == 3);

    // Check hobby strings are copied
    struct json *hobby1 = json_array_get(hobbies, 0);
    struct json *orig_hobby1 = json_array_get(orig_hobbies, 0);
    assert(hobby1 != orig_hobby1); // Different string objects
    assert(strcmp(json_string_value(hobby1), "reading") == 0);

    // Check nested object in array
    struct json *hobby_obj = json_array_get(hobbies, 2);
    struct json *orig_hobby_obj = json_array_get(orig_hobbies, 2);
    assert(hobby_obj != orig_hobby_obj); // Different objects
    assert(json_is_object(hobby_obj));
    assert(strcmp(json_string_value(json_object_get(hobby_obj, "sport")), "tennis") == 0);

    // Check metadata object
    struct json *metadata = json_object_get(copy, "metadata");
    assert(json_is_object(metadata));
    assert(json_object_length(metadata) == 3);

    // Check tags array
    struct json *tags = json_object_get(metadata, "tags");
    assert(json_is_array(tags));
    assert(json_array_length(tags) == 3);
    assert(strcmp(json_string_value(json_array_get(tags, 0)), "important") == 0);
    assert(json_array_get(tags, 1) == json_null()); // Singleton nulls should be same
    assert(json_array_get(tags, 2) == json_false()); // Singleton booleans should be same

    // Modify original to ensure independence
    json_object_set(json_object_get(original, "person"), "name", json_string("Modified"));
    assert(strcmp(json_string_value(json_object_get(json_object_get(copy, "person"), "name")), "Bob") == 0);

    json_free(original);
    json_free(copy);
    return 0;
}
