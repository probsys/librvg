API Reference
=============

The functions documented in this references are in :file:`generate.h`.

Defining A Target Distribution
------------------------------

To generate random variates, librvg operates using numerical specifications
of cumulative distribution functions (CDF) and survival functions (SF).
These functions must adhere to the following type.

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
        - Atom at ``-INFINITY``: let ``F(nextafterf(INFINITY, 0)) < F(INFINITY)``.
        - Atom at ``NAN``: let ``F(-INFINITY) < 1``.

      It is also possible to specify separate atoms at the negative
      and positive zeros (``0+``, ``0-``) of floating-point.

An advanced feature of librvg is the notion of a "dual distribution
function" (DDF), which combines a numerical CDF and SF to enable extended
accuracy sampling. This representation is described in the
:ref:`overview:Guarantees` and in :cite:p:`saad2025pldi{§6}`.
The type of a DDF is given in as

.. type:: void (*ddf32_t)(double x, bool * b, float * p);

    This function takes as input a double :data:`x`, and writes the
    result of evaluating a DDF at :data:`x` to the pointers
    :data:`b` and :data:`p`.
    The interpretation of :data:`b` and :data:`p` are as follows:

    - If :data:`b` is set to 0, then :math:`\Pr(X \le x) = p`

    - If :data:`b` is set to 1, then :math:`\Pr(X \le x) = 1 - p`

Functions that operate using a DDF have an ``_ext`` suffix (meaning "extended").

Wrapping a Distribution
^^^^^^^^^^^^^^^^^^^^^^^

librvg can interporate with existing CDF or SF implementations from the
`GNU Scientific Library <https://www.gnu.org/software/gsl/doc/html/randist.html>`_.
The following macros are available, where
:data:`name` should be a fresh C identifier (not a string)
for the name of the new function created from :data:`func`. The suffixes
``_P`` and ``_Q`` correspond to a CDF and SF, respectively, a naming
convention inherited from the GSL. The ``...`` varargs are passed
directly to :data:`func`.

.. doxygendefine:: MAKE_CDF_P
.. doxygendefine:: MAKE_CDF_Q
.. doxygendefine:: MAKE_CDF_UINT_P
.. doxygendefine:: MAKE_CDF_UINT_Q
.. doxygendefine:: MAKE_DDF

The :func:`MAKE_DDF` macro is not specific to a CDF or SF created from the
GSL, it can be used for any such pairing.

The following function from :file:`discrete.h` wraps an array of cumulative
probabilities into function of type :type:`cdf32_t`.

.. doxygenfunction:: cdf_discrete

Generating Random Variates
--------------------------

Entropy-Optimal Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^

These generators are described in :cite:`saad2025pldi`.

.. doxygenfunction:: generate_opt
.. doxygenfunction:: generate_opt_ext

Conditional-Bit Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^

librvg also includes implementations of the Conditional Bit Sampling
method. These functions are described in :cite:t:`Sobolewski1972{Section II}`.
The performance of these methods is generally inferior to that of
:func:`generate_opt` and :func:`generate_opt_ext` in terms
of entropy consumption and runtime, as they use arbitrary precision
arithmetic via the `GNU MP Bignum Library <https://gmplib.org/>`_ (libgmp).

.. doxygenfunction:: generate_cbs
.. doxygenfunction:: generate_cbs_ext

Querying a CDF
--------------

.. doxygenfunction:: quantile
.. doxygenfunction:: quantile_sf
.. doxygenfunction:: quantile_ext
.. doxygenfunction:: bounds_quantile
.. doxygenfunction:: bounds_quantile_sf
.. doxygenfunction:: bounds_quantile_ext
