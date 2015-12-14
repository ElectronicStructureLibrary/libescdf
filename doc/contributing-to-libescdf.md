Contributing to Libescdf
========================

Libescdf follows the GNU Coding Standards as much as permitted by the
experience of its developers. If you are planning to contribute, we thus
recommend you to consult the official [GNU Coding Standards
homepage](https://www.gnu.org/prep/standards/) before doing so.


Programming languages
---------------------

The core of Libescdf is written in C. All other languages must be supported
through bindings.

Since we use the Autotools to build, install, and distribute the code, all C,
C++, Python, or Fortran files, must be stored in language-specific
subdirectories. Mixes should not happen in the same directory, and even less
in the same library.


Optional features
-----------------

Optional features are triggered through *HAVE_FEATURE* preprocessing options,
where *FEATURE* is the name of the feature. All these preprocessing options
are handled through *config.h*, with the exception of the Autotools-reserved
*HAVE_CONFIG_H* option.

Within the source code, access to the optional features is granted in a
boolean way, i.e.:

    #if defined HAVE_FEATURE
    ... do something optional ...
    #endif

This allows keeping the build system as simple and maintainable as possible.


C header files
--------------

In order to keep the build process smooth and simple, we have adopted the
following conventions for the inclusion of C headers:

  * system headers should come first, then external dependency headers, then
    internal exported headers, then internal non-exported headers, then
    *config.h*, and finally non-exported headers depending on *config.h*
    and/or conditional inclusions;
  * all headers coming from outside Libescdf must be included using
    "less than" and "greater than" as delimiters, e.g. "<stdlib.h>";
  * internal headers must be included using double quotes as delimiters;
  * the inclusion of *config.h* must be controlled by the *HAVE_CONFIG_H*
    preprocessing option;
  * exported headers must not depend on *config.h* nor on non-exported
    headers;
  * all exported header names (apart escdf.h) must be prefixed with
    *"escdf_"*;
  * headers depending on *config.h* should be minimised;
  * *config.h* must be readable from Fortran, i.e. must not contain complex
    CPP macros.

Here is an example where the source code depends on whether MPI is available:

    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <string.h>

    #include "escdf.h"

    #if defined HAVE_CONFIG_H
    #include "config.h"
    #endif

    #ifdef HAVE_MPI
    #include <mpi.h>
    #endif

Since *escdf.h* is exported by the library, it is included before *config.h*,
in order to make sure that its contents do not depend on any CPP option.


Keeping the build system updated
--------------------------------

When changing the contents of a file, the build system does not need to be
updated. However, when adding, removing, or renaming files, you have to inform
the build system of what happened. The corresponding procedures are described
in [Hacking the build system](hacking-the-build-system.html).


Testing
-------

When adding new functions, it is extremely important to provide simple test
programs, aka "unit tests", to check whether these functions are performing as
they should. If you do not feel comfortable with integrating these tests with
the build system, please notify the other developers.

Systematically writing unit tests is not only essential to maintain the
overall quality of the code. It will greatly help efficiently design and
structure your contributions, since you will always have a concrete use
example at hand to feed your thoughts and help you improve the design of your
code.

