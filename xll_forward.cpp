// xllforward.cpp - forward curves
#include "xll_forward.h"

using namespace xll;

static AddInX xai_pwflat_forward(
	FunctionX(XLL_HANDLEX, _T("?xll_pwflat_forward"), _T("XLL.PWFLAT.FORWARD"))
	.Arg(XLL_FPX, _T("times"), _T("is an array of increasing times."))
	.Arg(XLL_FPX, _T("forwards"), _T("is an array of forward values."))
	.Arg(XLL_DOUBLEX, _T("_extrapolate"), _T("is an optional forward value for extrapolating past the last time. Default is 0."))
	.Uncalced()
	.FunctionHelp(_T("Return a handle to a piecewise flat forward curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
HANDLEX WINAPI xll_pwflat_forward(xfpx* pt, xfpx* pf, double _f)
{
#pragma XLLEXPORT
	handlex h;

	try {
		ensure (size(*pt) == size(*pf));

		if (_f == 0)
			_f = std::numeric_limits<double>::quiet_NaN();

		if (size(*pt) == 1 && pt->array[0] == 0) {
			handle<fms::pwflat::forward<>> h_{new fms::pwflat::forward<>(0, nullptr, nullptr, _f)};
			h = h_.get();
		}
		else {
			handle<fms::pwflat::forward<>> h_{new fms::pwflat::forward<>(size(*pt), pt->array, pf->array, _f)};
			h = h_.get();
		}
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

static AddInX xai_pwflat_forward_times(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_times"), _T("XLL.PWFLAT.FORWARD.TIMES"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.FunctionHelp(_T("Returns the times of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
xfpx* WINAPI xll_pwflat_forward_times(HANDLEX h)
{
#pragma XLLEXPORT
	static FPX t;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		t.resize(1, static_cast<xword>(h_->n));
		std::copy(h_->t, h_->t + h_->n, t.begin());
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return t.get();
}

static AddInX xai_pwflat_forward_values(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_values"), _T("XLL.PWFLAT.FORWARD.VALUES"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.FunctionHelp(_T("Returns the forward values of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
xfpx* WINAPI xll_pwflat_forward_values(HANDLEX h)
{
#pragma XLLEXPORT
	static FPX f;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		f.resize(1, static_cast<xword>(h_->n));
		std::copy(h_->f, h_->f + h_->n, f.begin());
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return f.get();
}

static AddInX xai_pwflat_forward_extrapolate(
	FunctionX(XLL_HANDLEX, _T("?xll_pwflat_forward_extrapolate"), _T("XLL.PWFLAT.FORWARD.EXTRAPOLATE"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.Arg(XLL_DOUBLEX, _T("Forward"), _T("is the forward value to use for extrapolation."))
	.Uncalced()
	.FunctionHelp(_T("Returns a new curve with the extrapolated forward."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
HANDLEX WINAPI xll_pwflat_forward_extrapolate(HANDLEX f, double _f)
{
#pragma XLLEXPORT
	handlex h;

	try {
		handle<fms::pwflat::forward<>> _(f);
		handle<fms::pwflat::forward<>> f_(new fms::pwflat::forward<>(*_));

		f_->_f = _f; // new extrapolation value

		h = f_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return h;
}


static AddInX xai_pwflat_forward_value(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_value"), _T("XLL.PWFLAT.FORWARD.VALUE"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.Arg(XLL_FPX, _T("Times"), _T("is one or more times at which to value of the curve."))
	.FunctionHelp(_T("Returns forward the value of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
	);
xfpx* WINAPI xll_pwflat_forward_value(HANDLEX h, xfpx* pt)
{
#pragma XLLEXPORT
	static FPX f;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		f.resize(pt->rows, pt->columns);
		std::transform(begin(*pt), end(*pt), f.begin(), [h_](double t) { return (*h_)(t); });
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return f.get();
}

static AddInX xai_pwflat_forward_integral(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_integral"), _T("XLL.PWFLAT.FORWARD.INTEGRAL"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.Arg(XLL_FPX, _T("Times"), _T("is one or more times at which to value the integral of the curve."))
	.FunctionHelp(_T("Returns the integral of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
xfpx* WINAPI xll_pwflat_forward_integral(HANDLEX h, xfpx* pt)
{
#pragma XLLEXPORT
	static FPX f;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		f.resize(pt->rows, pt->columns);
		std::transform(begin(*pt), end(*pt), f.begin(), [h_](double t) { return h_->integral(t); });
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return f.get();
}

static AddInX xai_pwflat_forward_spot(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_spot"), _T("XLL.PWFLAT.FORWARD.SPOT"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.Arg(XLL_FPX, _T("Times"), _T("is one or more times at which to evaluate the spot value."))
	.FunctionHelp(_T("Returns the spot values of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
xfpx* WINAPI xll_pwflat_forward_spot(HANDLEX h, xfpx* pt)
{
#pragma XLLEXPORT
	static FPX f;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		f.resize(pt->rows, pt->columns);
		std::transform(begin(*pt), end(*pt), f.begin(), [h_](double t) { return h_->spot(t); });
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return f.get();
}

static AddInX xai_pwflat_forward_discount(
	FunctionX(XLL_FPX, _T("?xll_pwflat_forward_discount"), _T("XLL.PWFLAT.FORWARD.DISCOUNT"))
	.Arg(XLL_HANDLEX, _T("Curve"), _T("is a handle to a XLL.PWFLAT.FORWARD curve."))
	.Arg(XLL_FPX, _T("Times"), _T("is one or more times at which to evaluate the discount value."))
	.FunctionHelp(_T("Returns the discount values of a curve."))
	.Category(CATEGORY)
	.Documentation(_T(""))
	);
xfpx* WINAPI xll_pwflat_forward_discount(HANDLEX h, xfpx* pt)
{
#pragma XLLEXPORT
	static FPX f;

	try {
		handle<fms::pwflat::forward<>> h_(h);

		f.resize(pt->rows, pt->columns);
		std::transform(begin(*pt), end(*pt), f.begin(), [h_](double t) { return h_->discount(t); });
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return f.get();
}

static AddInX xai_pwflat_forward_next(
	FunctionX(XLL_HANDLEX, _T("?xll_pwflat_forward_next"), _T("XLL.PWFLAT.FORWARD.NEXT"))
	.Arg(XLL_HANDLEX, _T("curve"), _T("is a handle to a curve returned by XLL.PWFLAT.FORWARD."))
	.Arg(XLL_HANDLEX, _T("instrument"), _T("a handle to an instrument."))
	.Arg(XLL_DOUBLEX, _T("_price"), _T("is the price of the instrument. Default is 0."))
	.Uncalced()
	.FunctionHelp(_T("Return a new handle to a curve after an instrument is bootstrapped."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
HANDLEX WINAPI xll_pwflat_forward_next(HANDLEX f, HANDLEX i, double p)
{
#pragma XLLEXPORT
	handlex h;

	try {
		handle<fms::pwflat::forward<>> _(f);
		handle<fms::pwflat::forward<>> f_(new fms::pwflat::forward<>(*_));
		handle<fms::vector_instrument<>> i_(i);

		f_->next(*i_, p);
		h = f_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

static AddInX xai_pwflat_forward_present_value(
	FunctionX(XLL_HANDLEX, _T("?xll_pwflat_forward_present_value"), _T("XLL.PWFLAT.FORWARD.PRESENT.VALUE"))
	.Arg(XLL_HANDLEX, _T("curve"), _T("is a handle to a curve returned by XLL.PWFLAT.FORWARD."))
	.Arg(XLL_HANDLEX, _T("instrument"), _T("a handle to an instrument."))
	.FunctionHelp(_T("Return the present value of an instrument."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
double WINAPI xll_pwflat_forward_present_value(HANDLEX f, HANDLEX i)
{
#pragma XLLEXPORT
	doublex pv;

	try {
		handle<fms::pwflat::forward<>> f_(f);
		handle<fms::vector_instrument<>> i_(i);

		pv = fms::pwflat::present_value(*i_, *f_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return pv;
}

static AddInX xai_pwflat_forward_duration(
	FunctionX(XLL_HANDLEX, _T("?xll_pwflat_forward_duration"), _T("XLL.PWFLAT.FORWARD.DURATION"))
	.Arg(XLL_HANDLEX, _T("curve"), _T("is a handle to a curve returned by XLL.PWFLAT.FORWARD."))
	.Arg(XLL_HANDLEX, _T("instrument"), _T("a handle to an instrument."))
	.FunctionHelp(_T("Return the duration of an instrument."))
	.Category(CATEGORY)
	.Documentation(_T(""))
	);
double WINAPI xll_pwflat_forward_duration(HANDLEX f, HANDLEX i)
{
#pragma XLLEXPORT
	doublex dur;

	try {
		handle<fms::pwflat::forward<>> f_(f);
		handle<fms::vector_instrument<>> i_(i);

		dur = fms::pwflat::duration(*i_, *f_);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return dur;
}

#ifdef _DEBUG
#include "fms_lmm.h"

XLL_TEST_BEGIN(xll_forward_test)
//_crtBreakAlloc = 2169;
	test_fms_newton();

	test_fms_pwflat();
	test_fms_bootstrap();
	test_fms_curve();
	test_fms_instrument();
	test_fms_forward();
	test_fms_pwflat_lmm();

//	test_fms_lmm();

XLL_TEST_END(xll_forward_test)

#endif // _DEBUG