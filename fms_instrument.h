// fms_instrument.h - instruments classes
#pragma once
#include <cmath>
#include <algorithm>
#include <vector>

namespace fms {

	// NOT a value type
	template<class U = double, class C = double>
	struct instrument {
		size_t m;
		const U* u; // times
		const C* c; // cash flows

		// construct with instrument<U,C>{m,u,c}

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
		U last() const
		{
			return m > 0 ? u[m-1] : std::numeric_limits<U>::quiet_NaN();
		}
	};

	//!!! put in another file
	template<class U = double, class C = double>
	class vector_instrument : public instrument<U,C> {
	protected:
		std::vector<U> u_;
		std::vector<C> c_;
	public:
		vector_instrument(size_t m = 0)
			: instrument<U,C>(instrument<U,C>{m}), u_(m), c_(m)
		{
			// this must occur after u_, c_ created
			u = u_.data();
			c = c_.data();
		}
		vector_instrument(size_t m, const U* u, const C* c)
			: instrument<U,C>(instrument<U,C>{m}), u_(u, u + n), c_(c, c + n)
		{
			u = u_.data();
			c = c_.data();
		}
		vector_instrument(const std::vector<U>& u, const std::vector<C>& c)
			: vector_instrument(u.size(), u.data(), c.data())
		{
			if (u_.size() != c_.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": cash flow times must equal the number of cash flows");
		}

		// insert(U, C) ...
	};

	//!!! put in another file
	enum frequency { 
		NONE = 0,
		ANNUAL = 1,
		SEMIANNUAL = 2,
		QUARTERLY = 4,
		MONTHLY = 12
	};

	template<class U = double, class C = double>
	struct bond : public vector_instrument<U,C> {
		bond(U maturity = 0, frequency freq = NONE, C coupon = 0)
			: vector_instrument{static_cast<size_t>(ceil(freq*maturity))}
		{
			// fill backwards from maturity
			U i = 0;
			std::generate(u_.rbegin(), u_.rend(), [&]() { return maturity - i++/freq; });
			std::fill(c_.begin(), c_.end(), coupon/freq);

			c_.back() += 1; // plus unit notional at maturity
		}
	};

} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_instrument()
{
	using namespace fms;

	{ //!!!test fms::instrument
	}

	{ //!!!test fms::vector_instrument
	}

	{
		bond<> b(3, SEMIANNUAL, 0.05);
		assert (b.m == 6);
		for (size_t i = 0; i < b.m; ++i) {
			assert (b.u[i] == 0.5 + 1.*i/SEMIANNUAL);
			assert (b.c[i] == (i < b.m - 1 ? 0.05/2 : 1 + .05/2));
		}
	}
	{ // short first coupon
		bond<> b(3.25, SEMIANNUAL, 0.05);
		assert (b.m == 7);
		for (size_t i = 0; i < b.m; ++i) {
			assert (b.u[i] == 0.25 + 1.*i/SEMIANNUAL);
			assert (b.c[i] == (i < b.m - 1 ? 0.05/2 : 1 + .05/2));
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