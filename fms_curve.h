// fms_curve.h - set of points for a curve
#pragma once

namespace fms {

	template<class T, class F>
	struct curve {
		size_t n;
		const T* t; // times
		const F* f; // forwards
		F _f;       // extrapolate
		curve(size_t n = 0, const T* t = nullptr, const F* f = nullptr, const F& _f = std::numeric_limits<F>::quiet_NaN())
			: n(n), t(t), f(f), _f(_f)
		{ }
		
		bool operator==(const curve& c) const
		{
			return n == c.n
				&& std::equal(t, t + n, c.t, c.t + c.n)
				&& std::equal(f, f + n, f.t, f.t + f.n);
		}
		bool operator!=(const curve& c) const
		{
			return !operator==(c);
		}
		
		// last maturity in curve
		T last() const
		{
			return n > 0 ? t[n-1] : 0;
		}
	};

} // fms