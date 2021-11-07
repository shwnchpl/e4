############################
Core: Standard Documentation
############################


The Forth 2012 standard is a flexible standard. In exchange for this
flexibility, it imposes the requirement that standards compliant systems
provide various pieces of "system documentation" related to
implementation defined options, ambiguous conditions, and other various
implementation details related to the system kernel itself and the
standard word sets implemented by the system. The Forth 2012 standard is
also a modular standard. In exchange for this modularity, it imposes the
requirement that standards compliant systems bear an official "system
label" documenting which portions of the standard are being implemented,
in part or in full. This chapter provides all such standard
documentation as it relates to the e4 kernel and word lists provided by
e4 core.

As noted in :numref:`qsg-compile-time-config`, it is possible to
include/exclude word lists from the e4 core variously at compile time.
This documentation is written from the perspective that *all* provided
word lists have been included, however where the exclusion of some word
list would change another word list's documented behavior, that is
explicitly noted. In all other cases, it can simply be assumed that if a
word list is excluded from e4 core at compile time, any documentation
associated with that word list is irrelevant.

It is also important to keep in mind that due to e4 core's embeddable
nature, not all system behavior is defined by e4 itself; rather,
behavior may depend on certain aspects of the host system into which e4
is embedded (such as word size) or the ways in which various user
provided IO hooks have been implemented. When this is the case, it is
noted explicitly in this documentation. Relatedly, in some cases, e4 may
provided default implementations for various IO hooks that a system can
use when embedding e4 core (for example, the family of functions
provided by the POSIX module documented in :numref:`apiref-posix`). When
this is the case, the behavior when using these provided hooks is also
documented.

When using this chapter, it may be helpful to cross reference the `Forth
2012 standard`_ itself, which is freely available in both HTML and PDF
formats. Because of the standard's modular nature, documentation
requirements imposed by the standard (and the usage requirements or
standard words to which they pertain) are not all located in the same
place within the standard's text. As such, this documentation contains
references to both the Forth 2012 standard sections requiring the
"system documentation" and the sections to which this documentation
pertains. These references are all prefixed with *F2012:* and formatted
in italics to make them stand out.

As an example, :numref:`stddoc-kernel` contains the standard
documentation required by *F2012: 4.1 System documentation* that pertain
to the usage requirements established in *F2012: 3 Usage requirements*.

Where appropriate, sections in this chapter are split into subsections
for documentation related to implementation defined behavior, ambiguous
conditions, and other standard documentation. When included, these
sections always appear in this order and each section is only included
if it would not be empty. Documentation appears within each of these
sections in the order that the requirement that it must appear appears
within the Forth 2012 standard.

.. _Forth 2012 standard: https://forth-standard.org/

.. toctree::
   :maxdepth: 2

   system-label.rst
   kernel.rst
   builtin/core.rst
   builtin/core-ext.rst
