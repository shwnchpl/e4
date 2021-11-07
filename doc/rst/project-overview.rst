################
Project Overview
################


**********
Background
**********

In 2021, at the time of this documentation's initial composition, Forth
is not a terribly popular programming language. Many pieces of user
facing software that would have once been systems applications are now
webpages (possibly running in containers) and a career programmer (or
"software engineer") could spend easily spend twenty years in the
industry without ever thinking about dynamically allocating memory, let
alone performing pointer arithmetic. Nevertheless, this is all possible
because of "low level" software systems which *do* afford the programmer
with such control, and many of the languages that fall into that
category (such as C, C++, Rust, and Zig) are still very exciting to the
people with an interest in developing (or a need to develop) such
systems.

Also popular in 2021 are programming (or perhaps it would be more
appropriate to say "scripting") languages that provide the programmer
with an interactive environment, a so called REPL (Read-Evaluate-Print
Loop). Languages like Javascript, Python, and Ruby boast rapid
interactive development and are handy enough to have taken the place of
the graphing calculator for many scientists and engineers.

Interestingly, however, as of 2021, there are preciously few such
languages that provide an interactive environment which also provide low
level facilities (such as manual memory management or the ability to
store and dereference at arbitrary addresses), at least, not in ways
that are terribly useful. Forth, of course, has always had this
capability, and is relatively simple to implement and work with, and yet
there is little contemporary interest in it, especially when compared to
a language like Common LISP, which is still often both cloned and
utilized by programmers everywhere.

The author of this documentation does not claim to know why this might
be. It is, however, worth noting that many existing Forth
implementations fall into one of two categories. The first category of
Forth implementations is extraordinarily minimal. These implementations
are suitable for execution on extremely resource-restricted
microcontrollers (which any embedded developer will tell you are more
used now then perhaps ever before). Many of these implementations are
written in some assembly language and are only suitable for execution on
a very limited number of pieces of hardware. It's no surprise, then,
that many developers have never encountered such a Forth outside the
context of some article (no matter how technically amazing such Forths
might be).

The second category of Forth is one that seeks to be a portable (and
possibly also fully featured) programming language. The most well known
free-software Forth in this category is without a doubt `gforth`_, the
GNU project's Forth implementation. Forths like gforth are great pieces
of software to be sure, but one thing that they decidedly do *not* do is
run on resource limited microcontrollers. To the contrary, gforth
expects and requires a fully featured general purpose host operating
system capable of arbitrary dynamic memory allocation and various POSIX
functionality. For the purposes of executing arbitrary applications
programs written in Forth, this is fine (better than fine, really--it's
excellent and gforth is an excellent piece of software) but it is a
completely separate use case from Forths in the first category, and as
such the portability of a Forth like gforth is less exciting than it
otherwise might be.

Another popular language (or is it a library?) in 2021 is `SQLite`_.
SQLite boldly defies the convention of providing relational database
services (including their associated access via SQL) using a
client-server model and instead provides these services as part of an
embeddable ANSI C library that can easily be run almost anywhere that
ANSI C can be run, without compromising on functionality when compared
to other relational database solutions, at least as much as is possible.
SQLite takes this to the extreme by providing its library in a single C
source file that can easily be integrated into any project, or wrapped
and accessed in any language capable of interfacing with native code
compiled from C, which includes most serious general purpose language.

To date, no one has really done this with a Forth, despite the fact that
this is an area where Forth would almost certainly be useful. That is to
say, what if there were a truly portable Forth (implemented in a
portable language like C) that could run on systems with extremely
limited resources (possibly only a few kilobytes of RAM) provided as a
convenient C library (maybe even amalgamated into a single C source file,
a la SQLite)?

e4 is an answer to that question. Specifically, e4 is an embeddable
Forth that makes no use of dynamic memory whatsoever, that is capable of
running on extremely resource constrained embedded systems, and that is
implemented as portably as possible in ANSI C and distributed as a
single ANSI C source file. What's more, e4 is Forth 2012 standards
compliant system (see the full specification) providing several standard
wordlists (either in their entirety or in part) in such a way that they
are configurable at compile time, so you only pay for what you intend to
use (if your memory/storage budget is strict enough that such a thing is
important).

e4 has been tested and runs correctly on several different
architectures. One of the most compelling use cases for e4 is
interactively exploring/debugging resource heavily constrained systems.
e4 can be easily integrated into a firmware image written in C and used
to call wrapped C functions, directly inspect memory, and tinker with
registers/hardware as necessary.

In addition to the core e4 library (referred to as e4 core), example
embedded implementations as well as an e4 REPL designed to run on POSIX
operating systems are provided as a part of the e4 project. Since they
are considered a part of the e4 project, those examples, as well as the
e4 REPL, are all documented here along with e4 core.

While the main goal of the e4 project is to make it easier to program
interactively on resource and/or access constrained systems, a secondary
goal is to write an useful general purpose Forth. As such, every effort
has been taken in e4 to compromise on as little as possible and to pave
the way for a third category of Forth that takes the best features from
both the minimal Forths and the portable Forths and turns them into a
portable minimal Forth. In the spirit of Forth, if e4 can't do something
(within reason), it's only because it hasn't been extended to do it yet.

TODO: Add link to full specification in core.

TODO: Add link to full system requirements and REPL requirements.

.. _gforth: https://www.gnu.org/software/gforth/
.. _sqlite: https://www.sqlite.org/index.html


************
Code License
************

TODO: Add license information.


*****************
Versioning Scheme
*****************

TODO: Add info about project versioning scheme.


**********************
Quirks and Limitations
**********************

Address Space Limitation
========================

TODO: Add content.

Difficulties of Floating Point Implementation
=============================================

TODO: Add content.
