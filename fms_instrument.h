// fms_instrument.h - instruments classes
#pragma once
#include <cmath>
#include <algorithm>
#include <vector>

namespace fms {

	// NOT a value type
	template<class U = double, class C = double>
	struct instrument_base {
		size_t m;
		const U* u; // times
		const C* c; // cash flows

		instrument_base(size_t m = 0, const U* u = nullptr, const C* c = nullptr)
			: m(m), u(u), c(c)
		{ }
		instrument_base(const instrument_base&) = delete;
		instrument_base& operator=(const instrument_base&) = delete;
		virtual ~instrument_base()
		{ }

		bool operator==(const instrument_base& i) const
		{
			return m == i.m 
				&& std::equal(u, u + m, i.u, i.u + i.m)
				&& std::equal(c, c + m, i.c, i.c + i.m);
		}
		bool operator!=(const instrument_base& i) const
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
	class vector_instrument : public instrument_base<U,C> {
	protected:
		std::vector<U> u_;
		std::vector<C> c_;
	public:
		vector_instrument(size_t m = 0)
			: instrument_base<U,C>(m), u_(m), c_(m)
		{
			// this must occur after u_, c_ created
			instrument_base<U,C>::u = u_.data();
			instrument_base<U,C>::c = c_.data();
		}
		vector_instrument(size_t m, const U* u, const C* c)
			: instrument_base<U,C>(m), u_(u, u + m), c_(c, c + m)
		{
			instrument_base<U,C>::u = u_.data();
			instrument_base<U,C>::c = c_.data();
		}
		vector_instrument(const std::vector<U>& u, const std::vector<C>& c)
			: vector_instrument(u.size(), u.data(), c.data())
		{
			if (u_.size() != c_.size())
				throw std::runtime_error(__FILE__ ": " __FUNCTION__ ": cash flow times must equal the number of cash flows");

			instrument_base<U,C>::u = u_.data();
			instrument_base<U,C>::c = c_.data();
		}
		vector_instrument(const vector_instrument& i)
			: vector_instrument(i.u_, i.c_)
		{ }
		vector_instrument& operator=(const vector_instrument& i)
		{
			if (this != &i) {
				u_ = i.u_;
				c_ = i.c_;
			}

			return *this;
		}
		virtual ~vector_instrument()
		{ }

		// insert(U, C) ...
	};

namespace instrument {

	enum frequency { 
		NONE = 0,
		ANNUAL = 1,
		SEMIANNUAL = 2,
		QUARTERLY = 4,
		MONTHLY = 12
	};

	// periodic coupons plus notional at maturity
	template<class U = double, class C = double>
	struct bond : public vector_instrument<U,C> {
		bond(U maturity = 0, frequency freq = NONE, C coupon = 0)
			: vector_instrument(static_cast<size_t>(ceil(freq*maturity)))
		{
			// fill backwards from maturity
			U i = 0;
			std::generate(u_.rbegin(), u_.rend(), [&]() { return maturity - i++/freq; });
			std::fill(c_.begin(), c_.end(), coupon/freq);

			c_.back() += 1; // plus unit notional at maturity
		}
	};

	// single cash flow 1 + r*t at u
	// initial price is usually 1
	template<class U = double, class C = double>
	struct cd : public vector_instrument<U,C> {
		cd(U maturity = 0, C coupon = 0)
			: vector_instrument(1)
		{
			u_[0] = maturity;
			c_[0] = 1 + coupon*maturity;
		}
	};
	// foward rate agreement with two cash flows: -1 at u and 1 + c(v-u) at v
	// initial price is usually 0
	template<class U = double, class C = double>
	struct fra : public vector_instrument<U,C> {
		// !!! implement here
	};
} // instrument
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_instrument()
{
	using namespace fms;
	using namespace instrument;

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
	{
		//!!! test fms::instrument::cd
	}
	{
		//!!! test fms::instrument::fra
	}
}

#endif // _DEBUG