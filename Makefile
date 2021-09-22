# Inspired by Job Vranish's excellent Makefile.
# See https://spin.atomicobject.com/2016/08/26/makefile-c-projects/.

BUILD_DIR := build

TARGET_AMALGAM := $(BUILD_DIR)/amalgamation/e4.c
TARGET_LIB := $(BUILD_DIR)/libe4.a
TARGET_REPL := $(BUILD_DIR)/e4
TARGET_TESTS := $(BUILD_DIR)/e4-tests

INC_DIRS := core/include
CORE_SRC_DIRS := core/src
REPL_SRC_DIRS := repl/src
TESTS_SRC_DIRS := tests/src

CORE_SRCS := $(shell find $(CORE_SRC_DIRS) -name *.c)
CORE_OBJS := $(CORE_SRCS:%=$(BUILD_DIR)/%.o)
CORE_DEPS := $(CORE_OBJS:.o=.d)

REPL_SRCS := $(shell find $(REPL_SRC_DIRS) -name *.c)
REPL_OBJS := $(REPL_SRCS:%=$(BUILD_DIR)/%.o)
REPL_DEPS := $(REPL_OBJS:.o=.d)

TESTS_SRCS := $(shell find $(TESTS_SRC_DIRS) -name *.c)
TESTS_OBJS := $(TESTS_SRCS:%=$(BUILD_DIR)/%.o)
TESTS_DEPS := $(TESTS_OBJS:.o=.d)

CFLAGS := -Wall -Wpedantic -Werror -std=c89 -Os -MMD -MP
POSIX_FLAGS := $(CFLAGS) -De4__BUILD_EVERYTHING
REPL_LDFLAGS := -ledit
TESTS_LDFLAGS := -L$(BUILD_DIR) -le4

# If a target fails, delete it.
.DELETE_ON_ERROR:

.PHONY: all amalgamation clean examples repl tests run-repl run-tests
.PHONY: longest-lines tags

# Default "all" target.
all: $(TARGET_LIB) amalgamation repl tests

# Actual build/clean targets.
amalgamation: $(TARGET_AMALGAM)
clean:
	rm -rf $(BUILD_DIR)
repl: $(TARGET_REPL)
tests: $(TARGET_TESTS)

# Examples targets.
EXAMPLES_TARGETS :=
include examples/arduino/arduino.mk
examples: $(EXAMPLES_TARGETS)

# Utility targets to run the repl and tests.
run-repl: repl
	$(TARGET_REPL)
run-tests: tests
	$(TARGET_TESTS)

# Utility target to find the longest line length in each source/header
# file.
longest-lines:
	find . -name '*.[ch]' -o -name '*.in[co]' -o -name '*.mk' \
		-o -name 'Makefile' | xargs wc -L

# Utility target to generate tags. Requires the clean target, to ensure
# that tags do not erroneously point into the amalgamation.
tags: clean
	ctags -R .

include tests/build-test/build-test.mk

# Actual targets.
## XXX: Require objects to build separately before generating
# an amalgamated header. It should always be possible to *not*
# use the amalgamation.
$(TARGET_AMALGAM): $(CORE_OBJS)
	mkdir -p $(dir $(TARGET_AMALGAM))
	amal.rb --headers $(INC_DIRS) --src $(CORE_SRC_DIRS) \
		> $(TARGET_AMALGAM)
	find $(INC_DIRS) -name '*.h' -exec cp {} $(dir $(TARGET_AMALGAM)) \;

$(TARGET_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $(CORE_OBJS)

$(TARGET_REPL): $(REPL_OBJS) $(TARGET_AMALGAM)
	$(CC) $(POSIX_FLAGS) $(TARGET_AMALGAM) $(REPL_OBJS) $(REPL_LDFLAGS) \
		-o $@

$(TARGET_TESTS): $(TESTS_OBJS) $(TARGET_LIB)
	$(CC) $(TESTS_OBJS) $(TESTS_LDFLAGS) -o $@

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(POSIX_FLAGS) -I$(INC_DIRS) -c $< -o $@

-include $(CORE_DEPS)
-include $(REPL_DEPS)
-include $(TESTS_DEPS)
