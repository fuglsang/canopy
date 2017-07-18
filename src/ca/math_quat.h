#pragma once

#include "ca/types.h"
#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		template <typename T>
		struct quat_t
		{
			union
			{
				struct { T x, y, z, w; };
				vec3_t<T> xyz;
				vec4_t<T> xyzw;
			};
		};

		typedef quat_t<f32> fquat_t;
		typedef quat_t<f64> dquat_t;

		//--------------------
		// operator overloads

		template <typename T>
		inline quat_t<T> operator* (quat_t<T> const & a, quat_t<T> const & b);
		
		template <typename T>
		inline vec3_t<T> operator* (quat_t<T> const & q, vec3_t<T> const & v);

		//-------------------
		// library functions

		template <typename T>
		inline quat_t<T> normalize(quat_t<T> const & q);

		template <typename T>
		inline quat_t<T> & normalize_in_place(quat_t<T> & q);

		template <typename T>
		inline quat_t<T> slerp(quat_t<T> const & q0, quat_t<T> const & q1, T t);

		//-------------------
		// utility functions

		template <typename T>
		inline void set_identity(quat_t<T> & q);

		template <typename T>
		inline void set_rotation_by_axis_angle(quat_t<T> & q, vec3_t<T> const & axis, T theta);

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, vec3_t<T> theta_xyz);

		template <typename T, u32 N>
		inline void set_rotation_by_from_to_direction(quat_t<T> & q, vec3_t<T> const & v0, vec3_t<T> const & v1);
	}
}

#include "ca/math_quat.inl"