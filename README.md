# xllforward

This project is for bootstrapping forward curves using instruments.
The _discout_, D(t), is the value of 1 unit paid at time t. Define the
_spot rate_, r(t), by D(t) = exp(-t r(t)) and the (instantaneous)
_forward rate_, f(t) by D(t) = exp(-int_0^t f(s) ds).

Note the spot r(t) = (1/t) int_0^t f(s) ds is the average of the forward.

All forward curves are piecewise constant. They are specified by
times, t[i], forwards, f[i], and an extrapolation value _f.
We define f(t) = f[i] if t[i-1] < t <= t[i], 0 < i < n, and f(t) = _f if t > t[n-1].
The curve is not defined for t < 0. This in indicated by returning a NaN.

## fms_pwflat.h

The function `value` calculates the value of a forward curve as described above.
The function `integral` compute the integral of a piecewise constant curve.
The functons `discount`, `spot`, and `forward` are defined in terms of these.

## fms_instrument.h

An instrument is a sequence of cash flows, c[i], at time u[i]. 