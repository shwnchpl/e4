# Inspired by Job Vranish's excellent Makefile.
# See https://spin.atomicobject.com/2016/08/26/makefile-c-projects/.

TARGET_LIB := libe4.a
TARGET_REPL := e4

BUILD_DIR := build
INC_DIRS := core/include
CORE_SRC_DIRS := core/src
REPL_SRC_DIRS := repl/src

CORE_SRCS := $(shell find $(CORE_SRC_DIRS) -name *.c)
CORE_OBJS := $(CORE_SRCS:%=$(BUILD_DIR)/%.o)
CORE_DEPS := $(CORE_OBJS:.o=.d)

REPL_SRCS := $(shell find $(REPL_SRC_DIRS) -name *.c)
REPL_OBJS := $(REPL_SRCS:%=$(BUILD_DIR)/%.o)
REPL_DEPS := $(REPL_OBJS:.o=.d)

# TODO: Add amal.rb based amalgamated source file core target.
# TODO: Add an additional repl target to build the repl based
# on the amalgamated core target.

CFLAGS := -Wall -std=c89 -MMD -MP
REPL_LDFLAGS := -L$(BUILD_DIR) -le4

# The final build step.
$(BUILD_DIR)/$(TARGET_REPL): $(REPL_OBJS) $(BUILD_DIR)/$(TARGET_LIB)
	$(CC) $(REPL_OBJS) $(REPL_LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $(CORE_OBJS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIRS) -c $< -o $@

.PHONY: clean run-repl
run-repl: $(BUILD_DIR)/$(TARGET_REPL)
	$(BUILD_DIR)/$(TARGET_REPL)
clean:
	rm -r $(BUILD_DIR)

-include $(CORE_DEPS)
-include $(REPL_DEPS)
