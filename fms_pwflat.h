// fms_pwflat.h - piecewise flat curve
/*
	f(t) = f[i] if t[i-1] < t <= t[i]; 
	     = _f if t > t[n-1];
	and undefined if t < 0

	|                                   _f
	|        f[1]             f[n-1] o--------
	| f[0] o------          o--------x
	x------x      ... ------x   
	|
	0-----t[0]--- ... ---t[n-2]---t[n-1]
*/
#pragma once
#include <cmath>     // exp
#include <algorithm> // adjacent_find
#include <limits>    // quiet_Nan()
#include <numeric>   // upper/lower_bound

namespace fms {
namespace pwflat {

	template<class I>
	inline bool monotonic(I b, I e)
	{
		using T = std::iterator_traits<I>::value_type;

		return e == std::adjacent_find(b, e, [](const T& t0, const T&t1) { return t0 >= t1; });
	}
	template<class T>
	inline bool monotonic(size_t n, const T* t)
	{
		return monotonic(t, t + n);
	}

	// piecewise flat curve
	// return f[i] if t[i-1] < u <= t[i]
	// assumes t[i] monotonically increasing
	template<class T, class F>
	inline F value(const T& u, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
#ifdef _DEBUG
		if (!monotonic(n, t))
			return std::numeric_limits<F>::quiet_NaN();
#endif
		if (u < 0)
			return std::numeric_limits<F>::quiet_NaN();

		auto ti = std::lower_bound(t, t + n, u);
		

		return ti == t + n ? _f : f[ti - t];
	}

	// int_0^u f(t) dt
	template<class T, class F>
	inline F integral(const T& u, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		if (u < 0)
			return std::numeric_limits<T>::quiet_NaN();

		F I{0};
		T t_{0};

		size_t i;
		for (i = 0; i < n && t[i] <= u; ++i) {
			I += f[i] * (t[i] - t_);
			t_ = t[i];
		}
		I += (n == 0 || u > t[n-1] ? _f : f[i]) *(u - t_);

		return I;
	}

	// discount D(u) = exp(-int_0^u f(t) dt)
	template<class T, class F>
	inline F discount(const T& u, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		return exp(-integral(u, n, t, f, _f));
	}

	// spot r(u) = (int_0^u f(t) dt)/u
	template<class T, class F>
	inline F spot(const T& u, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		return u < t[0] ? f[0] : integral(u, n, t, f, _f)/u;
	}

	// value of instrument having cash flow c[i] at time u[i]
	template<class T, class F>
	inline F present_value(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F p{0};

		for (size_t i = 0; i < m; ++i)
			p += c[i]*pwflat::discount(u[i], n, t, f, _f);

		return p;
	}

	// derivative of present value wrt parallels shift of forward curve
	template<class T, class F>
	inline F duration(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F d{0};

		for (size_t i = 0; i < m; ++i) {
			d -= u[i]*c[i]*pwflat::discount(u[i], n, t, f, _f);
		}

		return d;
	}
} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>
#include <vector>

inline void test_fms_pwflat()
{
	using namespace fms::pwflat;

	std::vector<double> t{1,2,3}, f{.1,.2,.3};

	{ // monotonic
		assert (monotonic(std::begin(t), std::end(t)));
		assert (monotonic(std::begin(f), std::end(f)));
		double f2 = f[2];
		f[2] = -1;
		assert (!monotonic(std::begin(f), std::end(f)));
		f[2] = f2;
		assert (!monotonic(std::rbegin(f), std::rend(f)));
	}
	{ // forward
		assert (isnan(value<int,double>(0, 0, nullptr, nullptr)));
		int u;
		u = 1;
		double x{0.1}, x_;
		x_ = fms::pwflat::value<int,double>(u, 0, nullptr, nullptr, x);
		assert (x_ == x);

		u = 1;
		x_ = fms::pwflat::value<int,double>(u, 0, nullptr, nullptr, x);
		assert (x_ == x);

		u = -1;
		x_ = fms::pwflat::value<int,double>(u, 0, nullptr, nullptr, x);
		assert (x_ != x_);
		assert (isnan(x_));
		//		assert (x == forward<int,double>(0, 0, nullptr, nullptr, 0.1)); // !!!VS 2015 compiler error

		for (int i = 0; i < 3; ++i)
			assert (f[i] == value(t[i], t.size(), t.data(), f.data()));
	  //!!! add tests
	}
	{ // integral
		double u;
		u = -1;
		assert (isnan(integral(u, t.size(), t.data(), f.data())));
		u = 4;
		assert (isnan(integral(u, t.size(), t.data(), f.data())));
		u = 0;
		assert (0 == integral(u, t.size(), t.data(), f.data()));
		u = 0.5;
		assert (.1*.5 == integral(u, t.size(), t.data(), f.data()));
		u = 1;
		assert (.1 == integral(u, t.size(), t.data(), f.data()));
		u = 1.5;
		assert (.1 + .2*.5 == integral(u, t.size(), t.data(), f.data()));
		u = 2.5;
		assert (.1 + .2 + .3*.5 == integral(u, t.size(), t.data(), f.data()));
		u = 3;
		assert (.1 + .2 + .3 == integral(u, t.size(), t.data(), f.data()));
		assert (.1 + .2 + .3 != .6); //!!!
	}
	{ // discount
		//!!! add tests
	}
	{ // spot
		//!!! add tests
	}
	{ // present_value
		// !!! add tests
	}
}

#endif // _DEBUG