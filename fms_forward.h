// fmsforward.h - forward curve
#pragma once
#include <vector>
#include "fms_bootstrap.h"

namespace fms {
namespace pwflat {

	template<class T = double, class F = double>
	class forward : public vector_curve<T,F> {
	public:
		forward(size_t n = 0, const T* t = nullptr, const F* f = nullptr)
			: vector_curve<T,F>(n, t, f)
		{ }
		forward(const std::vector<T>& t, const std::vector<F>& f)
			: vector_curve<T,F>(t, f)
		{
			if (t.size() != f.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": times and forwards must be the same size");
		}
		F value(const T& u) const
		{
			return value(u, n, t, f);
		}

		F spot(const T& u) const
		{
			return spot(u, n, t, f);
		}

		F discount(const T& u) const
		{
			return discount(u, n, t, f);
		}

		// extend curve
		forward& next(const instrument<T,F>& i, F p = 0, F e = 0)
		{
			e = bootstrap::next(i, *this, p, e);

			push_back(i.last(), e);			

			return *this;
		}
	};

} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_pwflat_forward()
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

		double t[] = {1,2,3,5,7,10};
		double c = 0.05;
		for (const auto ti : t) {
			f.next(bond<>(ti, SEMIANNUAL, c), 1);
		}
		for (const auto ti : t) {
			auto b = bond<>(ti, SEMIANNUAL, c);
			double pv = pwflat::present_value(b, f);
			assert (fabs(pv - 1) < std::numeric_limits<double>::epsilon());
		}
	}
}

#endif // _DEBUG