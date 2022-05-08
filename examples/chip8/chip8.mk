EXAMPLES_TARGETS += example.chip8

EXAMPLE_CHIP8_SRC_DIR := examples/chip8/src
EXAMPLE_CHIP8_SRC := $(EXAMPLE_CHIP8_SRC_DIR)/chip8.fth

.PHONY: example.chip8 example.chip8.run

example.chip8: $(TARGET_REPL)

# XXX: Hack to allow passing arguments to the example.chip8.run target.
ifeq (example.chip8.run, $(firstword $(MAKECMDGOALS)))
EXAMPLE_CHIP8_RUN_ARGS := \
	$(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
$(eval $(EXAMPLE_CHIP8_RUN_ARGS):;@true)
endif

example.chip8.run: example.chip8
	$(TARGET_REPL) $(EXAMPLE_CHIP8_SRC) $(EXAMPLE_CHIP8_RUN_ARGS)
