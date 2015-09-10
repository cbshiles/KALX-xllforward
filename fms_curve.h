// fms_curve.h - set of points for a curve
// IDEA: template<class T, class F> class curve { T t; F f; iterator_traits<F>::value_type _f; ... }
#pragma once
#include <vector>
#include "fms_pwflat.h"

namespace fms {
namespace pwflat {

	// NOT a value type
	template<class T = double, class F = double>
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
				&& std::equal(f, f + n, c.f, c.f + c.n);
		}
		bool operator!=(const curve& c) const
		{
			return !operator==(c);
		}
		
		F operator()(const T& u) const
		{
			return pwflat::value(u, n, t, f, _f);
		}
		F spot(const T& u) const
		{
			return spot(u, n,t,f);
		}
		F discount(const T& u) const
		{
			return discount(u, n,t,f);
		}

		// last maturity in curve
		T last() const
		{
			return n > 0 ? t[n-1] : 0;
		}
	};

	// a regular value type
	template<class T = double, class F = double>
	class vector_curve : public curve<T,F> {
	protected:
		std::vector<T> t_;
		std::vector<F> f_;
	public:
		vector_curve(size_t n = 0)
			: curve<T,F>(n)
		{
			t = t_.data();
			f = f_.data();
		}
		vector_curve(size_t n, const T* t, const F* f)
			: curve<T,F>(n), t_(t, t + n), f_(f, f + n)
		{
			curve<T,F>::t = t_.data();
			curve<T,F>::f = f_.data();
		}
		vector_curve(const std::vector<T>& t, const std::vector<F>& f)
			: curve<T,F>(t.size()), t_(t), f_(f)
		{
			if (t.size() != f.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ "time and forward vector must be the same size");

			curve<T,F>::t = t_.data();
			curve<T,F>::f = f_.data();
		}

		// extend using a time and forward value
		vector_curve& push_back(const T& u, const F& g)
		{
			if (u <= last())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": curve times must be increasing");

			t_.push_back(u);
			f_.push_back(g);

			// update base members
			++n;
			t = t_.data();
			f = f_.data();

			return *this;
		}

	};

} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_curve()
{
	using namespace fms;

	{
		pwflat::curve<> c;
		assert (c.n == 0);
		auto c2 = c;
		assert (c2 == c);
		c = c2;
		assert (!(c != c2));
	}
	{
		double t[] = {1,2,3};
		double f[] = {.1,.2,.3};

		pwflat::curve<> c0(3,t,f);
		pwflat::vector_curve<> c1(std::vector<double>(std::begin(t),std::end(t)), std::vector<double>(std::begin(f),std::end(f)));
		//!!! add more tests, e.g is c0 == c1? do copy constructor/assignment work?
	}
	{ //!!! test operator(), spot, discount
	}
	{
		pwflat::vector_curve<> c(std::vector<double>{1,2,3}, std::vector<double>{.1,.2,.3});
		auto c2 = c;
		assert (c2 == c);
		assert (c.n == 3);
		assert (c.t[0] == 1);
		assert (c.f[2] == .3);

		pwflat::vector_curve<> c3;
		assert (c3.n == 0);
		c3.push_back(c.t[0], c.f[0]);
		assert (c3.n == 1);
		c3.push_back(c.t[1], c.f[1]);
		c3.push_back(c.t[2], c.f[2]);

		assert (c3 == c);
	}
}

#endif // _DEBUG