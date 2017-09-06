#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		//--------------------
		// operator overloads

		template <typename T>
		inline quat_t<T> operator* (quat_t<T> const & a, quat_t<T> const & b)
		{
			quat_t<T> q;
			q.xyz = a.w * b.xyz + b.w * q.xyz + cross(a.xyz, b.xyz);
			q.w = a.w * a.w - dot(a.xyz, b.xyz);
			return q;
		}

		template <typename T>
		inline vec_t<T, 3> operator* (quat_t<T> const & q, vec_t<T, 3> const & v)
		{
			// method by fabian giesen
			// see: http://mollyrocket.com/forums/viewtopic.php?t=833
			return (v + (T(2) * cross(q.xyz, cross(q.xyz, v) + q.w * v)));
		}

		//-------------------
		// library functions

		template <typename T>
		inline quat_t<T> normalize(quat_t<T> const & q)
		{
			return normalize(q.xyzw);
		}

		template <typename T>
		inline quat_t<T> & normalize_in_place(quat_t<T> & q)
		{
			normalize_in_place(q.xyzw);
			return q;
		}

		template <typename T>
		inline quat_t<T> slerp(quat_t<T> const & q0, quat_t<T> const & q1, T t)
		{
			T dot = dot(q0.xyzw, q1.xyzw);
			if (dot < 0.99f && dot > -0.99f)
			{
				T theta = acos(dot);
				return (q0 * sin(theta * (1.0f - t)) + q1 * sin(theta * t)) / sin(theta);
			}
			else
			{
				return lerp(q0, q1, t);
			}
		}

		//-------------------
		// utility functions

		template <typename T>
		inline void set_identity(quat_t<T> & q)
		{
			q.x = T(0);
			q.y = T(0);
			q.z = T(0);
			q.w = T(1);
		}

		template <typename T>
		inline void set_rotation_by_axis_angle(quat_t<T> & q, vec3_t<T> const & axis, T theta)
		{
			q.xyz = sin(theta / T(2)) * axis;
			q.w = cos(theta / T(2));

			normalize_in_place(q);
		}

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, vec3_t<T> euler_xyz)
		{
			T hz = euler_xyz.z / T(2);// roll
			T hx = euler_xyz.x / T(2);// pitch
			T hy = euler_xyz.y / T(2);// yaw

			T c1 = cos(hz); T s1 = sin(hz);
			T c2 = cos(hx); T s2 = sin(hx);
			T c3 = cos(hy); T s3 = sin(hy);

			q.x = (s1 * c2 * c3) - (c1 * s2 * s3);
			q.y = (c1 * s2 * c3) + (s1 * c2 * s3);
			q.z = (c1 * c2 * s3) - (s1 * s2 * c3);
			q.w = (c1 * c2 * c3) + (s1 * s2 * s3);

			normalize_in_place(q);
		}

		template <typename T>
		inline void set_rotation_by_from_to_direction(quat_t<T> & q, vec3_t<T> const & v0, vec3_t<T> const & v1)
		{
			// method by stan melax
			// see: game programming gems 1
			T s = sqrt(T(2) + T(2) * dot(v0, v1));
			if (s < T(0.01f))
			{
				set_identity(q);
			}
			else
			{
				q.xyz = cross(v0, v1) / s;
				q.w = T(2) * s;

				normalize_in_place(q);
			}
		}

		// - - - -

		template <typename T>
		inline quat_t<T> quat_identity()
		{
			quat_t<T> q;
			set_identity(q);
			return q;
		}

		template <typename T>
		inline quat_t<T> quat_rotation(vec3_t<T> const & axis, T theta)
		{
			quat_t<T> q;
			set_rotation_by_axis_angle(q, axis, theta);
			return q;
		}

		template <typename T>
		inline quat_t<T> quat_rotation(vec3_t<T> euler_xyz)
		{
			quat_t<T> q;
			set_rotation_by_euler_angles(q, euler_xyz);
			return q;
		}

		template <typename T>
		inline quat_t<T> quat_rotation(vec3_t<T> const & v0, vec3_t<T> const & v1)
		{
			quat_t<T> q;
			set_rotation_by_from_to_direction(q, v0, v1);
			return q;
		}
	}
}