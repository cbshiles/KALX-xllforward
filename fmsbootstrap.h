// bootstrap.h - bootstrap a curve
#pragma once
#include "newton.h"
#include "fmspwflat.h"

namespace fms {
namespace bootstrap {

	template<class T, class F>
	inline F present_value(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F pv{0};

		for (size_t i = 0; i < m; ++i)
			pv += c[i]*pwflat::discount(u[i], n, t, f, _f);

		return pv;
	}

	// derivative of present value wrt parallels shift of forward curve
	template<class T, class F>
	inline F duration(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F dur{0};

		for (size_t i = ui - u; i < m; ++i) {
			dur -= u[i]*c[i]*pwflat::discount(u[i], n, t, f, _f);
		}

		return dur;
	}

	// extend f(t) to make present value of c[i] at u[i] equal to p using initial guess _f
	template<class T, class F>
	inline F next(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, F p = 0, F _f = 0)
	{
		// current maximum curve time
		T t0 = n > 0 ? t[n - 1] : 0;

		auto ui = std::upper_bound(u, u + m, t0);
		if (ui == u + m)
			throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": no cash flows past end of curve");

		// present values of cash flows to curve end
		F p0 = present_value(ui - u, u, c, n, t, f);

		// skip cash flows to curve end
		m -= ui - u;
		u += ui - u;
		c += ui - u;

		auto f = [p,p0,m,u,c,n,t,f](F _f) {
			return -p + p0 + present_value(m, u, c, n, t, f, _f);
		};
		auto df = [m,u,c,n,t,f](F _f) {
			return duration(m, u, c, n, t, f, _f);
		};

		// initial bootstrap guess
		if (_f = 0)
			_f = n > 0 ? f[n - 1] : F(.01);

		return newton::root(_f, f, df);
	}

} // bootstrap
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_bootstrap()
{
}

#endif // _DEBUG