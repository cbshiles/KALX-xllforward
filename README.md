# xllforward

This project lets you bootstrap forward curves using instruments.

## Bootstrap

The _discount_, \(D(t)\), is the price now of 1 unit paid at time \(t\).
Define the _spot rate_, \(r(t)\), by \(D(t) = exp(-t r(t))\) and the (instantaneous)
_forward rate_, \(f(t)\), by \(D(t) = exp(-int_0^t f(s)\,ds)\).
Note the spot \(r(t) = (1/t) int_0^t f(s)\,ds\) is the average of the forward.

A _fixed income instrument_ pays _cash flows_ \(c_j\) at times \(u_j\). 
If it has price \(p\) the discount _reprices_ the instrument if \(p = \sum_j c_j D(u_j)\).
Given a collection of fixed income instruments and prices there are many discount curves that will reprice them. 
Bootstrapping provides a unique solution to this problem.

The _maturity_ or _termination_ (or _tenor_) of an instrument is the time of the last cash flow.
Sort the instruments by increasing maturity. We can find a unique constant forward rate, \(f_0\),
over the tenor of the first instrument that reprices it: \(p^0 = \sum_j c_j^0 e^{-u_j^0 f_0}\).

Similarly, we can find a unique constant forward rate, \(f_1\), over the maturity of the first instrument
to the maturity of the second instrument that reprices it. The price of the first instrument will not
be effected by the value of \(f_1\). Continuing in this manner constructs a
piecewise constant forward curve that reprices all instruments.

### Remarks

It is not always the case we can bootstrap arbitrary data. E.g., if the cash flows are all positive
and the price is negative, there is no solution even if we allow negative forward rates.

Since the bootstrap method leaves the curve to from 0 to the previous maturity fixed, it may be impossible
or require extreme forward rates to reprice the next instrument. It is a good idea to keep successive
maturities well spaced and be sure the prices you use are actually traded in the market.

## `fms_pwflat.h`

We use forward curves that are piecewise constant. They are specified by
times, \(t_i\), forwards, \(f_i\), and an extrapolation value \(_f\).
We define \(f(t) = f_i\) if \(t_{i-1} < t \le t_i_\), \(0 < i < n\), and \(f(t) = _f\) if \(t > t_{n-1}\).
The curve is not defined for \(t < 0\).

The function `fms::pwflat::value` calculates the value of a forward curve as described above.
It returns not-a-number (NaN) when the curve is not defined.
The function `integral` computes the integral of a piecewise constant curve.
The functons `discount` and `spot` are defined in terms of these.
Note how only `value` and `integral` do error checking and return NaN's
instead of throwing exceptions. The other functions return NaN's on error.

This file also provids `present_value` and `duration` for valuing an instrument
and computing the derivative with respect to a parallel shift of the forward curve.

## `fms_bootstrap.h`

Using functions from `fms_pwflat.h`, the function `fms::bootstrap::next` returns the next forward rate that will reprice the
given instrument using the curve that has been built up to that point.

## `fms_curve.h`

The struct `fms::pwflat::curve` collects the size, time pointer, forward pointer, and the extrapolation constant.
It provides equality tests, allows curves to be used as function objects, and provides
the `last` time point in the curve before extrapolation kicks in. The user is responsible for the
memory being pointed at. This makes it possible to avoid copying preexisting memory.

The class `vector_curve` is `curve` that is a value type. It provides `push_back` to add points past the end of the curve.
If the point is not past the last value then an exception is thrown, unlike the routines in
`fms_pwflat.h` that return NaNs. It stores copies of the times and forwards in `std::vector`s.

## `fms_instrument.h`

The struct `fms::instrument` collects the size, time pointer, and cash flow pointer.
It provides equality tests an provides the `last` maturity. The user is responsible for the
meemory being pointed at.

The class `vector_instrument` is an `instrument` that is a value type. TODO: add `insert` to insert time, cash flow pairs.

The class `bond` is a `vector_instrument` that models simple, periodic bonds and allows for short first coupons.

## `fms_forward.h`

The `fms::pwflat::forward` class allows you bootstrap forward curves using instruments. Instantiate
a foward curve, then call `next(i,p)` with a instruments of increasing maturity and their prices.