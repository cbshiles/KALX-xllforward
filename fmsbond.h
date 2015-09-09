// fmsbond.h - simple bond class
#pragma once

namespace fms {
namespace instrument {

	enum frequency { 
		ANNUAL = 1,
		SEMIANNUAL = 2,
		QUARTERLY = 4,
		MONTHLY = 12
	};

	template<class T, class C>
	class bond {
		T tenor;
		frequency frequency;
	public:
	};

} // instrument
} // fms