CC = gcc
CFLAGS = -Wall -Wextra -std=c2x -pedantic -g
INCLUDES = -Iinclude -Itests

LIBRARY_NAME = libqutil.a
LIB_DIR = lib
INCLUDE_DIR = include
TESTS_DIR = tests
BUILD_DIR = build
LIB_BUILD_DIR = $(BUILD_DIR)/lib
TESTS_BUILD_DIR = $(BUILD_DIR)/tests

LIB_SOURCES = $(wildcard $(LIB_DIR)/*.c)
TEST_SOURCES = $(wildcard $(TESTS_DIR)/*.c)

LIB_OBJECTS = $(patsubst $(LIB_DIR)/%.c,$(LIB_BUILD_DIR)/%.o,$(LIB_SOURCES))
TEST_OBJECTS = $(patsubst $(TESTS_DIR)/%.c,$(TESTS_BUILD_DIR)/%.o,$(TEST_SOURCES))

TEST_OBJECTS_FILTERED = $(filter-out $(TESTS_BUILD_DIR)/test_main.o,$(TEST_OBJECTS))

LIBRARY_TARGET = $(BUILD_DIR)/$(LIBRARY_NAME)
TEST_EXECUTABLE = $(BUILD_DIR)/test_runner

all: $(LIBRARY_TARGET) $(TEST_EXECUTABLE)

$(LIBRARY_TARGET): $(LIB_OBJECTS)
	ar rcs $@ $^
	@echo "Library $@ created"

$(LIB_BUILD_DIR)/%.o: $(LIB_DIR)/%.c | $(LIB_BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TESTS_BUILD_DIR)/%.o: $(TESTS_DIR)/%.c | $(TESTS_BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIBRARY_TARGET)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJECTS) -L$(BUILD_DIR) -lqutil
	@echo "Test runner $@ created"

$(BUILD_DIR):
	mkdir -p $@

$(LIB_BUILD_DIR):
	mkdir -p $@

$(TESTS_BUILD_DIR):
	mkdir -p $@

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR)

info:
	@echo "Library sources: $(LIB_SOURCES)"
	@echo "Library objects: $(LIB_OBJECTS)"
	@echo "Test sources: $(TEST_SOURCES)"
	@echo "Test objects: $(TEST_OBJECTS)"
	@echo "Filtered test objects: $(TEST_OBJECTS_FILTERED)"

.PHONY: all clean test info