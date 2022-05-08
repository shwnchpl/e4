chip8.fth
=========

A simple example `CHIP-8 emulator`_ written in e4, using SDL2 via the e4
FFI wordlist.

.. _CHIP-8 emulator: https://en.wikipedia.org/wiki/CHIP-8

Requirements
------------

chip8.fth requires a compiled version of the e4 REPL built with FFI
support. On a system with such an e4 REPL binary installed somewhere
within the current path, chip8.fth can be invoked by simply running one
of the following commands:

.. code:: bash

   $ ./chip8.fth
   $ e4 chip8.fth

Alternatively, the ``example.chip8`` make rule builds all e4 related
chip8.fth dependencies (ie. the e4 REPL itself, with FFI support) and
the ``example.chip8.run`` make rule can be used to run chip8.fth from
the project root directory. Building the e4 REPL for chip8.fth requires
`libffi`_ and running chip8.fth requires `SDL2`_.

.. _libffi: https://sourceware.org/libffi/
.. _SDL2: https://www.libsdl.org/

Usage
-----

If no command line arguments are supplied to chip8.fth, it will start in
"interactive mode," which is simply the e4 REPL with all chip8.fth words
loaded. Otherwise, chip8.fth may optionally be passed the path to a
CHIP-8 ROM file to execute, the frequency of the tone that should be
played by the buzzer as a base 10 integer (between 20hz and 2000hz), and
the `8-bit RGB332 color codes`_ that should be used for the background
and foreground as hexadecimal integers.

For example, the following invocation loads and runs a ROM called
BRIX.ch8 with the default tone and colors:

.. code:: bash

    $ make example.chip8.run /tmp/BRIX.ch8

The following invocation loads and runs the same ROM with 440hz (`A4`_)
as the frequency of the tone, white as the background color, and black
as the foreground color:

.. code:: bash

   $ make example.chip8.run /tmp/BRIX.ch8 440 ff 00

.. _8-bit RGB332 color codes: https://en.wikipedia.org/wiki/8-bit_color
.. _A4: https://en.wikipedia.org/wiki/A440_(pitch_standard)

Additional Resources
--------------------

For a handy RGB332 color picker, see Rodger-random's
`RGB332_color_wheel_three.js`_. A Google search for "chip8 roms" turns
up a number of results, most of which should be compatible with this
emulator. For more information on the CHIP-8 architecture, one of the
most concise and accurate resources available is
`Cowgod's Chip-8 Technical Reference`_.

.. _RGB332_color_wheel_three.js: https://roger-random.github.io/RGB332_color_wheel_three.js/
.. _Cowgod's Chip-8 Technical Reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
