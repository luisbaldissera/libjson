# libjson

libjson is a lightweight C library for creating, manipulating, and serializing JSON objects. It provides a simple API for working with JSON data structures, making it easy to integrate into your C projects.

## Features

- Create JSON objects, arrays, strings, numbers, and booleans.
- Manipulate JSON data structures with ease.
- Serialize JSON objects to strings for output or storage.
- Unit tests to ensure reliability and correctness.

## Installation

To install libjson, you can either clone the repository or download the source code directly. After obtaining the source code, you can build the library using CMake

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
struct json *obj = json_object();
json_object_set(obj, "name", json_string("libjson"));
json_object_set(obj, "version", json_number(1.0));
json_object_set(obj, "features", json_array());

struct json *features = json_object_get(obj, "features");
json_array_push(features, json_string("Easy to use"));
json_array_push(features, json_string("Lightweight"));
json_array_push(features, json_string("Fast"));

// Serialize to stdout
json_fwrite(obj, stdout);
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

## Running Tests

To run the unit tests, you can use the following command after building the library:

```
make test
```

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

## TODOs

- feat: implement escaping correctly
   - object keys
   - string values
   - getting strings (resolve escaping)
- feat: implement json_read(in: FILE): json*
- refac: replace closure for iterators
   - hash table set
- refac: refactor iterators to be static in memory