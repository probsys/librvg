Installation
============

This software is tested on Ubuntu 24.04. Other Linux distributions are also
expected to work with minor modifications to the names of the dependencies.

* **Step 1**: Install C dependencies

  .. code-block:: bash

    $ sudo apt install build-essential libgsl-dev libgmp-dev

* **Step 2**: Build the library

  .. code-block:: bash

      $ make

  The files are installed in

   - static library: ``build/lib/librvg.a``
   - header files: ``build/include/*.h``

* **Step 3**: Run the examples

  .. code-block:: bash

    $ cd examples/
    $ make
    $ ./main.out
    $ ./readme.out

  The following Makefile shows how to build new programs
  that use ``librvg.a``.

  .. literalinclude:: ../../examples/Makefile
    :language: Makefile
