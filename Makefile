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

# TODO: Add amal.rb based amalgamated source file core target.
# TODO: Add an additional repl target to build the repl based
# on the amalgamated core target.

CFLAGS := -Wall -std=c89 -Os -MMD -MP
REPL_LDFLAGS := -ledit
TESTS_LDFLAGS := -L$(BUILD_DIR) -le4

# Default "all" target.
.PHONY: all amalgamation clean repl tests run-repl run-tests
all: $(TARGET_LIB) amalgamation repl tests
amalgamation: $(TARGET_AMALGAM)
clean:
	rm -rf $(BUILD_DIR)
repl: $(TARGET_REPL)
tests: $(TARGET_TESTS)
run-repl: repl
	$(TARGET_REPL)
run-tests: tests
	$(TARGET_TESTS)

# Actual targets.
# XXX: Require objects to build separately before generating
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
	$(CC) $(TARGET_AMALGAM) $(REPL_OBJS) $(REPL_LDFLAGS) -o $@

$(TARGET_TESTS): $(TESTS_OBJS) $(TARGET_LIB)
	$(CC) $(TESTS_OBJS) $(TESTS_LDFLAGS) -o $@

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIRS) -c $< -o $@

-include $(CORE_DEPS)
-include $(REPL_DEPS)
-include $(TESTS_DEPS)
