#pragma once

#include "ca_base.h"

namespace ca
{
	namespace math
	{
		const f32 pi = 3.14159265358979323846f;
		const f32 pi_2 = 1.57079632679489661923f;
		const f32 pi_4 = 0.785398163397448309616f;

		const f32 tau = 2.0f * pi;

		const f32 rcp_pi = 1.0f / pi;
		const f32 rcp_tau = 1.0f / tau;

		const f32 rad_deg = tau / 360.0f;
		const f32 deg_rad = 360.0f / tau;
	}
}