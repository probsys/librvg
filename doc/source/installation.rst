Installation
============

This software is tested on Ubuntu 24.04. Other Linux distributions are also
expected to work with minor modifications to the names of the dependencies.
An alternative approach to installation is by using Docker, as described in
the section :ref:`installation:Docker Installation`.

Native Installation
-------------------

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

Docker Installation
-------------------

Installing librvg and running the experiments can also be executed in a
virtual Docker container, by using the provided
:file:`Dockerfile`.

To build the container, first install Docker for your platform using
the instructions at https://docs.docker.com/get-started/get-docker/,
then run the following steps.

* **Step 1**. Build the image and container

  .. code-block:: bash

    $ docker build -f Dockerfile -t librvg .        # build image (~20 mins)
    $ docker run -dit --name librvg librvg:latest   # create container

* **Step 2**. Extract the experiment results to the local filesytem.

  .. code-block:: bash

    $ docker cp librvg:/librvg librvg-build         # extract to ./librvg-build

* **Step 3**. (Optional) Launch an interactive terminal in the Docker container.

  .. code-block:: bash

    $ docker start librvg
    $ docker attach librvg
