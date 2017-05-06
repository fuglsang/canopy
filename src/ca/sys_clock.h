#pragma once

#include "ca/types.h"

namespace ca
{
	namespace sys
	{
		void reset_clock();

		u64 clock();
		u64 clock_milli();
		u64 clock_micro();

		f32 clockf();
		f32 clockf_milli();
		f32 clockf_micro();

		f64 clockd();
		f64 clockd_milli();
		f64 clockd_micro();
	}
}