// bootstrap.h - bootstrap a curve
#pragma once
#include "fmspwflat.h"
#include "newton.h"

namespace fms {
namespace bootstrap {

	template<class T, class F>
	inline F present_value(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f)
	{
		F pv{0};

		for (size_t i = 0; i < m; ++i)
			pv += c[i]*pwflat::discount(u[i], n, t, f);

		return pv;
	}

	// d(pv)/df for parallel shift past u0
	template<class T, class F>
	inline F duration(const T& u0, size_t m, const T* u, const F* c, size_t n, const T* t, const F* f)
	{
		auto ui = std::lower_bound(u, u + m, u0);

		F dur{0};
		for (size_t i = ui - u; i < m; ++i) {
			dur -= (u[i] - u0)*(c[i])*pwflat::discount(u[i], n, t, f);
		}

		return dur;
	}

	// extend f(t) to make present value  of c[i] at u[i] equal to p
	template<class T, class F>
	inline F step(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, F p = 0, F _f = 0)
	{
		// current maximum curve time
		T t0 = n > 0 ? t[n - 1] : 0;

		auto ui = std::upper_bound(u, u + m, t0);
		// no cash flows past t0
		ensure (ui != u + m);

		// present values of cash flows to curve end
		F p0 = present_value(ui - u, u, c, n, t, f);

		m -= ui - u;
		u += ui - u;
		c += ui - u;

		auto f = [p,p0,m,u,c,n,t,f](F _f) {
			return -p + p0 + present_value(m, u, c, n, t, f);
		};
		auto df = [t0,m,u,c,n,t,f](F _f) {
			return duration(t0, m, u, c, n, t, f);
		};

		// initial bootstrap guess
		if (_f = 0)
			_f = n > 0 ? f[n - 1] : F(.01);

		return newton::root(_f, f, df);
	}

} // bootstrap
} // fms