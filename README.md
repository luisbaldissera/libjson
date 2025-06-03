# libjson

libjson is a lightweight C library for creating, manipulating, and serializing
JSON objects. It provides a simple API for working with JSON data structures,
making it easy to integrate into your C projects.

## Features

- Create JSON objects, arrays, strings, numbers, and booleans.
- Manipulate JSON data structures with ease.
- Serialize JSON objects to strings for output or storage.
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

// NULL is the default error buffer
struct json *person_json = json_read_string("{\"name\":\"Bob\",\"age\":25,\"location\":[+1234567,-9876543]}", NULL);

// NULL is the default error buffer
if (json_error(NULL)) {
   fprintf(stderr, "%s\n", json_error(NULL));
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
- feat: add support for json5 (in different `json5.h` header)
  - `struct json * json5_read(FILE*)`
  - `struct json * json5_read_string(const char *)`
- feat: add support for YAML (in different `yaml.h` header)
  - `struct json *yaml_read(FILE*)`
  - `struct json *yaml_read_string(const char *)`
  - `void yaml_write(struct json *, FILE*)`
- optm: refactor iterators to be static in memory
- optm: implement and use binary tree for hash maps, instead of linked list
- optm: use static buffer in "raw" data structures in general
- feat: lazy json read -> only process the when `json_{*}_get()` or
  `json_{*}_value()` is called. And only until the necessary to return.
  - note: also handle errbuf in `json_{*}_get(..., errbuf)` and
    `json_{*}_value(errbuf)`
