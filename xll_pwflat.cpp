#include "xll_forward.h"

using namespace xll;

static AddInX xai_pwflat_value(
	FunctionX(XLL_DOUBLEX, _T("?xll_pwflat_value"), _T("FMS.PWFLAT.VALUE"))
	.Arg(XLL_DOUBLEX, _T("t"), _T("is the time at which the forward is evaluated."))
	.Arg(XLL_FPX, _T("times"), _T("is an array of times."))
	.Arg(XLL_FPX, _T("forward"), _T("is an array of forwards"))
	.Arg(XLL_DOUBLEX, _T("_f"), _T("is an optional forward extrapolation value."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Return the piecewise flat forward at time t."))
	.Documentation()
);
double WINAPI xll_pwflat_value(double t, xfpx* pt, xfpx* pf, double _f)
{
#pragma XLLEXPORT
	doublex v;

	try {
		ensure (size(*pt) == size(*pf));
		if (_f == 0)
			_f = std::numeric_limits<double>::quiet_NaN();

		v = fms::pwflat::value(t, size(*pt), pt->array, pf->array, _f);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

static AddInX xai_pwflat_integral(
	FunctionX(XLL_DOUBLEX, _T("?xll_pwflat_integral"), _T("FMS.PWFLAT.INTEGRAL"))
	.Arg(XLL_DOUBLEX, _T("t"), _T("is the time at which the integral is evaluated."))
	.Arg(XLL_FPX, _T("times"), _T("is an array of times."))
	.Arg(XLL_FPX, _T("forward"), _T("is an array of forwards"))
	.Arg(XLL_DOUBLEX, _T("_f"), _T("is an optional forward extrapolation value."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Return the integral of the piecewise flat forward to time t."))
	.Documentation()
	);
double WINAPI xll_pwflat_integral(double t, xfpx* pt, xfpx* pf, double _f)
{
#pragma XLLEXPORT
	doublex v;

	try {
		ensure (size(*pt) == size(*pf));
		if (_f == 0)
			_f = std::numeric_limits<double>::quiet_NaN();

		v = fms::pwflat::integral(t, size(*pt), pt->array, pf->array, _f);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

static AddInX xai_pwflat_discount(
	FunctionX(XLL_DOUBLEX, _T("?xll_pwflat_discount"), _T("FMS.PWFLAT.DISCOUNT"))
	.Arg(XLL_DOUBLEX, _T("t"), _T("is the time at which the discount is evaluated."))
	.Arg(XLL_FPX, _T("times"), _T("is an array of times."))
	.Arg(XLL_FPX, _T("forward"), _T("is an array of forwards"))
	.Arg(XLL_DOUBLEX, _T("_f"), _T("is an optional forward extrapolation value."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Return the discount of the piecewise flat forward to time t."))
	.Documentation()
	);
double WINAPI xll_pwflat_discount(double t, xfpx* pt, xfpx* pf, double _f)
{
#pragma XLLEXPORT
	doublex v;

	try {
		ensure (size(*pt) == size(*pf));
		if (_f == 0)
			_f = std::numeric_limits<double>::quiet_NaN();

		v = fms::pwflat::discount(t, size(*pt), pt->array, pf->array, _f);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}

static AddInX xai_pwflat_spot(
	FunctionX(XLL_DOUBLEX, _T("?xll_pwflat_spot"), _T("FMS.PWFLAT.SPOT"))
	.Arg(XLL_DOUBLEX, _T("t"), _T("is the time at which the spot is evaluated."))
	.Arg(XLL_FPX, _T("times"), _T("is an array of times."))
	.Arg(XLL_FPX, _T("forward"), _T("is an array of forwards"))
	.Arg(XLL_DOUBLEX, _T("_f"), _T("is an optional forward extrapolation value."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Return the spot of the piecewise flat forward to time t."))
	.Documentation()
	);
double WINAPI xll_pwflat_spot(double t, xfpx* pt, xfpx* pf, double _f)
{
#pragma XLLEXPORT
	doublex v;

	try {
		ensure (size(*pt) == size(*pf));
		if (_f == 0)
			_f = std::numeric_limits<double>::quiet_NaN();

		v = fms::pwflat::spot(t, size(*pt), pt->array, pf->array, _f);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return v;
}
