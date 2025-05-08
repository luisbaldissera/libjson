# libjson

libjson is a lightweight C library for creating, manipulating, and serializing JSON objects. It provides a simple API for working with JSON data structures, making it easy to integrate into your C projects.

## Features

- Create JSON objects, arrays, strings, numbers, and booleans.
- Manipulate JSON data structures with ease.
- Serialize JSON objects to strings for output or storage.
- Unit tests to ensure reliability and correctness.

## Installation

To install libjson, you can either clone the repository or download the source code directly. After obtaining the source code, you can build the library using either CMake or Make.

### Using CMake

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

### Using Make

1. Navigate to the project directory:
   ```
   cd path/to/libjson
   ```

2. Build the library:
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
JSON json = JSON_object();
JSON_object_set(json, "name", JSON_string("libjson"));
JSON_object_set(json, "version", JSON_decimal(1.0));
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