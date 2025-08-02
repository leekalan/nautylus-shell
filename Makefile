CC := gcc
CFLAGS := -Wall -Wextra -O2 -Iinclude

# All sources including main.c
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
DEP := $(OBJ:.o=.d)
EXEC := build/app

# Library sources exclude main.c for tests
LIB_SRC := $(filter-out src/main.c, $(SRC))
LIB_OBJ := $(patsubst src/%.c, build/%.o, $(LIB_SRC))

# Test sources and objects
TEST_SRC := $(wildcard tests/*.c)
TEST_EXEC := $(patsubst tests/%.c, build/tests/%, $(TEST_SRC))

# Linking executable
$(EXEC): $(OBJ)
	@echo "Linking executable..."
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files
build/%.o: src/%.c
	@echo "Creating objects $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Compile individual tests
build/tests/%: tests/%.c
	@echo "Compiling and linking test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(if $(LIB_OBJ),$(LIB_OBJ)) -o $@

-include $(DEP)

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
