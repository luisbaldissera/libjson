CC = gcc
CFLAGS = -Iinclude -Iinclude/libjson -Wall -Wextra -pedantic
SRC = src/json.c src/ds.c
OBJ = $(SRC:.c=.o)
LIB = libjson.a
TESTS = tests/test_json_parsing.c tests/test_json_creation.c tests/test_json_manipulation.c
TEST_OBJ = $(TESTS:.c=.o)
TEST_EXEC = test_json

.PHONY: all clean test

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_OBJ) -L. -ljson
	./$(TEST_EXEC)

clean:
	rm -f $(OBJ) $(LIB) $(TEST_EXEC) $(TEST_OBJ)