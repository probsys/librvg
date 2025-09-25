Experiments
===========

The :file:`experiments` directory contains programs that produce Table 1,
Table 2, and Figure 8 of :cite:`saad2025pldi`. Running the plotting code
requires ``python3`` to be available in the path, with the pandas, numpy,
and matplotlib libraries installed. Each experiment takes around 5-10
minutes.

Running Natively
----------------

Table 1
    .. code-block:: bash

        $ cd experiments/
        $ make table1

    The output is ``experiments/results.rate/table_1.txt``

    These results shows the bits/variate, variates/sec for random
    variate generation using three baselines (GSL, CBS, OPT)
    on 24 probability distributions.

Table 2
    .. code-block:: bash

        $ cd experiments/
        $ make table2

    The outputs are the .png files in ``experiments/results.bounds``

    These results show the output random of random variates using four
    generation methods (GSL, CDF, SF, DDF) for 13 probability distributions.
    Each row in Table 2 corresponds to a specific png file. In each png file,
    the runtimes are shown under the labels on the y-axis and the output
    ranges are shown on the x-axis.

Figure 8
    .. code-block:: bash

        $ cd experiments/
        $ make figure8

    The output is ``experiments/results.rate/figure_8.png``

    These results show the ratios of bits/variate and variate/sec using
    the basic and extended-accuracy variants of two methods (CBS, OPT).

Running via Docker
------------------

Installing librvg and running the experiments can also be executed in a
virtual Docker container.

To build the container, first install Docker for your platform using
the instructions at https://docs.docker.com/get-started/get-docker/.

1. Build the image and container
    .. code-block:: bash

        $ docker build -f Dockerfile -t librvg .        # build image (~20 mins)
        $ docker run -dit --name librvg librvg:latest   # create container

2. Extract the experiment results to the local filesytem.
    .. code-block:: bash

        $ docker cp librvg:/librvg librvg-build         # extract to ./librvg-build

3. (Optional) Launch an interactive terminal in the Docker container.
    .. code-block:: bash

        $ docker start librvg
        $ docker attach librvg

