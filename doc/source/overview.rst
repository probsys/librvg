Overview
========

librvg is an open-source C library for generating random variables.
It is released under the Apache-2.0 License.
The methods implemented in librvg are described in the following
publication.

   .. bibliography::
      :list: bullet
      :filter: key % "saad2025pldi"

For the latest version, please visit https://github.com/probsys/librvg.

.. attention::

   This library is research-grade software and is intended
   for research and educational purposes.

   - It may contain errors, inefficiencies, or incomplete features.
   - It has not been tested for production use or under real-world workloads.

   Users should take appropriate precautions when using this library.
   Please report questions and errors at https://github.com/probsys/librvg/issues/

Purpose
-------

The purpose of librvg is to efficiently and automatically generate exact random variates
from a given probability distribution, which is formally specified using a
numerical implementation of one or both of its
`cumulative distribution function <https://en.wikipedia.org/wiki/Cumulative_distribution_function>`_ (CDF)
or
`survival function <https://en.wikipedia.org/wiki/Survival_function>`_ (SF).

By working with a formal specification of the desired probability
distribution of the random variates, the generators are equipped with a
precise guarantee on their output distributions. This feature can be
contrasted with existing C libraries for random variate generation:

   `GNU Scientific Library <https://www.gnu.org/software/gsl/doc/html/index.html>`__ (GSL).
   This library provides CDF/SF implementations of various probability
   distributions as well as separate implementation of generators for
   these distributions. In general, there is no precise relationship
   between the CDF, the SF, and the generator―which can lead to
   unpredictable results. In addition, the true output distributions of
   generators are often difficult or intractable to quantify exactly.

   `UNU.RAN <https://statmath.wu.ac.at/unuran/>`_. This library
   generates random variables given a CDF and includes universal
   (automated) algorithms that apply to any user-specified distribution.
   However, it does not guarantee exact samples.

The exact and automated algorithms in librvg generally operate more slowly
than more specialized, approximate algorithms from the above libraries.
This library is most suitable for situations where rigorous guarantees are
needed on the exact output distribution and runtime characteristics of the
generator. It is also suitable when the entropy (or amount of randomness)
consumed by the algorithm is an important resource to be minimized; see
:ref:`overview:Optimality`.

Guarantees
----------

The algorithms in librvg cohere with the specified CDF (or SF)
interpreted as a floating-point function. In particular, given a numerical
implementation
:math:`\texttt{cdf}: \mathbb{F} \to \mathbb{F} \cap [0,1]` of a CDF that
maps a floating-point number to a float between 0 and 1, a random
variate :math:`X` is generated that satisfies

.. math::

   \begin{aligned}
   \Pr(X \le f) = \texttt{cdf}(f) && (f \in \mathbb{F}).
   \end{aligned}

If a numerical implementation
:math:`\texttt{sf}: \mathbb{F} \to \mathbb{F} \cap [0,1]` of an SF
is given instead, then the guarantee is

.. math::

   \begin{aligned}
   \Pr(X > f) = \texttt{sf}(f) && (f \in \mathbb{F}).
   \end{aligned}

An advanced feature of librvg is the notion of a *dual distribution
function* (DDF), which combines a CDF and SF to obtain a more accurate
representation of the target distribution than can be achieved using either
a CDF or SF in isolation. The DDF addresses the limitation of
floating-point probabilities, which are very dense near 0 but sparse near
1. Informally, the property that a DDF guarantees is

.. math::

   \begin{aligned}
   \Pr(X \le f) = \begin{cases}
      \texttt{cdf}(f) & \mbox{if } f \le f^* \\
      1 -_{\mathbb{R}} \texttt{sf}(f) & \mbox{if } f^* < f
      \end{cases}
      && (f \in \mathbb{F}),
   \end{aligned}

where :math:`f^*` is a cutoff-point corresponding to the exact
floating-point median of :math:`cdf`; and the subtraction in the second
case is (i.e., not subject to floating-point error). Additional details of
this representation are available in :cite:p:`saad2025pldi{§6}`.

Optimality
----------

The library includes generators that are *entropy optimal* in the sense of
:cite:t:`knuth1976`, meaning that they consume the least amount of random
bits (on average) to produce a random variate, among the class of all exact
generators for the given CDF.

Programming with librvg
-----------------------

The following program gives a brief synopsis of how to get started with the
main tasks supported by librvg. The :ref:`api:API Reference` contains
more detailed information the functions.

.. literalinclude:: ../../examples/readme.c
   :language: C
   :linenos:

The primary function provided by librvg is :func:`generate_opt`, which takes as
input a CDF implementation and a pseudorandom number generator (prng),
and returns as output an exact random variate drawn from that CDF.
