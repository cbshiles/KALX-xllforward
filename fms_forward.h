// fmsforward.h - forward curve
#pragma once
#include <vector>
#include "fms_bootstrap.h"

namespace fms {
namespace pwflat {

	template<class T = double, class F = double>
	class forward {
		std::vector<T> t;
		std::vector<F> f;
	public:
		forward(size_t n = 0, const T* t = nullptr, const F* f = nullptr)
			: t(t, t + n), f(f, f + n)
		{ }
		forward(const std::vector<T>& t, const std::vector<F>& f)
			: t(t), f(f)
		{
			if (t.size() != f.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": times and forwards must be the same size");
		}

		const curve<T,F>& curve() const
		{
			return curve<T,F>(t.size(), t.data(), f.data());
		}

		bool operator==(const forward& c) const
		{
			return t == c.t && f == c.f;
		}
		bool operator!=(const forward& c) const
		{
			return !operator==(c);
		}

		size_t size() const
		{
			return t.size();
		}

		F value(const T& u) const
		{
			return value(u, size(), t.data(), f.data());
		}
		// function object
		F operator()(const T& u) const
		{
			return value(u);
		}

		F spot(const T& u) const
		{
			return spot(u, size(), t.data(), f.data());
		}

		F discount(const T& u) const
		{
			return discount(u, size(), t.data(), f.data());
		}

		// extend curve
		forward& next(const instrument<T,F> i, F p = 0, F _f = 0)
		{
			_f = bootstrap::next(i, curve(), p, _f);

			t.push_back(i.last());
			f.push_back(_f);

			return *this;
		}
	};

} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_pwflat_forward()
{
	{
		// test default constructor
		fms::pwflat::forward<> f;

		assert (f.size() == 0);
		// test default copy constructor
		auto f1 = f;
		assert (f.size() == 0);
		// test default assignment
		f = f1;
		assert (f.size() == 0);
	}
	{
		// test array constructor
		int t[] = {1,2,3};
		double f[] = {.1,.2,.3};
		fms::pwflat::forward<int,double> c(3,t,f);

		assert (c.size() == 3);
		auto c1(c);
		c = c1;
		assert (c == c1);
		assert (!(c != c1));
	}
	{
		// test vector constructor
		std::vector<int> t{1,2,3};
		std::vector<double> f{.1,.2,.3};
		fms::pwflat::forward<int,double> c(t,f);

		assert (c.size() == 3);
		auto c1(c);
		c = c1;
		assert (c == c1);
		assert (!(c != c1));
	}
}

#endif // _DEBUG