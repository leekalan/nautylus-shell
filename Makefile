# Directories
BUILD := build
CLANG_BUILD := build/clang
TEST_BUILD := build/tests
CLANG_TEST_BUILD := build/clang/tests

# GCC flags
CC := gcc
CFLAGS := -Wall -Wextra -O2 -Iinclude

# Clang flags
CLANG_CC := clang

UNAME_S := $(shell uname -s)

CLANG_PLATFORM_FLAGS :=
ifeq ($(UNAME_S),Linux)
  CLANG_PLATFORM_FLAGS += -D_FORTIFY_SOURCE=2
endif

CLANG_FLAGS := -Wall -Wextra -O1 -g -Iinclude \
  -fsanitize=undefined,address -fno-omit-frame-pointer -fno-common \
  -fno-sanitize-recover=all -Wpedantic -Wshadow $(CLANG_PLATFORM_FLAGS)

# Default target
default: $(EXEC)

### SHARED ###

# All sources including main.c
SRC := $(shell find src -name '*.c')

# Library sources and objects exclude main.c for tests
LIB_SRC := $(filter-out src/main.c, $(SRC))

# Test utility
TEST_UTIL := $(wildcard tests/util/*.c)

# Test sources and executables
TEST_SRC := $(filter-out $(TEST_UTIL), $(shell find tests -name '*.c'))

### GCC ###

# Build
OBJ := $(patsubst src/%.c, $(BUILD)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)
EXEC := $(BUILD)/app

# Library objects exclude main.c for tests
LIB_OBJ := $(patsubst src/%.c, $(BUILD)/%.o, $(LIB_SRC))

# Test utility objects
TEST_UTIL_OBJ := $(patsubst tests/%.c, $(TEST_BUILD)/%.o, $(TEST_UTIL))

# Test executables
TEST_EXEC := $(patsubst tests/%.c, $(TEST_BUILD)/%, $(TEST_SRC))

### CLANG ###

# Build
CLANG_OBJ := $(patsubst src/%.c, $(CLANG_BUILD)/%.o, $(SRC))
CLANG_DEP := $(CLANG_OBJ:.o=.d)
CLANG_EXEC := $(CLANG_BUILD)/app

# Library objects exclude main.c for tests
CLANG_LIB_OBJ := $(patsubst src/%.c, $(CLANG_BUILD)/%.o, $(LIB_SRC))

# Test utility objects
CLANG_TEST_UTIL_OBJ := $(patsubst tests/util/%.c, $(CLANG_TEST_BUILD)/util/%.o, $(TEST_UTIL))

# Test executables
CLANG_TEST_EXEC := $(patsubst tests/%.c, $(CLANG_TEST_BUILD)/%, $(TEST_SRC))

### DEPENDENCIES ###

-include $(DEP) $(TEST_UTIL_OBJ:.o=.d) $(CLANG_DEP) $(CLANG_TEST_UTIL_OBJ:.o=.d)

### GCC BUILD ###

# Compile source files
$(BUILD)/%.o: src/%.c
	@echo "Creating object $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Build
$(EXEC): $(OBJ)
	@echo "Building..."
	$(CC) $(CFLAGS) -o $@ $^

# Compile test utilities
$(TEST_UTIL_OBJ): $(TEST_UTIL)
	@echo "[TEST] Compiling object $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Build tests
$(TEST_BUILD)/%: tests/%.c $(LIB_OBJ) $(TEST_UTIL_OBJ)
	@echo "[TEST] Building $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) $(TEST_UTIL_OBJ) -o $@

### CLANG BUILD ###

# Compile source files
$(CLANG_BUILD)/%.o: src/%.c
	@echo "[CLANG] Creating object $<..."
	@mkdir -p $(dir $@)
	$(CLANG_CC) $(CLANG_FLAGS) -MMD -c $< -o $@

# Build
$(CLANG_EXEC): $(CLANG_OBJ)
	@echo "[CLANG] Building..."
	$(CLANG_CC) $(CLANG_FLAGS) -o $@ $^

# Compile test utilities
$(CLANG_TEST_UTIL_OBJ): $(TEST_UTIL)
	@echo "[CLANG/TEST] Compiling object $<..."
	@mkdir -p $(dir $@)
	$(CLANG_CC) $(CLANG_FLAGS) -MMD -c $< -o $@

# Build tests
$(CLANG_TEST_BUILD)/%: tests/%.c $(CLANG_LIB_OBJ) $(CLANG_TEST_UTIL_OBJ)
	@echo "[CLANG/TEST] Building $<..."
	@mkdir -p $(dir $@)
	$(CLANG_CC) $(CLANG_FLAGS) $< $(CLANG_LIB_OBJ) $(CLANG_TEST_UTIL_OBJ) -o $@

### COMMANDS ###

.PHONY: clean
clean:
	rm -rf build/

# Build and run executable
.PHONY: run
run: $(EXEC)
	./scripts/run.sh

.PHONY: tests-gcc
test-gcc: $(TEST_EXEC)
	@echo "\033[1mRunning all tests...\033[0m"
	@fail=0; \
	for test in $(TEST_EXEC); do \
 		echo ""; \
		$$test || fail=1; \
	done; \
	exit $$fail

.PHONY: run-clang
run-clang: $(CLANG_EXEC)
	./scripts/run-clang.sh

.PHONY: tests
test: $(CLANG_TEST_EXEC)
	@echo "\033[1mRunning all tests...\033[0m"
	@fail=0; \
	for test in $(CLANG_TEST_EXEC); do \
 		echo ""; \
		$$test || fail=1; \
	done; \
	exit $$fail

.PHONY: all
all: $(EXEC) $(TEST_EXEC) $(CLANG_EXEC) $(CLANG_TEST_EXEC)
