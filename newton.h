// newton.h - newton root finding
#pragma once
#include <functional>

namespace fms {
namespace newton {

	template<class X, class Y>
	using function = std::function<Y(X)>;

	template<class X, class Y>
	inline X step(const X& x, const function<X,Y>& f, const function<X,Y>& df)
	{
		Y dfx = df(x);

		if (fabs(dfx) < .5)
			dfx = copysign(.5, dfx);

		return x - f(x)/dfx;
	}

	template<class X, class Y>
	inline X root(const X& x, const function<X,Y>& f, const function<X,Y>& df, const X& dx = 2*std::numeric_limits<X>::epsilon())
	{
		X x_ = step(x, f, df);

		while (fabs(x_ - x) > epsilon) {
			x = x_;
			x_ = next(x, f, df);
		}

		return x_;
	}

} // newton
} // fms
