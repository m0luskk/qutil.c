CC = gcc
CFLAGS = -fstrict-aliasing -Wstrict-aliasing -Wall -Wextra -pedantic -std=c2x -Iinclude # -O2
AR = ar
ARFLAGS = rcs

SRCDIR = lib
INCDIR = include
TESTDIR = tests
BUILDDIR = build

LIB_SOURCES = $(wildcard $(SRCDIR)/*.c)
LIB_OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(LIB_SOURCES))
LIBRARY = $(BUILDDIR)/libproject.a

TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS = $(patsubst $(TESTDIR)/%.c,$(BUILDDIR)/%.o,$(TEST_SOURCES))
TEST_EXECUTABLE = $(BUILDDIR)/tests

.PHONY: all clean

all: $(LIBRARY) $(TEST_EXECUTABLE)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(TESTDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(LIB_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIBRARY)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILDDIR)