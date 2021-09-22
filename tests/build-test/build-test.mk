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
$(eval $(call build-test,BUILD_EVERYTHING,0x20601a1,-De4__BUILD_EVERYTHING))
$(eval $(call build-test,BUILD_DEFAULT,0x601a1,-De4__BUILD_DEFAULT))
$(eval $(call build-test,BUILD_MINIMAL,0,-De4__BUILD_MINIMAL))

# Test excluding any single wordlist from an everything build.
$(eval $(call build-test,BUILD_EVERYTHING_NO_CORE_EXT,0x20601a0,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_CORE_EXT))
$(eval $(call build-test,BUILD_EVERYTHING_NO_EXCEPTION,0x2060181,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_EXCEPTION))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FACILITY,0x2060121,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_FACILITY))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FACILITY_EXT,0x20600a1,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_FACILITY_EXT))
$(eval $(call build-test,BUILD_EVERYTHING_NO_TOOLS,0x20401a1,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS))
$(eval $(call build-test,BUILD_EVERYTHING_NO_TOOLS_EXT,0x20201a1,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS_EXT))

# Test excluding any single wordlist from a default build.
$(eval $(call build-test,BUILD_DEFAULT_NO_CORE_EXT,0x601a0,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_CORE_EXT))
$(eval $(call build-test,BUILD_DEFAULT_NO_EXCEPTION,0x60181,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_EXCEPTION))
$(eval $(call build-test,BUILD_DEFAULT_NO_FACILITY,0x60121,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FACILITY))
$(eval $(call build-test,BUILD_DEFAULT_NO_FACILITY_EXT,0x600a1,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FACILITY_EXT))
$(eval $(call build-test,BUILD_DEFAULT_NO_TOOLS,0x401a1,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_TOOLS))
$(eval $(call build-test,BUILD_DEFAULT_NO_TOOLS_EXT,0x201a1,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_TOOLS_EXT))

build-test: $(BUILD_TEST_TARGETS)

-include $(BUILD_TEST_DEPS)
