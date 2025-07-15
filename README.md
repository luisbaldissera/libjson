# libjson

libjson is a lightweight C library for creating, manipulating, and serializing
JSON objects. It provides a simple API for working with JSON data structures,
making it easy to integrate into your C projects.

## Features

- Create JSON objects, arrays, strings, numbers, and booleans.
- Manipulate JSON data structures with ease.
- Serialize JSON objects to strings for output or storage.
- **JSON5 support** with comments, single quotes, unquoted keys, and trailing commas.
- **YAML support** for reading and writing YAML documents.
- Unit tests to ensure reliability and correctness.

## Installation

To install libjson, you can either clone the repository or download the source
code directly. After obtaining the source code, you can build the library using
CMake

1. Navigate to the project directory:

   ```
   cd path/to/libjson
   ```

2. Create a build directory:

   ```
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:

   ```
   cmake ..
   ```

4. Build the library:
   ```
   make
   ```

## Usage

To use libjson in your project, include the header file in your source code:

```c
#include <libjson/json.h>
```

For JSON5 support:

```c
#include <libjson/json5.h>
```

For YAML support:

```c
#include <libjson/yaml.h>
```

### Example

Here is a simple example of creating a JSON object:

```c
struct json *obj = json_object(
   {"simple", json_true()},
   {"really", json_array(
      json_number(1),
      json_string("Piece of cake"))});

json_write(obj, stdout);
// {
//    "simple": true,
//    "really": [
//      1,
//      "Piece of cake"
//    ]
// }

json_free(obj);
```

Here a more verbose example:

```c
struct json *obj = json_object();
json_object_set(obj, "name", json_string("libjson"));
json_object_set(obj, "version", json_number(1.0));
json_object_set(obj, "features", json_array());

struct json *features = json_object_get(obj, "features");
json_array_push(features, json_string("Easy to use"));
json_array_push(features, json_string("Lightweight"));
json_array_push(features, json_string("Fast"));

// Serialize to stdout
json_write(obj, stdout);
// {
//   "name": "libjson",
//   "version": 1.0,
//   "features": [
//     "Easy to use",
//     "Lightweight",
//     "Fast"
//   ]
// }

// Free memory when done
json_free(obj);
```

Example on reading json objects:

```c
struct Person {
   const char *name;
   int age;
   int location[2];
};

struct json *person_json = json_read_string("{\"name\":\"Bob\",\"age\":25,\"location\":[+1234567,-9876543]}", NULL);

if (json_error(NULL)) {
   fprintf(stderr, "%s\n", json_error());
   exit(1);
}

struct Person person = {
   .name = json_string_value(json_object_get(person_json, "name")),
   .age = json_int_value(json_object_get(person_json, "age")),
   .location = {
      json_int_value(json_array_get(json_object_get(person_json, "location"), 0)),
      json_int_value(json_array_get(json_object_get(person_json, "location"), 1))
   }
};
```

Example of serializing structs to json:

```c
struct Person {
   const char *name;
   int age;
   int location[2];
};

struct json *person_to_json(struct Person *p) {
  return json_object(
    {"name", json_string(p->name)},
    {"age", json_number(p->age)},
    {"location", json_array(
       p->location[0],
       p->location[1])});
}
```

## JSON5 Support

libjson provides JSON5 support, allowing you to parse JSON5 format into standard JSON structures. JSON5 is a superset of JSON that adds several convenient features while maintaining compatibility with existing JSON parsers.

### JSON5 Features Supported

- **Comments**: Both single-line (`//`) and multi-line (`/* */`) comments
- **Trailing commas**: Optional trailing commas in arrays and objects
- **Unquoted keys**: Object keys can be unquoted if they're valid identifiers
- **Single quotes**: Strings can use single quotes in addition to double quotes
- **Keywords as keys**: Reserved words like `default`, `class` can be used as unquoted keys
- **Complex identifiers**: Support for `$`, `_`, and alphanumeric characters in unquoted keys

### Basic JSON5 Reading

```c
#include <libjson/json5.h>
#include <libjson/json.h>

const char *json5_data =
    "{\n"
    "  // This is a comment\n"
    "  name: 'John Doe',  // Unquoted key, single quotes\n"
    "  age: 30,\n"
    "  active: true,\n"
    "  scores: [95, 87, 92,], // Trailing comma\n"
    "  /* Multi-line comment\n"
    "     can span multiple lines */\n"
    "  class: 'developer', // 'class' is a keyword but valid as unquoted key\n"
    "}";

char errbuf[1024];
struct json *parsed = json5_read_string(json5_data, errbuf);

if (!parsed) {
    fprintf(stderr, "JSON5 parse error: %s\n", errbuf);
    exit(1);
}

// Access data using standard JSON API
struct json *name = json_object_get(parsed, "name");
printf("Name: %s\n", json_string_value(name));

struct json *scores = json_object_get(parsed, "scores");
printf("Number of scores: %d\n", json_array_length(scores));

json_free(parsed);
```

### Reading JSON5 from Files

```c
FILE *json5_file = fopen("config.json5", "r");
if (!json5_file) {
    perror("Failed to open JSON5 file");
    exit(1);
}

char errbuf[512];
struct json *config = json5_read(json5_file, errbuf);
if (!config) {
    fprintf(stderr, "Failed to parse JSON5: %s\n", errbuf);
    fclose(json5_file);
    exit(1);
}

// Use the config with standard JSON API...
json_free(config);
fclose(json5_file);
```

### JSON5 Comment Examples

```c
// Single-line comments
const char *single_line_comments =
    "{\n"
    "  // Configuration settings\n"
    "  timeout: 30, // seconds\n"
    "  retries: 3   // maximum attempts\n"
    "}";

// Multi-line comments
const char *multi_line_comments =
    "{\n"
    "  /*\n"
    "   * Application configuration\n"
    "   * Updated: 2025-07-15\n"
    "   */\n"
    "  debug: false,\n"
    "  /* TODO: Add more options */ version: '1.0'\n"
    "}";

struct json *config1 = json5_read_string(single_line_comments, NULL);
struct json *config2 = json5_read_string(multi_line_comments, NULL);
```

### Unquoted Keys and String Variations

```c
const char *mixed_syntax =
    "{\n"
    "  // Various key formats\n"
    "  unquoted: 'value',\n"
    "  'single-quoted': \"value\",\n"
    "  \"double-quoted\": 'value',\n"
    "  $special: 'identifiers work',\n"
    "  _underscore: 'also work',\n"
    "  default: 'keywords as keys',\n"
    "  class: 'also work',\n"
    "  \n"
    "  // String variations\n"
    "  singleQuotes: 'This uses single quotes',\n"
    "  doubleQuotes: \"This uses double quotes\",\n"
    "}";

struct json *data = json5_read_string(mixed_syntax, NULL);

// Access values normally
printf("Special: %s\n", json_string_value(json_object_get(data, "$special")));
printf("Underscore: %s\n", json_string_value(json_object_get(data, "_underscore")));
printf("Single quotes: %s\n", json_string_value(json_object_get(data, "singleQuotes")));

json_free(data);
```

### Trailing Commas Support

```c
const char *trailing_commas =
    "{\n"
    "  users: [\n"
    "    {\n"
    "      name: 'Alice',\n"
    "      role: 'admin',\n"
    "    }, // Trailing comma in object\n"
    "    {\n"
    "      name: 'Bob',\n"
    "      role: 'user',\n"
    "    }, // Another trailing comma\n"
    "  ], // Trailing comma in array\n"
    "  settings: {\n"
    "    theme: 'dark',\n"
    "    notifications: true,\n"
    "  }, // Final trailing comma\n"
    "}";

struct json *data = json5_read_string(trailing_commas, NULL);
struct json *users = json_object_get(data, "users");
printf("Number of users: %d\n", json_array_length(users));

json_free(data);
```

### Error Handling

JSON5 parsing follows the same error handling pattern as regular JSON:

```c
char errbuf[512];  // Thread-safe error buffer

const char *invalid_json5 = "{ invalid syntax here }";
struct json *result = json5_read_string(invalid_json5, errbuf);

if (!result) {
    printf("JSON5 Error: %s\n", errbuf);
    // Handle error appropriately
}

// Or use default error buffer (not thread-safe)
result = json5_read_string(invalid_json5, NULL);
if (!result) {
    printf("Error: %s\n", json_error(NULL));
}
```

### Migration from JSON to JSON5

Since JSON5 is a superset of JSON, any valid JSON is also valid JSON5:

```c
// This JSON works with both json_read_string() and json5_read_string()
const char *standard_json = "{\"name\": \"value\", \"array\": [1, 2, 3]}";

struct json *via_json = json_read_string(standard_json, NULL);
struct json *via_json5 = json5_read_string(standard_json, NULL);

// Both produce identical results
assert(json_is_object(via_json));
assert(json_is_object(via_json5));

json_free(via_json);
json_free(via_json5);
```

### Best Practices

1. **Use comments for documentation**: JSON5's comment support makes configuration files self-documenting
2. **Trailing commas for maintainability**: Makes adding/removing items easier in version control
3. **Consistent quoting**: While JSON5 allows mixed quoting, pick a style and stick to it
4. **Error handling**: Always provide error buffers in multi-threaded applications

```c
// Good: Well-formatted JSON5 with consistent style
const char *config =
    "{\n"
    "  // Database configuration\n"
    "  database: {\n"
    "    host: 'localhost',\n"
    "    port: 5432,\n"
    "    name: 'myapp',\n"
    "    ssl: true, // Always use SSL in production\n"
    "  },\n"
    "  \n"
    "  // Feature flags\n"
    "  features: {\n"
    "    newUI: false,\n"
    "    analytics: true,\n"
    "    betaFeatures: false,\n"
    "  },\n"
    "}";
```

## YAML Support

libjson provides YAML support, allowing you to read YAML documents into JSON structures and write JSON structures as YAML output.

### Basic YAML Reading

```c
#include <libjson/yaml.h>
#include <libjson/json.h>

const char *yaml_data =
    "name: John Doe\n"
    "age: 30\n"
    "active: true\n"
    "scores:\n"
    "  - 95\n"
    "  - 87\n"
    "  - 92\n";

char errbuf[1024];
struct json *parsed = yaml_read_string(yaml_data, errbuf);

if (!parsed) {
    fprintf(stderr, "YAML parse error: %s\n", errbuf);
    exit(1);
}

// Access data using standard JSON API
struct json *name = json_object_get(parsed, "name");
printf("Name: %s\n", json_string_value(name));

struct json *scores = json_object_get(parsed, "scores");
struct json *first_score = json_array_get(scores, 0);
printf("First score: %d\n", json_int_value(first_score));

json_free(parsed);
```

### YAML Writing

```c
// Create a JSON structure
struct json *person = json_object(
    (struct json_key_value){"name", json_string("Alice")},
    (struct json_key_value){"age", json_number(25)},
    (struct json_key_value){"hobbies", json_array(
        json_string("reading"),
        json_string("swimming"),
        json_string("coding")
    )}
);

// Write as YAML
yaml_write(person, stdout);
// Output:
// name: Alice
// age: 25
// hobbies:
//   - reading
//   - swimming
//   - coding

json_free(person);
```

### YAML Streams (Multiple Documents)

YAML supports multiple documents in a single file/stream, separated by `---`:

```c
const char *yaml_stream =
    "name: Alice\n"
    "role: developer\n"
    "---\n"
    "name: Bob\n"
    "role: designer\n"
    "---\n"
    "- apple\n"
    "- banana\n"
    "- cherry\n";

FILE *stream = fmemopen(yaml_stream, strlen(yaml_stream), "r");

struct json *doc1 = yaml_read_document(stream, errbuf);
struct json *doc2 = yaml_read_document(stream, errbuf);
struct json *doc3 = yaml_read_document(stream, errbuf);

// doc1 and doc2 are objects, doc3 is an array
// Process each document...

json_free(doc1);
json_free(doc2);
json_free(doc3);
fclose(stream);
```

### Reading YAML from Files

```c
FILE *yaml_file = fopen("config.yaml", "r");
if (!yaml_file) {
    perror("Failed to open YAML file");
    exit(1);
}

struct json *config = yaml_read(yaml_file, errbuf);
if (!config) {
    fprintf(stderr, "Failed to parse YAML: %s\n", errbuf);
    fclose(yaml_file);
    exit(1);
}

// Use the config...
json_free(config);
fclose(yaml_file);
```

### YAML Features Supported

- **All JSON data types**: Objects, arrays, strings, numbers, booleans, null
- **YAML-specific booleans**: `true`/`false`, `yes`/`no`, `on`/`off` (case-insensitive)
- **YAML null values**: `null`, `~`
- **Comments**: Full `#` comment support (inline and standalone)
- **Quoted strings**: Both single and double quotes
- **Unquoted strings**: Simple scalar values
- **Document streams**: Multiple documents with `---` separators
- **Nested structures**: Objects within objects, arrays within arrays
- **Mixed arrays**: Arrays containing different data types

### Error Handling

All YAML functions follow the same error handling pattern as the JSON functions:

```c
char errbuf[512];  // Thread-safe error buffer

struct json *result = yaml_read_string(yaml_input, errbuf);
if (!result) {
    // Check for error
    printf("Error: %s\n", errbuf);
}
```

Example of reading from json stream, by parsing each json individually until the
end.

```c
const char *json_stream =
"{\"index\":0, \"name\": \"First\"}\n"
"{\"index\":2, \"name\": \"Third\"}\n"
"{\"index\":1, \"name\": \"Second\"}";

FILE *stream_file = fmemopen(json_stream, strlen(json_stream), "r");

struct json *element;
while (element = json_read(stream_file, NULL)) {
   // process element
}

if (json_error(NULL)) {
   fprintf(stderr, "%s\n", json_error(NULL));
}
```

On multi-threaded applications, you can provide an error buffer so that is
re-entrant:

```c
// re-entrant error buffer
char errbuf[512];

struct json *element;

element = json_read_string("{\"name\":\"Bob\",\"age\":25,\"location\":[+1234567,-9876543]}", errbuf);
if (json_error(errbuf)) {
  fprintf(stderr, "%s\n", errbuf);
  exit(1);
}
```

## Running Tests

To run the unit tests, you can use the following command after building the
library:

```
make test
```

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an
issue for any enhancements or bug fixes.

### Technical decisions for contributing

#### Error handling

To handle errors and error messages, use the static error buffer from json.c and
provide an optional parameter `char *errbuf` that is reentrant to correctly
handle error in multi-threaded systems.

#### Usage of FILE

Simply try using it whenever possible. Prefer to delegate string based functions
to the equivalent `FILE*` implementation using in-memory files. See, for example
`json_read` and `json_read_string`.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

## TODOs

- feat: add error handling and definition to parser
- feat: generate documentation/reference.
- test: write more consistent tests and increase coverage
- feat(test): auto generate coverage
- feat: add options to `json_write`. E.g. `{ .pretty = true, .sort_keys = true, .indent = 4 }`
- optm: refactor iterators to be static in memory
- optm: implement and use binary tree for hash maps, instead of linked list
- optm: use static buffer in "raw" data structures in general
- feat: lazy json read -> only process the when `json_{*}_get()` or
  `json_{*}_value()` is called. And only until the necessary to return.
  - note: also handle errbuf in `json_{*}_get(..., errbuf)` and
    `json_{*}_value(errbuf)`
