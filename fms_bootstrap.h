// fms_bootstrap.h - bootstrap a curve
#pragma once
#include "newton.h"
//#include "fms_curve.h"
//#include "fms_instrument.h"
#include "fms_pwflat.h"

namespace fms {
namespace bootstrap {

	// extend f(t) to make present value of c[i] at u[i] equal to p using initial guess _f
	template<class T, class F>
	inline F next(size_t m, const T* u, const F* c, size_t n, const T* t, const F* f, F p = 0, F _f = 0)
	{
		// end of current curve
		T t0 = n > 0 ? t[n - 1] : 0;

		// index to cash flows before end of curve
		auto ui = std::upper_bound(u, u + m, t0);
		if (ui == u + m)
			throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": no cash flows past end of curve");

		// present values of cash flows to end of curve
		auto m0 = ui - u;
		F p0 = pwflat::present_value(m0, u, c, n, t, f);

		// incrment to cash flows past curve end
		m -= m0;
		u += m0;
		c += m0;

		auto pv = [p,p0,m,u,c,n,t,f](F _f) {
			return -p + p0 + pwflat::present_value(m, u, c, n, t, f, _f);
		};
		auto dur = [m,u,c,n,t,f](F _f) {
			return pwflat::duration(m, u, c, n, t, f, _f);
		};

		// initial bootstrap guess
		if (_f == 0)
			_f = n > 0 ? f[n - 1] : F(.01);

		return newton::root<F,F>(_f, pv, dur);
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

	{
		double t[] = {1,2,3};
		double f[] = {.1,.2,.3};
		double u[] = {1.1, 2.9};
		double c[] = {4, 5};

		double D[] = { discount(u[0], 3,t,f), discount(u[1], 3,t,f) };
		assert (c[0]*D[0] + c[1]*D[1] == present_value(2,u,c, 3,t,f));
		assert (u[0]*c[0]*D[0] + u[1]*c[1]*D[1] == -duration(2,u,c, 3,t,f));

		// check duration numerically
		double h = sqrt(std::numeric_limits<double>::epsilon());
		std::transform(f, f + 3, f, [h](double x) { return x + h; });
		auto pv_up = present_value(2,u,c, 3,t,f); // f + h

		std::transform(f, f + 3, f, [h](double x) { return x - 2*h; });
		auto pv_dn = present_value(2,u,c, 3,t,f); // f - h
		auto dur_ = (pv_up - pv_dn)/(2*h);

		std::transform(f, f + 3, f, [h](double x) { return x + h; });
		auto dur = duration(2,u,c, 3,t,f); // f
		assert (fabs(dur_ - dur) < h);
	}
	{ // bootstrap
		double eps = std::numeric_limits<double>::epsilon();
		double f0 = 0.04; // D(t) = exp(-t*f0)
		double f[3];
		double t[] = {1,2,3};
		double c0[1], c1[2], c2[3];

		// !!!test bootstrap returns (approximately) f0
		// 1 = c0[0]*D(t[0])
		c0[0] = 1/exp(-t[0]*f0);
		f[0] = bootstrap::next<double,double>(1,t,c0, 0,t,f, 1);
		assert (fabs(f[0] - f0) < eps);

		// 1 = c1[0]*D(t[0]) + c1[1]*D(t[1])
		c1[0] = c0[0];
		c1[1] = (1 - c1[0]*exp(-t[0]*f0))/exp(-t[1]*f0);
		f[1] = bootstrap::next<double,double>(2,t,c1, 1,t,f, 1);
		assert (fabs(f[1] - f0) < eps);

		// 1 = c2[0]*D(t[0]) + c2[1]*D(t[1]) + c2[2]*D(t[2])
		c2[0] = c1[0];
		c2[1] = c1[1];
		c2[2] = (1 - c2[0]*exp(-t[0]*f0)-c2[1]*exp(-t[1]*f0))/exp(-t[2]*f0); //!!! replace with appropriate value
		f[2] = bootstrap::next<double,double>(3,t,c2, 2,t,f, 1);
		assert (fabs(f[2] - f0) < eps);
	}
}

#endif // _DEBUG