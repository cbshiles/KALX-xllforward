// fms_lmm.h - LIBOR market model
/*
Use t, phi just like t, f for pwflat forward curves.
phi[0] is the stub from [0,t[0]] - it is fixed
phi[i] is the futures over (t[i-1],t[i]] for i > 0
*/
#pragma once
#include <cmath>
#include <random>
#include "fms_forward.h"

namespace fms {
namespace pwflat {

	template<class T = double, class F = double>
	class lmm {
		T s0; // current calendar time
		F gamma_; // convexity - default gamma_*5^2 = 5bps
		std::vector<T> t; // forward times
		std::vector<F> phi; // initial stub followed by futures
		std::vector<F> sigma; // atm forward vols
		std::vector<F> theta; // correlations
	public:
		lmm(const std::vector<T>& t, const std::vector<F>& phi, const std::vector<F>& sigma, const std::vector<F>& theta)
			: s0(0), gamma_(5e-4/25), t(t), phi(phi), sigma(sigma), theta(theta)
		{
			ensure (t.size() == phi.size());
			ensure (t.size() == sigma.size());
			ensure (t.size() == theta.size());
		}

		lmm(const lmm&) = default;
		lmm& operator=(const lmm&) = default;

		lmm(lmm&&) = default;
		lmm& operator=(lmm&&) = default;

		~lmm()
		{ }

		// set gamma
		void gamma(const F& g)
		{
			gamma_ = g;
		}
		// convexity at time t, difference between futures and forward
		F convexity(const T& t) const
		{
			return gamma_*t*t;
		}

		// evolve the curve forward in calendar time
		lmm& advance(const T& s)
		{
			// curve already evolved to s0
			ensure (s > s0);
			T ds = s - s0;
			T sqrtds = sqrt(ds);

			//!!! find first j such that t[j] > s0;
			size_t j = 0; ///!!!<=== fix this

			for (size_t i = j; i < t.size(); ++i) {
				// correlated normal increments
				F dB = (cos(theta[i])*Z() + sin(theta[i])*Z())*sqrtds;
				phi[i] *= exp(-sigma[i]*sigma[i]*ds/2 + sigma[i]*dB); 
			}

			return *this;
		}

		// forward curve at current calendar time
		vector_curve<T,F> curve() const
		{
			//!!! find first j such that t[j] > s0;

			//!!! vector with times t_[0] = t[j] - s0, t_[1] = t[j+1] - s0, ...
			std::vector<T> t_;

			//!!! forwards with f[0] = phi[j] - convexity(t_[0]), ...
			std::vector<F> f;

			return vector_curve<T,F>(t_, f);
		}

	private:
		// standard normal
		static double Z()
		{
			static std::default_random_engine dre;
			static std::normal_distribution<F> nd;

			return nd(dre);
		}
	};

} // pwflat
} // fms

#ifdef _DEBUG
#include <cassert>

inline void test_fms_pwflat_lmm()
{
	{
		// test with 0-vol
		std::vector<double> t{1,2,3};
		std::vector<double> phi{.01,.02,.03};
		std::vector<double> sigma{0,0,0};
		std::vector<double> theta{0,0,0};

		fms::pwflat::lmm<> m(t, phi, sigma, theta);
		m.gamma(0); // no convexity
		auto c = m.curve();
		auto c0 = fms::pwflat::vector_curve<>(t,phi);
//		ensure (c == c0); //!!! make this work

		m.advance(1);
		//!!! show m.curve() == curve({0, 1}, {.02,.03})

		m.advance(2);
		//!!! show m.curve() == curve({1}, {0.03})
	}
}

#endif // _DEBUG