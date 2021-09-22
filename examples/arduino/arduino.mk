EXAMPLES_TARGETS += example.arduino

EXAMPLE_ARDUINO_TARGET_DIR := $(BUILD_DIR)/examples/arduino/build
EXAMPLE_ARDUINO_SKETCH := $(BUILD_DIR)/examples/arduino/e4-serial
EXAMPLE_ARDUINO_TARGET := $(BUILD_DIR)/examples/arduino/e4-serial.zip

EXAMPLE_ARDUINO_SKETCH_SRC_DIR := examples/arduino/src
EXAMPLE_ARDUINO_SKETCH_SRC := $(EXAMPLE_ARDUINO_SKETCH_SRC_DIR)/e4-serial.ino

EXAMPLE_ARDUINO_FQBN ?= arduino:samd:mkrzero

.PHONY: example.arduino example.arduino.connect example.arduino.flash

example.arduino: $(EXAMPLE_ARDUINO_TARGET) ;

$(EXAMPLE_ARDUINO_TARGET): $(TARGET_AMALGAM) $(EXAMPLE_ARDUINO_SKETCH_SRC)
	@mkdir -p $(EXAMPLE_ARDUINO_TARGET_DIR)
	@mkdir -p $(EXAMPLE_ARDUINO_SKETCH)
	@cp -r $(EXAMPLE_ARDUINO_SKETCH_SRC) $(EXAMPLE_ARDUINO_SKETCH)
	@cp $(TARGET_AMALGAM) $(TARGET_AMALGAM:.c=.h) $(EXAMPLE_ARDUINO_SKETCH)
	@rm -f $(EXAMPLE_ARDUINO_TARGET)
	arduino-cli compile --fqbn $(EXAMPLE_ARDUINO_FQBN) --build-path \
		$(EXAMPLE_ARDUINO_TARGET_DIR) $(EXAMPLE_ARDUINO_SKETCH)
	arduino-cli sketch archive $(EXAMPLE_ARDUINO_SKETCH) \
		$(EXAMPLE_ARDUINO_TARGET)

ifndef SERIAL_TARGET
example.arduino.connect:
	$(error SERIAL_TARGET must be defined in order to connect)
else
example.arduino.connect:
	# XXX: KEY unfortunately does not work correctly when using
	# rlwrap in this way, but no KEY is better than no line
	# editing (especially considering e4 does not handle backspace
	# and delete characters).
	rlwrap -a picocom $(SERIAL_TARGET) --echo --imap lfcrlf
endif

ifndef SERIAL_TARGET
example.arduino.flash:
	$(error SERIAL_TARGET must be defined in order to flash)
else
example.arduino.flash: $(EXAMPLE_ARDUINO_TARGET)
	arduino-cli upload -p $(SERIAL_TARGET) --fqbn $(EXAMPLE_ARDUINO_FQBN) \
		--input-dir $(EXAMPLE_ARDUINO_TARGET_DIR)
endif
