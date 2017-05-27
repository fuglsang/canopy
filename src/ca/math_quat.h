#pragma once

#include "ca/types.h"
#include "ca/math_util.h"
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
				vec_t<T, 3> xyz;
				vec_t<T, 4> xyzw;
			};
		};

		typedef quat_t<f32> fquat_t;
		typedef quat_t<f64> dquat_t;

		//--------------------
		// operator overloads

		template <typename T>
		inline quat_t<T> operator* (quat_t<T> const & a, quat_t<T> const & b);
		
		template <typename T>
		inline vec_t<T, 3> operator* (quat_t<T> const & a, vec_t<T, 3> const & b);

		//-------------------
		// library functions

		template <typename T>
		inline void normalize(quat_t<T> & q);

		template <typename T>
		inline quat_t<T> normalize_copy_of(quat_t<T> const & q);

		template <typename T>
		inline quat_t<T> slerp(quat_t<T> const & q0, quat_t<T> const & q1, T t);

		template <typename T>
		inline void set_identity(quat_t<T> & q);

		template <typename T>
		inline void set_rotation_by_axis_angle(quat_t<T> & q, vec_t<T, 3> const & axis, T theta);

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, vec_t<T, 3> theta_xyz);

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, f32 theta_x, f32 theta_y, f32 theta_z);

		template <typename T, u32 N>
		inline void set_rotation_by_direction_change(quat_t<T> & q, vec_t<T, N> const & v0, vec_t<T, N> const & v1);
	}
}

#include "ca/math_quat.inl"