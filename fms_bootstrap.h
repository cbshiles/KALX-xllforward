// fms_bootstrap.h - bootstrap a curve
#pragma once
#include "newton.h"
#include "fms_curve.h"
#include "fms_instrument.h"
#include "fms_pwflat.h"

namespace fms {
namespace pwflat {

	template<class T, class F>
	inline F discount(const F& u, const curve<T,F>& c)
	{
		return pwflat::discount(u, c.n, c.t, c.f, c._f);
	}

	template<class T, class F>
	inline F present_value(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F pv{0};

		for (size_t i = 0; i < m; ++i)
			pv += c[i]*pwflat::discount(u[i], n, t, f, _f);

		return pv;
	}
	template<class T, class F>
	inline F present_value(const instrument<T,F>& i, const curve<T,F>& c)
	{
		return present_value(i.m, i.u, i.c, c.n, c.t, c.f, c._f);
	}

	// derivative of present value wrt parallels shift of forward curve
	template<class T, class F>
	inline F duration(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, const F& _f = std::numeric_limits<F>::quiet_NaN())
	{
		F dur{0};

		for (size_t i = 0; i < m; ++i) {
			dur -= u[i]*c[i]*pwflat::discount(u[i], n, t, f, _f);
		}

		return dur;
	}
	template<class T, class F>
	inline F duration(const instrument<T,F>& i, const curve<T,F>& c)
	{
		return duration(i.m, i.u, i.c, c.n, c.t, c.f, c._f);
	}
} // pwflat

namespace bootstrap {

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
		auto m0 = ui - u;
		F p0 = present_value(m0, u, c, n, t, f);

		// skip cash flows to curve end
		m -= m0;
		u += m0;
		c += m0;

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
	template<class T, class F>
	inline F next(const instrument<T,F>& i, const curve<T,F>& c, F p = 0, F _f = 0)
	{
		return next(i.m, i.u, i.c, c.n, c.t, c._f, p, _f);
	}

} // bootstrap
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_bootstrap()
{
	using namespace fms;
	using pwflat::discount;
	using pwflat::present_value;
	using pwflat::duration;

	double t[] = {1,2,3};
	double f[] = {.1,.2,.3};
	double u[] = {1.1, 2.9};
	double c[] = {4, 5};

	{
		auto d = curve<double,double>(3,t,f);
		auto i = instrument<double,double>{2,u,c};

		double D[] = { discount(u[0], d), discount(u[1], d) };
		assert (c[0]*D[0] + c[1]*D[1] == present_value(i,d));
		assert (u[0]*c[0]*D[0] + u[1]*c[1]*D[1] == -duration(i,d));

		// check duration numerically
		double h = sqrt(std::numeric_limits<double>::epsilon());
		std::transform(f, f + 3, f, [h](double x) { return x + h; });
		auto pv_up = present_value(i, d); // f + h

		std::transform(f, f + 3, f, [h](double x) { return x - 2*h; });
		auto pv_dn = present_value(i, d); // f - h
		auto dur_ = (pv_up - pv_dn)/(2*h);

		std::transform(f, f + 3, f, [h](double x) { return x + h; });
		auto dur = duration(i, d); // f
		assert (fabs(dur_ - dur) < h);
	}
}

#endif // _DEBUG