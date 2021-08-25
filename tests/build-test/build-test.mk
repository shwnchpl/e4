.PHONY: build-test

BUILD_TEST_SRCS := tests/build-test/main.c
BUILD_TEST_DEPS := $(BUILD_TEST_SRCS:%=$(BUILD_DIR)/%.d)

define build-test
$(eval $(1)_BUILD_DIR := $(BUILD_DIR)/build-test/$(1)-test)
$(eval $(1)_OBJS := $(CORE_SRCS:%=$($(1)_BUILD_DIR)/%.o))
$(eval $(1)_TARGET := $($(1)_BUILD_DIR)/$(1)-test)

$(eval BUILD_TEST_TARGETS += $($(1)_TARGET))
$(eval BUILD_TEST_DEPS += $($(1)_OBJS:.o=.d))

$($(1)_TARGET): $($(1)_OBJS) $(BUILD_TEST_SRCS) $(TARGET_AMALGAM)
	@$(CC) $(CFLAGS) $(3) -I$(INC_DIRS) $(TARGET_AMALGAM) \
		$(BUILD_TEST_SRCS) -o $$@
	@$($1_TARGET) $(2)

$($(1)_BUILD_DIR)/%.c.o: %.c
	@mkdir -p $$(dir $$@)
	@$(CC) $(CFLAGS) $(3) -I$(INC_DIRS) -c $$< -o $$@
endef

# Test the three main build modes.
$(eval $(call build-test,BUILD_EVERYTHING,0x60021,-De4__BUILD_EVERYTHING))
$(eval $(call build-test,BUILD_DEFAULT,0x60021,-De4__BUILD_DEFAULT))
$(eval $(call build-test,BUILD_MINIMAL,0,-De4__BUILD_MINIMAL))

# Test excluding any single word set.
$(eval $(call build-test,BUILD_NO_CORE_EXT,0x60020,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_CORE_EXT))
$(eval $(call build-test,BUILD_NO_EXCEPTION,0x60001,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_EXCEPTION))
$(eval $(call build-test,BUILD_NO_TOOLS,0x40021,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS))
$(eval $(call build-test,BUILD_NO_TOOLS_EXT,0x20021,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS_EXT))

build-test: $(BUILD_TEST_TARGETS)

-include $(BUILD_TEST_DEPS)
