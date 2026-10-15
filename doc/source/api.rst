API Reference
=============

The functions documented in this reference are in
:src:`generate.h`, unless otherwise stated.

It will be helpful to review the main :ref:`example:Example` for an
overview of the interface.

Defining A Target Distribution
------------------------------

To generate random variates, librvg requires a specification of the target
as a cumulative distribution function (CDF), a survival function (SF), or
both. Functions that compute a CDF or SF must adhere to the following type.

.. type:: float (*cdf32_t)(double x);

    A function type that takes as input a double `x` and returns a float.

    Use for a Cumulative Distribution Function
      When used as a CDF, the function returns a float representing
      :math:`\Pr(X \le x)` for the generated random variate :math:`X`.
      To be a valid specification, F
      must satisfy the following properties:

      1. For all floats x, F(x) is a float in the unit interval [0, 1].
      2. F(x) = 1, whenever x is NaN (i.e., if x != x).
      3. F is nondecreasing, i.e., if x' < x then F(x') <= F(x).

    Use for a Survival Function
      When used as a SF, the function returns a float representing
      :math:`\Pr(X > x)` for the generated random variate :math:`X`.
      To be a valid specification, S
      must satisfy the following properties:

      1. For all floats x, S(x) is a float in the unit interval [0, 1].
      2. S(x) = 0, whenever x is NaN (i.e., if x != x).
      3. S is nonincreasing, i.e., if x' < x then S(x) <= S(x').

    .. note::

      According to the above specification, probability distributions are
      permitted to include atoms at special floating-point values
      ``-INFINITY``, ``INFINITY``, and ``NAN``. Using a CDF F, this behavior
      can be achieved as follows.

        - Atom at ``-INFINITY``: let ``F(-INFINITY) > 0``.
        - Atom at ``INFINITY``: let ``F(nextafterf(INFINITY, 0)) < F(INFINITY)``.
        - Atom at ``NAN``: let ``F(INFINITY) < 1``.

      It is also possible to specify separate atoms at the negative
      and positive zeros (``0+``, ``0-``) of floating-point.

Accurate Sampling in the Right Tail
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An advanced feature of librvg is the notion of a "dual distribution
function" (DDF), which enables extended accuracy sampling in the upper tail
of a distribution.

Recall that floating-point probabilities are dense near 0 but sparse
near 1. For example, a 32-bit CDF with ``F(x) < 1`` has ``F(x) <= 1 - 2^-24``, so it cannot
represent an upper-tail probability ``Pr(X > x)`` that lies strictly between 0
and ``2^-24``. A DDF solves this limitation. It can be built in two ways, and
both give full floating-point precision in the upper tail:

- From a CDF and an SF, using :func:`MAKE_DDF`. The result is also precise in
  the lower tail, as the CDF is used up to its median and the SF above it.

- From an SF alone, using :func:`MAKE_DDF_SF`. The lower half of the
  distribution is then only as precise as ``1 - S(x)``, i.e., probabilities
  at most 1/2 are multiples of ``2^-24``.

A CDF alone is already precise in the lower tail, and can be used directly
with the generators that do not have an ``_ext`` suffix. This
representation is further described in the :ref:`overview:Guarantees` and
in :cite:p:`saad2025pldi{§6}`.

The type of a DDF is given in as follows.

.. type:: void (*ddf32_t)(double x, bool * b, float * p);

    This function takes as input a double :data:`x`, and writes the
    result of evaluating a DDF at :data:`x` to the pointers
    :data:`b` and :data:`p`.
    The interpretation of :data:`b` and :data:`p` are as follows:

    - If :data:`b` is set to 0, then :math:`\Pr(X \le x) = p`

    - If :data:`b` is set to 1, then :math:`\Pr(X \le x) = 1 - p`

    As with a CDF, a DDF must satisfy :math:`\Pr(X \le x) = 1` whenever
    :data:`x` is NaN (i.e., if ``x != x``). That is, it must set :data:`b`
    to 1 and :data:`p` to 0. A DDF made using :func:`MAKE_DDF` from a CDF
    and SF, or using :func:`MAKE_DDF_SF` from an SF, that satisfy the above
    properties has this behavior automatically.

Functions that operate using a DDF have an ``_ext`` suffix (meaning "extended").

Using GSL Distributions
^^^^^^^^^^^^^^^^^^^^^^^

librvg can interporate with existing CDF or SF implementations from the
`GNU Scientific Library <https://www.gnu.org/software/gsl/doc/html/randist.html>`_.
The following examples show how to wrap a Normal(0,1) and Poisson(5)
distributions to create new functions that are compatible with
with :type:`cdf32_t` and :type:`ddf32_t`.

.. code-block:: c

    // Gaussian distribution from the GNU library (continuous).
    MAKE_CDF_P(gaussian_cdf, gsl_cdf_gaussian_P, 1);
    MAKE_CDF_Q(gaussian_sf, gsl_cdf_gaussian_Q, 1);
    MAKE_DDF(gaussian_ddf, gaussian_cdf, gaussian_sf);

    // EXAMPLE 2: Poisson distribution from the GNU library (discrete).
    MAKE_CDF_UINT_P(poisson_cdf, gsl_cdf_poisson_P, 5);
    MAKE_CDF_UINT_Q(poisson_sf, gsl_cdf_poisson_Q, 5);
    MAKE_DDF(poisson_ddf, poisson_cdf, poisson_sf);


The following macros are available, where
:data:`name` should be a fresh C identifier (not a string)
for the name of the new function created from :data:`func`. The suffixes
``_P`` and ``_Q`` correspond to a CDF and SF, respectively, a naming
convention inherited from the GSL. The ``...`` varargs are passed
directly to :data:`func`. The :func:`MAKE_DDF` macro is not specific to a
CDF or SF created from the GSL, it can be used for any such pairing. The
:func:`MAKE_DDF_SF` macro creates a DDF from an SF alone, for a target that
is specified only by its survival function.

.. doxygendefine:: MAKE_CDF_P
.. doxygendefine:: MAKE_CDF_Q
.. doxygendefine:: MAKE_CDF_UINT_P
.. doxygendefine:: MAKE_CDF_UINT_Q
.. doxygendefine:: MAKE_DDF
.. doxygendefine:: MAKE_DDF_SF

.. function:: float cdf_discrete(double x, const float *P, size_t K);

    Available in :src:`discrete.h`.

    A cumulative distribution function (:type:`cdf32_t`) for arbitrary
    discrete distributions over nonnegative integers. It is parameterized
    by a length-:data:`K` input array :data:`P` of cumulative
    probabilities, where ``P[i]`` is the cumulative probability of
    integer ``i``.

Generating Random Variates
--------------------------

Entropy-Optimal Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^

The generators are the entropy-optimal algorithms from :cite:`saad2025pldi`.
They take as input the target :data:`cdf` as described in the
:ref:`previous section <api:Defining a Target Distribution>` as well as a
:data:`prng`, which is described in :ref:`api:Pseudorandom Number Generators`.

.. doxygenfunction:: generate_opt
.. doxygenfunction:: generate_opt_ext

Conditional-Bit Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^

librvg also includes implementations of the Conditional Bit Sampling (CBS)
method. These algorithms are described in :cite:t:`Sobolewski1972{Section II}`.
The performance of these functions is generally inferior to that of
:func:`generate_opt` and :func:`generate_opt_ext` in terms
of entropy consumption and runtime, as they use arbitrary precision
arithmetic via the `GNU MP Bignum Library <https://gmplib.org/>`_ (libgmp).

.. doxygenfunction:: generate_cbs
.. doxygenfunction:: generate_cbs_ext

Inverse-Transform Method
^^^^^^^^^^^^^^^^^^^^^^^^

librvg also includes exact generators based on the
`inversion method <https://en.wikipedia.org/wiki/Inverse_transform_sampling>`_.
These functions draw a uniform random number in the unit interval, rounded
to a float :data:`q`, and return the exact :data:`q`-quantile of the
target, as computed by :func:`quantile` and :func:`quantile_ext` (see
:ref:`api:Querying a CDF`). The :func:`generate_inversion_ext` function draws
:data:`q` in a dual representation, to retain precision near 1.

.. doxygenfunction:: generate_inversion
.. doxygenfunction:: generate_inversion_ext

Querying a CDF
--------------


.. function:: double quantile(cdf32_t cdf, float q)
              double quantile_ext(ddf32_t ddf, bool d, float q)

  These functions are used to compute the exact quantiles of a CDF
  or DDF. Recall that the
  `quantile <https://en.wikipedia.org/wiki/Quantile>`_ :math:`Q`
  of a cumulative distribution function :math:`F` is its generalized
  inverse:

  .. math::

     \begin{aligned}
     Q(u) = \inf\{ x \in \mathbb{R} \mid u \le F(x) \} && (u \in [0,1]).
     \end{aligned}

  For a numerical function :math:`\texttt{cdf}: \mathbb{F} \to \mathbb{F} \cap [0,1]`,
  the exact quantile is

  .. math::

     \begin{aligned}
     \texttt{quantile}(u) = \min\{ f \in \mathbb{F} \mid f \le \texttt{cdf}(x) \} && (u \in \mathbb{F} \cap [0,1]).
     \end{aligned}


.. function:: void bounds_quantile(cdf32_t cdf, double * xlo, double * xhi);
              void bounds_quantile_ext(ddf32_t ddf, double * xlo, double * xhi);

  These functions compute the values of the smallest and largest atoms of a
  distribution. The results are stored in the output parameters :var:`xlo`
  and :var:`xhi`.


Uniform Generation
------------------

Available in :src:`uniform.h`

Generating Floats
^^^^^^^^^^^^^^^^^

These functions generate a uniform number in the unit interval ``[0,1]``,
according to the following process:

  .. math::

     \begin{aligned}
     U' &\sim \mathrm{Uniform}_{\mathbb{R}}(0,1) \\
     U &\gets \mathrm{round}_{\mathbb{F}, \alpha}(U')
     \end{aligned}

where the variable :math:`U'` is an infinitely precise uniform real
and :math:`\mathrm{round}_{\mathbb{F}, \alpha}` is a rounding function
that converts a real number in :math:`\mathbb{R}` to a floating-point
number in :math:`\mathbb{F}`, according to a rounding mode :math:`\alpha`
(round up, round down, round to nearest).

.. doxygenenum:: round_mode

.. doxygenfunction:: uniformf
.. doxygenfunction:: uniform


Generating Dual Floats
^^^^^^^^^^^^^^^^^^^^^^

When performing inversion sampling using :func:`generate_inversion_ext`, it
is necessary to use a dual representation of floats that mirrors the dual
distribution function representations (see :ref:`overview:Guarantees`).

.. function:: void uniformf_ext(bool * d, float * q, struct flip_state * prng)
              void uniform_ext(bool * d, double * q, struct flip_state * prng)

  These functions simulate a uniform real number in the dual distribution
  format. The algorithm is as follows

  .. math::

     \begin{aligned}
     D &\sim \mathrm{Bernoulli}(1/2) \\
     \alpha &\gets D = 0 \;\, ? \;\, \texttt{RND\_U} : \texttt{RND\_D} \\
     U' &\sim \mathrm{Uniform}_{\mathbb{R}}(0,1/2) \\
     U &\gets \mathrm{round}_{\mathbb{F}, \alpha}(U')
     \end{aligned}

  The result of the sample is stored in :var:`d` and :var:`q`.
  Here ``d=0`` means that the sample is ``q`` and ``d=1`` means that the
  sample is ``1 - q`` where the subtraction is in exact (real) arithmetic.


Pseudorandom Number Generators
------------------------------

Available in :src:`prng.h` and :src:`flip.h`

Flip States
^^^^^^^^^^^

The generators described :ref:`above <api:Generating Random Variates>`
take as input a pseudorandom number generator called :data:`prng`, which
must be obtained using :func:`make_flip_state`. The usual pattern is to
make a flip state using a :data:`gsl_rng` pointer from the
`GNU Scientific Library <https://www.gnu.org/software/gsl/doc/html/rng.html>`__,
as follows.

.. code-block:: c

  // Prepare the random number generator.
  gsl_rng * rng = gsl_rng_alloc(gsl_rng_default);
  struct flip_state prng = make_flip_state(rng);

.. doxygenstruct:: flip_state

.. doxygenfunction:: make_flip_state

Random bits are obtained from a :data:`flip_state` using the following
functions.

.. doxygenfunction:: flip
.. doxygenfunction:: flip_k
.. doxygenfunction:: randint

Additional PRNGs
^^^^^^^^^^^^^^^^

There are a large number of
`pseudorandom number generators in the GSL <https://www.gnu.org/software/gsl/doc/html/rng.html>`_,
which can be used out of the box. librvg also provides two additional PRNG
types.

.. var:: extern const gsl_rng_type * gsl_rng_urandom

  This generator draws system-level entropy
  using the `getrandom <https://man7.org/linux/man-pages/man2/getrandom.2.html>`_
  syscall, which is typically :file:`/dev/urandom`. It maintains no state
  itself and cannot be seeded by the user, so calls such as
  :code:`gsl_rng_set` have no effect. Used primarily for workflows that
  require cryptographically secure random bits, see
  :cite:`fois2023` for a detailed description of the system entropy
  source.

.. var:: extern const gsl_rng_type * gsl_rng_deterministic

  This generator deterministically returns its seed. Its state consists of
  a single ``unsigned long int`` value (typically 32 bits) which is
  always returned. It is useful for debugging and characterizing the
  properties of generators.
