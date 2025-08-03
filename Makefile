CC := gcc
CFLAGS := -Wall -Wextra -O2 -Iinclude

# All sources including main.c
SRC := $(shell find src -name '*.c')
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
DEP := $(OBJ:.o=.d)
EXEC := build/app

# Library sources exclude main.c for tests
LIB_SRC := $(filter-out src/main.c, $(SRC))
LIB_OBJ := $(patsubst src/%.c, build/%.o, $(LIB_SRC))

# Test utility
TEST_UTIL := $(wildcard tests/util/*.c)
TEST_UTIL_OBJ := $(patsubst %.c, build/%.o, $(TEST_UTIL))

# Test sources and objects
TEST_SRC := $(filter-out $(TEST_UTIL), $(shell find tests -name '*.c'))
TEST_EXEC := $(patsubst tests/%.c, build/tests/%, $(TEST_SRC))

# Linking executable
$(EXEC): $(OBJ)
	@echo "Linking executable..."
	$(CC) $(CFLAGS) -o $@ $^

# Compile test utility
$(TEST_UTIL_OBJ): $(TEST_UTIL)
	@echo "Compiling shared test util $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Compile source files
build/%.o: src/%.c
	@echo "Creating object $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Compile individual tests
build/tests/%: tests/%.c $(TEST_UTIL_OBJ)
	@echo "Compiling and linking test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(TEST_UTIL_OBJ) $(if $(LIB_OBJ),$(LIB_OBJ)) -o $@

-include $(DEP) $(TEST_UTIL_OBJ:.o=.d)

.PHONY: clean
clean:
	rm -rf build/

# Build and run executable
.PHONY: run
run: $(EXEC)
	./scripts/run.sh

.PHONY: tests
tests: $(TEST_EXEC)

.PHONY: run-tests
run-tests: $(TEST_EXEC)
	@echo "Running all tests..."
	@for test in $(TEST_EXEC); do \
		echo "Running $$test"; \
		$$test || exit 1; \
	done
