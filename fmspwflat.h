// fmspwflat.h - portable piecewise flat curve
#pragma once
#include <cassert>
#ifndef ensure
#define enusre(e) assert(e)
#endif
#include <cmath> // exp
#include <limits> // quiet_Nan()
#include <numeric> // upper/lower_bound

namespace fms {
namespace pwflat {

	// piecewise flat curve
	// return x[i] if t[i-1] < u <= t[i]
	// assumes t[i] monotonically increasing
	template<class T, class F>
	inline F value(const T& u, size_t n, const T* t, const F* f)
	{
		auto ti = std::lower_bound(t, t + n, u);
		
		if (u < 0 || i == t + n)
			return std::numeric_limits<F>::quiet_NaN();

		return x[ti - t];
	}

	// int_0^u f(t) dt
	template<class T, class F>
	inline F integral(const T& u, size_t n, const T* t, const F* f)
	{
		if (u > t[n - 1])
			return std::numeric_limits<T>::quiet_NaN();

		F I{0};
		T t_{0};

		size_t i;
		for (i = 0; i < n && t[i] <= u; ++i) {
			I += f[i] * (t[i] - t_);
			t_ = t[i];
		}

		if (i < n)
			I += f[i] *(u - t_);

		return I;
	}

	// discount D(u) = exp(-int_0^u f(t) dt)
	template<class T, class F>
	inline F discount(const T& u, size_t n, const T* t, const F* f)
	{
		return exp(-integral(u, n, t, f));
	}

	// spot r(u) = (int_0^u f(t) dt)/u
	template<class T, class F>
	inline F spot(const T& u, size_t n, const T* t, const F* f)
	{
		return u + 1 == 1 ? f[0] : integral(u, n, t, f)/u;
	}

} // pwflat
} // fms

#ifdef _DEBUG

inline void test_fms_pwflat()
{
}

#endif // _DEBUG