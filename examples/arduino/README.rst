Example Arduino Application
===========================

A very simple Arduino application for the MKR Zero that provides e4
handlers for serial input and output as well as a few builtin LED
related words before entering interactive mode. After initialization, e4
can be used interactively via serial connection.

Building
--------

This example application can be built independently by running ``make
example.arduino`` or with all other examples by running ``make
examples``. Building requires both `Arduino CLI`_ and an installed
``arduino:samd`` core. With Arduino CLI installed, the appropriate core
can be installed by running the following commands:

.. code:: bash

    $ arduino-cli core update-index
    $ arduino-cli core install arduino:samd

If you should happen to desire to redistribute the generated sketch, it
is made available as the archive ``e4-serial.zip`` in the
``build/examples/arduino`` directory.

The makefile in this directory also provides the
``example.arduino.flash`` and ``example.arduino.connect`` rules for
flashing a built sketch and connecting respectively. These rules both
require ``SERIAL_TARGET`` to be set to the path to the serial device
that should be flashed/accessed. For example:

.. code:: bash

    $ make example.arduino.flash SERIAL_TARGET=/dev/ttyACM0

The following command will list all all serial paths associated with
Arduino devices connected over USB:

.. code:: bash

    $ arduino-cli board list

The ``example.arduino.connect`` rule makes use of both rlwrap and
picocom and as such requires both of these programs to be available.

Finally, it is possible to attempt to build for a board other than the
MKR Zero by manually setting EXAMPLE_ARDUINO_FQBN to the fully qualified
board name (which can be obtained with either the ``board list`` or
``board listall`` Arduino CLI commands) of the board you would like to
target. For example, to target a Arduino Mega run the following command
with the ``arduino:avr`` installed:

.. code:: bash

    $ make EXAMPLE_ARDUINO_FQBN=arduino:avr:mega

This is not, however, guaranteed to actually succeed for every possibly
Arduino board. Some Arduino boards, such as the Arduino Nano, simply do
not have enough SRAM to fit the generated sketch, as the AVR
architecture does not have a unified memory space allowing constants to
be stored in flash automatically. Furthermore, it is critical to include
the explicit ``EXAMPLE_ARDUINO_FQBN`` definition when invoking rules for
flashing and connecting as well, otherwise those rules may not work
correctly.

.. _Arduino CLI: https://github.com/arduino/arduino-cli

Example Usage
-------------

From the interactive environment, the following Forth words can be
defined to interact with the builtin LED.

.. code:: forth

    : led-init ( -- )
        led-mode @ led-pin or
        led-mode !
        ;

    : led-set ( f -- )
        led-out @ led-pin rot
        if      \ turn LED on
            or
        else    \ turn LED off
            invert and
        then
        led-out !
        ;

    : led-blink ( n -- )
        2* 1+ 0 do
            i 2 mod led-set
            500 ms
        loop
        ;

After executing ``led-init``, ``led-set`` can be used to toggle the
builtin LED and ``led-blink`` can be used to make the LED blink ``n``
times.
