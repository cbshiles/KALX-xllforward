// bachelier.cpp - The Bachelier model
#include "../xll8/xll/xll.h"

using namespace xll;

// In the Bachelier model of stock price as Brownain motion, the value of a call is
// E max{F_t - k, 0} = (f - k) N(d) + sigma sqrt(t) N'(d), where N is the normal cdf.
//!!! Implement a function for valuing the call option in the Bachelier model
inline double bachelier_call_value(double f, double sigma, double k, double t)
{
	return 0;
}

//!!! Implement an Excel add-in for this function called BACHELIER.CALL.VALUE.

#ifdef _DEBUG

XLL_TEST_BEGIN(xll_bachelier_test)

//!!! Implement tests for the bachelier_call_value function.

XLL_TEST_END(xll_bachelier_test)

#endif // _DEBUG

//!!! 10pts Extra credit - What is the formula for F_t?