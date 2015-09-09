// fms_instrument.h - instruments classes
#pragma once
#include <cmath>
#include <algorithm>
#include <vector>

namespace fms {

	enum frequency { 
		NONE = 0,
		ANNUAL = 1,
		SEMIANNUAL = 2,
		QUARTERLY = 4,
		MONTHLY = 12
	};

	template<class T = double, class F = double>
	struct instrument {
		size_t m;
		const T* u; // times
		const F* c; // cash flows

		bool operator==(const instrument& i) const
		{
			return m == i.m 
				&& std::equal(u, u + m, i.u, i.u + i.m)
				&& std::equal(c, c + m, i.c, i.c + i.m);
		}
		bool operator!=(const instrument& i) const
		{
			return !operator==(i);
		}

		// Time of last cash flow. AKA maturity, termination
		T last() const
		{
			return m > 0 ? u[m-1] : std::numeric_limits<T>::quiet_NaN();
		}
	};

	template<class T = double, class F = double>
	struct bond : public instrument<T,F> {
		std::vector<T> u;
		std::vector<F> c;
	public:
		bond(T maturity = 0, frequency freq = NONE, F coupon = 0)
			: instrument{static_cast<size_t>(ceil(freq*maturity))}, u(m), c(m)
		{
			instrument<T,F>::u = u.data();
			instrument<T,F>::c = c.data();

			// fill backwards from maturity
			T i = 0;
			std::generate(u.rbegin(), u.rend(), [&]() { return maturity - i++/freq; });
			std::fill(c.begin(), c.end(), coupon);
			c.back() += 1; // standard notional
		}

	};

} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_instrument()
{
	using namespace fms;

	{
		bond<> b(3, SEMIANNUAL, 0.05);
		assert (b.m == 6);
		for (size_t i = 0; i < b.m; ++i) {
			assert (b.u[i] == 0.5 + 1.*i/SEMIANNUAL);
			assert (b.c[i] == (i < b.m - 1 ? 0.05 : 1.05));
		}
	}
	{ // short first coupon
		bond<> b(3.25, SEMIANNUAL, 0.05);
		assert (b.m == 7);
		for (size_t i = 0; i < b.m; ++i) {
			assert (b.u[i] == 0.25 + 1.*i/SEMIANNUAL);
			assert (b.c[i] == (i < b.m - 1 ? 0.05 : 1.05));
		}
	}
	{
		bond<> b(2, QUARTERLY, 0.01);
		auto b2(b);
		assert (b2 == b);
		b = b2;
		assert (!(b != b2));
	}
}

#endif // _DEBUG