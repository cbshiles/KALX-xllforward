// newton.h - newton method for root finding
#pragma once
#include <functional>

namespace fms {
namespace newton {

	template<class X, class Y>
	inline X step(const X& x, const std::function<Y(X)>& f, const std::function<Y(X)>& df, const Y& m = 0.5)
	{
		Y dfx = df(x);

		// slope must be > m or < -m
		if (fabs(dfx) < m)
			dfx = copysign(m, dfx);

		return x - f(x)/dfx;
	}

	template<class X, class Y>
	inline X root(X x, const std::function<Y(X)>& f, const std::function<Y(X)>& df, int n = 2)
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

inline void test_fms_newton(int n = 0/*1000*/)
{
	std::default_random_engine dre;
	dre.seed((unsigned)time(0));
	std::uniform_real_distribution<> u;
	{
		double a = 1/u(dre);
		auto f = [a](double x) { return x*x - a; };
		auto df = [](double x) { return 2*x; };

		double sqrta = sqrt(a);
		for (int i = 0; i < n; ++i) {
			double x = 1/u(dre);
			double r = fms::newton::root<double,double>(x, f, df);
			assert (fabs(sqrta - r) <= 20*std::numeric_limits<double>::epsilon());
		}
	}
}

#endif // _DEBUG