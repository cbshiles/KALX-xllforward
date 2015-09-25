// fms_forward.h - forward curve
#pragma once
#include <vector>
#include "fms_bootstrap.h"
#include "fms_curve.h"
#include "fms_instrument.h"

namespace fms {
namespace pwflat {

	// convenience functions
	template<class T, class F>
	inline F discount(const F& u, const curve<T,F>& c)
	{
		return pwflat::discount(u, c.n,c.t,c.f,c._f);
	}

	template<class T, class F>
	inline F present_value(const instrument<T,F>& i, const curve<T,F>& c)
	{
		return present_value(i.m,i.u,i.c, c.n,c.t,c.f,c._f);
	}

	template<class T, class F>
	inline F duration(const instrument<T,F>& i, const curve<T,F>& c)
	{
		return duration(i.m,i.u,i.c, c.n,c.t,c.f,c._f);
	}

	template<class T = double, class F = double>
	class forward : public vector_curve<T,F> {
	public:
		forward(size_t n = 0, const T* t = nullptr, const F* f = nullptr, const F& _f = std::numeric_limits<F>::quiet_NaN())
			: vector_curve<T,F>(n, t, f, _f)
		{ }
		forward(const std::vector<T>& t, const std::vector<F>& f, const F& _f = std::numeric_limits<F>::quiet_NaN())
			: vector_curve<T,F>(t, f, _f)
		{
			if (t.size() != f.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": times and forwards must be the same size");
		}
		forward(const forward& f)
			: forward(f.t_, f.f_, f._f)
		{ }
		forward& operator=(const forward& g)
		{
			if (this != &g) {
				vector_curve<T,F>::operator=(g);
			}

			return *this;
		}
		~forward()
		{ }

		// operator(), spot, discount inherited from curve

		// extend curve
		forward& next(const instrument<T,F>& i, F p = 0, F e = 0)
		{
			e = bootstrap::next(i.m,i.u,i.c, n,t,f, p,e);

			push_back(i.last(), e);			

			return *this;
		}
	};

} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

inline void test_fms_forward()
{
	using namespace fms;

	{
		// test default constructor
		pwflat::forward<> f;

		assert (f.n == 0);
		// test default copy constructor
		auto f1 = f;
		assert (f.n == 0);
		// test default assignment
		f = f1;
		assert (f.n == 0);
	}
	{
		// test array constructor
		int t[] = {1,2,3};
		double f[] = {.1,.2,.3};
		pwflat::forward<int,double> c(3,t,f);

		assert (c.n == 3);
		auto c1(c);
		c = c1;
		assert (c == c1);
		assert (!(c != c1));
	}
	{
		// test vector constructor
		std::vector<int> t{1,2,3};
		std::vector<double> f{.1,.2,.3};
		pwflat::forward<int,double> c(t,f);

		assert (c.n == 3);
		auto c1(c);
		c = c1;
		assert (c == c1);
		assert (!(c != c1));
	}
	{
		pwflat::forward<> f;

		// bond maturities
		double t[] = {1,2,3,5,7,10};
		double c = 0.05;
		for (const auto ti : t) {
			// semiannual par bond
			f.next(bond<>(ti, SEMIANNUAL, c), 1);
		}
		// verify repricing
		for (const auto ti : t) {
			auto b = bond<>(ti, SEMIANNUAL, c);
			double pv = pwflat::present_value(b, f);
			assert (fabs(pv - 1) < std::numeric_limits<double>::epsilon());
		}
	}
	{ // test with random coupons
		std::default_random_engine dre;
		std::uniform_real_distribution<> u(-0.01,0.01);
		pwflat::forward<> f;

		// bond maturities
		double t[] = {1,2,3,5,7,10};
		double c = 0.05;

		std::vector<double> rc;
		for (int i = 0; i < sizeof(t) / sizeof(*t); i++) {
			rc.push_back(u(dre) + c);
		}

		for (int i = 0; i < sizeof(t) / sizeof(*t); i++) {
			// semiannual par bond
			// !!! set breakpoint here
			f.next(bond<>(t[i], SEMIANNUAL, rc[i]), 1);
		}
		
		// verify repricing
		for (int i = 0; i < sizeof(t) / sizeof(*t); i++) {
				auto b = bond<>(t[i], SEMIANNUAL, rc[i]);
				double pv = pwflat::present_value(b, f);
				//!!! find out why this is failing !!!
				double x; 
				x = pv - 1;
				assert (fabs(x) < 10*std::numeric_limits<double>::epsilon());
		}
	}
}

#endif // _DEBUG