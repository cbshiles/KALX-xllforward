// xllforward.cpp - forward curves
#include "xllforward.h"

using namespace xll;

#ifdef _DEBUG

XLL_TEST_BEGIN(xll_forward_test)

	test_fms_newton();
	test_fms_pwflat();
	test_fms_pwflat_forward();
	test_fms_bootstrap();

XLL_TEST_END(xll_forward_test)

#endif // _DEBUG