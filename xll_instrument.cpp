// xllinstrument.cpp - instruments
#include "xll_forward.h"

using namespace xll;

static AddInX xai_instrument(
	FunctionX(XLL_HANDLEX, _T("?xll_instrument"), _T("XLL.INSTRUMENT"))
	.Arg(XLL_FPX, _T("times"), _T("is an array of cash flow times."))
	.Arg(XLL_FPX, _T("cash_flows"), _T("is an array of cash flow amounts."))
	.Uncalced()
	.FunctionHelp(_T("Return a handle to a fixed cash flow instrument."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
HANDLEX WINAPI xll_instrument(xfpx* pu, xfpx* pc)
{
#pragma XLLEXPORT
	handlex h;

	try {
		ensure (size(*pu) == size(*pc));
		handle<fms::vector_instrument<>> h_{new fms::vector_instrument<>{size(*pu), pu->array, pc->array}};

		h = h_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

static AddInX xai_instrument_times(
	FunctionX(XLL_FPX, _T("?xll_instrument_times"), _T("XLL.INSTRUMENT.TIMES"))
	.Arg(XLL_HANDLEX, _T("instrument"), _T("is a handle to an instrument."))
	.FunctionHelp(_T("Return the times of a fixed cash flow instrument."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
xfpx* WINAPI xll_instrument_times(HANDLEX i)
{
#pragma XLLEXPORT
	static FPX u;

	try {
		handle<fms::vector_instrument<>> i_(i);

		u.resize(1, static_cast<xword>(i_->m));
		std::copy(i_->u, i_->u + i_->m, u.begin());
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return u.get();
}

static AddInX xai_instrument_cash_flows(
	FunctionX(XLL_FPX, _T("?xll_instrument_cash_flows"), _T("XLL.INSTRUMENT.CASH.FLOWS"))
	.Arg(XLL_HANDLEX, _T("instrument"), _T("is a handle to an instrument."))
	.FunctionHelp(_T("Return the cash flows of a fixed cash flow instrument."))
	.Category(CATEGORY)
	.Documentation(_T(""))
	);
xfpx* WINAPI xll_instrument_cash_flows(HANDLEX i)
{
#pragma XLLEXPORT
	static FPX c;

	try {
		handle<fms::vector_instrument<>> i_(i);

		c.resize(1, static_cast<xword>(i_->m));
		std::copy(i_->c, i_->c + i_->m, c.begin());
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return c.get();
}

static AddInX xai_instrument_bond(
	FunctionX(XLL_HANDLEX, _T("?xll_instrument_bond"), _T("XLL.INSTRUMENT.BOND"))
	.Arg(XLL_DOUBLEX, _T("maturity"), _T("is the maturity in years of the bond."))
	.Arg(XLL_WORDX, _T("frequency"), _T("is the number of coupons per year."))
	.Arg(XLL_DOUBLEX, _T("coupon"), _T("is coupon paid by the bond."))
	.Uncalced()
	.FunctionHelp(_T("Return a handle to a bond."))
	.Category(CATEGORY)
	.Documentation(_T(""))
);
HANDLEX WINAPI xll_instrument_bond(double maturity, fms::instrument::frequency freq, double coupon)
{
#pragma XLLEXPORT
	handlex h;

	try {
		handle<fms::vector_instrument<>> h_{new fms::instrument::bond<>(maturity, freq, coupon)};

		h = h_.get();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}

	return h;
}

// !!! implement xll_instrument_cd

// !!! implement xll_instrument_fra
