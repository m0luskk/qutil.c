CC = gcc
CFLAGS = -g -fstrict-aliasing -Wstrict-aliasing -Wall -Wextra -pedantic -std=c2x -Iinclude -fsanitize=undefined -fsanitize=address -fsanitize=leak # -O2 # -fsanitize=thread
LDFLAGS = -lcheck -lsubunit -lm
BUILD_DIR = build
TARGET = $(BUILD_DIR)/tests
SRC_DIR = tests

TEST_SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)