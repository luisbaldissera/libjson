#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test YAML stream with multiple documents
    const char *yaml_stream = 
        "# First document\n"
        "name: Alice\n"
        "age: 30\n"
        "---\n"
        "# Second document\n"
        "name: Bob\n"
        "age: 25\n"
        "active: true\n"
        "---\n"
        "# Third document (array)\n"
        "- apple\n"
        "- banana\n"
        "- cherry\n";

    // Create a temporary file with the YAML stream
    FILE *temp = tmpfile();
    assert(temp != NULL);
    fprintf(temp, "%s", yaml_stream);
    rewind(temp);

    // Read first document
    struct json *doc1 = yaml_read_document(temp, errbuf);
    assert(doc1 != NULL);
    assert(json_is_object(doc1));
    
    struct json *name = json_object_get(doc1, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    struct json *age = json_object_get(doc1, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    
    json_free(doc1);

    // Read second document
    struct json *doc2 = yaml_read_document(temp, errbuf);
    assert(doc2 != NULL);
    assert(json_is_object(doc2));
    
    name = json_object_get(doc2, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Bob") == 0);
    
    age = json_object_get(doc2, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 25);
    
    struct json *active = json_object_get(doc2, "active");
    assert(json_is_boolean(active));
    assert(active == json_true());
    
    json_free(doc2);

    // Read third document (array)
    struct json *doc3 = yaml_read_document(temp, errbuf);
    assert(doc3 != NULL);
    assert(json_is_array(doc3));
    assert(json_array_length(doc3) == 3);
    
    struct json *item = json_array_get(doc3, 0);
    assert(json_is_string(item));
    assert(strcmp(json_string_value(item), "apple") == 0);
    
    item = json_array_get(doc3, 1);
    assert(json_is_string(item));
    assert(strcmp(json_string_value(item), "banana") == 0);
    
    item = json_array_get(doc3, 2);
    assert(json_is_string(item));
    assert(strcmp(json_string_value(item), "cherry") == 0);
    
    json_free(doc3);

    // Try to read fourth document (should be NULL)
    struct json *doc4 = yaml_read_document(temp, errbuf);
    assert(doc4 == NULL);

    fclose(temp);

    // Test simple two-document stream
    const char *simple_stream = 
        "foo: bar\n"
        "---\n"
        "baz: qux\n";

    temp = tmpfile();
    assert(temp != NULL);
    fprintf(temp, "%s", simple_stream);
    rewind(temp);

    struct json *simple1 = yaml_read_document(temp, errbuf);
    assert(simple1 != NULL);
    assert(json_is_object(simple1));
    
    struct json *foo = json_object_get(simple1, "foo");
    assert(json_is_string(foo));
    assert(strcmp(json_string_value(foo), "bar") == 0);
    
    json_free(simple1);

    struct json *simple2 = yaml_read_document(temp, errbuf);
    assert(simple2 != NULL);
    assert(json_is_object(simple2));
    
    struct json *baz = json_object_get(simple2, "baz");
    assert(json_is_string(baz));
    assert(strcmp(json_string_value(baz), "qux") == 0);
    
    json_free(simple2);

    // Should be no more documents
    struct json *simple3 = yaml_read_document(temp, errbuf);
    assert(simple3 == NULL);

    fclose(temp);

    printf("YAML streams test passed!\n");
    return 0;
}