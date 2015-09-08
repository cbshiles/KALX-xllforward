// fmsforward.h - portable forward curve
#pragma once
#include <vector>
#include "fmspwflat.h"

namespace fms {
namespace pwflat {

	template<class T, class F>
	class forward {
		std::vector<T> t;
		std::vector<F> f;
	public:
		forward(size_t n, const T* t, const X* x)
			: t(0,0), f(0,0)
		{ }

		F value(const T& u) const
		{
			return value(u, n, &t[0], &f[0]);
		}
		// function object
		F operator()(const T& u) const
		{
			return value(u);
		}

		F spot(const valueT& u) const
		{
			return 
	};

} // pwflat
} // fms
