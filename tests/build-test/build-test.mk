.PHONY: build-test

BUILD_TEST_SRCS := tests/build-test/main.c
BUILD_TEST_DEPS := $(BUILD_TEST_SRCS:%=$(BUILD_DIR)/%.d)

# XXX: While not every build-test configuration requires libffi, the
# point of the build-test is to test every configuration, so it is
# reasonable enough to simply require libffi outright in order to run
# the build tests. As such, any configuration that *would* require
# libffi in order to build *will* fail without it.
#BUILD_TEST_LDFLAGS := -lffi

define build-test
$(eval $(1)_BUILD_DIR := $(BUILD_DIR)/build-test/$(1)-test)
$(eval $(1)_OBJS := $(CORE_SRCS:%=$($(1)_BUILD_DIR)/%.o))
$(eval $(1)_TARGET := $($(1)_BUILD_DIR)/$(1)-test)

$(eval BUILD_TEST_TARGETS += $($(1)_TARGET))
$(eval BUILD_TEST_DEPS += $($(1)_OBJS:.o=.d))

$($(1)_TARGET): $($(1)_OBJS) $(BUILD_TEST_SRCS) $(TARGET_AMALGAM)
	@$(CC) $(CFLAGS) $(3) $(4) -I$(INC_DIRS) \
		$(TARGET_AMALGAM) $(BUILD_TEST_SRCS) -o $$@
	@$($1_TARGET) $(2)

$($(1)_BUILD_DIR)/%.c.o: %.c
	@mkdir -p $$(dir $$@)
	@$(CC) $(CFLAGS) $(4) -I$(INC_DIRS) -c $$< -o $$@
endef

# Test the three main build modes.
$(eval $(call build-test,BUILD_EVERYTHING,0xe0607a1,\
	-lffi,-De4__BUILD_EVERYTHING))
$(eval $(call build-test,BUILD_DEFAULT,0x607a1,,-De4__BUILD_DEFAULT))
$(eval $(call build-test,BUILD_MINIMAL,0,,-De4__BUILD_MINIMAL))

# Test excluding any single wordlist from an everything build.
$(eval $(call build-test,BUILD_EVERYTHING_NO_CORE_EXT,0xe0607a0,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_CORE_EXT))
$(eval $(call build-test,BUILD_EVERYTHING_NO_DLFCN,0xa0607a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_DLFCN))
$(eval $(call build-test,BUILD_EVERYTHING_NO_EXCEPTION,0xe060781,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_EXCEPTION))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FACILITY,0xe060721,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_FACILITY))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FACILITY_EXT,0xe0606a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_FACILITY_EXT))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FILE,0xe0605a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_FILE))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FILE_EXT,0xe0603a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_FILE_EXT))
$(eval $(call build-test,BUILD_EVERYTHING_NO_FFI,0x60607a1,,\
	-De4__BUILD_EVERYTHING -De4__EXCLUDE_FFI))
$(eval $(call build-test,BUILD_EVERYTHING_NO_TOOLS,0xe0407a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS))
$(eval $(call build-test,BUILD_EVERYTHING_NO_TOOLS_EXT,0xe0207a1,\
	-lffi,-De4__BUILD_EVERYTHING -De4__EXCLUDE_TOOLS_EXT))

# Test excluding any single wordlist from a default build.
$(eval $(call build-test,BUILD_DEFAULT_NO_CORE_EXT,0x607a0,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_CORE_EXT))
$(eval $(call build-test,BUILD_DEFAULT_NO_EXCEPTION,0x60781,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_EXCEPTION))
$(eval $(call build-test,BUILD_DEFAULT_NO_FACILITY,0x60721,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FACILITY))
$(eval $(call build-test,BUILD_DEFAULT_NO_FACILITY_EXT,0x606a1,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FACILITY_EXT))
$(eval $(call build-test,BUILD_DEFAULT_NO_FILE,0x605a1,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FILE))
$(eval $(call build-test,BUILD_DEFAULT_NO_FILE_EXT,0x603a1,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_FILE_EXT))
$(eval $(call build-test,BUILD_DEFAULT_NO_TOOLS,0x407a1,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_TOOLS))
$(eval $(call build-test,BUILD_DEFAULT_NO_TOOLS_EXT,0x207a1,,\
	-De4__BUILD_DEFAULT -De4__EXCLUDE_TOOLS_EXT))

build-test: $(BUILD_TEST_TARGETS)

-include $(BUILD_TEST_DEPS)
