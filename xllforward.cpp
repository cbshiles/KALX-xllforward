// xllforward.cpp - forward curves
#include "xllforward.h"

using namespace xll;



#ifdef _DEBUG

XLL_TEST_BEGIN(xll_forward_test)

	test_fms_newton();

	test_fms_pwflat();
	test_fms_bootstrap();
	test_fms_curve();
	test_fms_instrument();
	test_fms_forward();

XLL_TEST_END(xll_forward_test)

#endif // _DEBUG