// newton.h - newton method for root finding
#pragma once
#include <functional>

namespace fms {
namespace newton {

	template<class X, class Y>
	using function = std::function<Y(X)>;

	template<class X, class Y>
	inline X step(const X& x, const function<X,Y>& f, const function<X,Y>& df, const Y& m = 0.5)
	{
		Y dfx = df(x);

		// slope must be > m or < -m
		if (fabs(dfx) < m)
			dfx = copysign(m, dfx);

		return x - f(x)/dfx;
	}

	template<class X, class Y>
	inline X root(X x, const function<X,Y>& f, const function<X,Y>& df, int n = 2)
	{
		X x_ = step(x, f, df);
#ifdef _DEBUG
		int iter = 0;
#endif
		while (fabs(x_ - x) > n*std::numeric_limits<X>::epsilon()) {
#ifdef _DEBUG
			++iter;
			if (iter > 1000)
				return std::numeric_limits<X>::quiet_NaN();
#endif
			x = x_;
			x_ = step(x, f, df);
		}

		return fabs(f(x_)) < fabs(f(x)) ? x_ : x;
	}

} // newton
} // fms

#ifdef _DEBUG
#include <cassert>
#include <ctime>
#include <random>

inline void test_fms_newton(int n = 1000)
{
	std::default_random_engine dre;
	dre.seed((unsigned)time(0));
	std::uniform_real_distribution<> u;
	{
		auto f = [](double x) { return x*x - 2; };
		auto df = [](double x) { return 2*x; };

		double sqrt2 = sqrt(2);
		for (int i = 0; i < n; ++i) {
			double x = 1/u(dre);
			double r = fms::newton::root<double,double>(x, f, df);
			assert (fabs(sqrt2 - r) <= 2*std::numeric_limits<double>::epsilon());
		}
	}
}

#endif // _DEBUG