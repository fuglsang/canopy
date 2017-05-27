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
		inline vec_t<T, 3> operator* (quat_t<T> const & a, vec_t<T, 3> const & b)
		{
			// method by fabian giesen
			// see: http://mollyrocket.com/forums/viewtopic.php?t=833
			return (v + (T(2) * cross(q.xyz, cross(q.xyz, v) + q.w * v)));
		}

		//-------------------
		// library functions

		template <typename T>
		inline void normalize(quat_t<T> & q)
		{
			normalize(q.xyzw);
		}

		template <typename T>
		inline quat_t<T> normalize_copy_of(quat_t<T> const & q)
		{
			quat_t<T> copy = q;
			normalize(copy.xyzw);
			return copy;
		}

		template <typename T>
		inline quat_t<T> slerp(quat_t<T> const & q0, quat_t<T> const & q1, T t)
		{
			T dot = q0.w * q1.w + dot(q0.xyz, q1.xyz);
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

		template <typename T>
		inline void set_identity(quat_t<T> & q)
		{
			q.x = T(0);
			q.y = T(0);
			q.z = T(0);
			q.w = T(1);
		}

		template <typename T>
		inline void set_rotation_by_axis_angle(quat_t<T> & q, vec_t<T, 3> const & axis, T theta)
		{
			q.xyz = sin(theta / T(2)) * axis;
			q.w = cos(theta / T(2));
			normalize(q);
		}

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, vec_t<T, 3> theta_xyz)
		{
			T hz = theta_xyz.z / T(2);// roll
			T hx = theta_xyz.x / T(2);// pitch
			T hy = theta_xyz.y / T(2);// yaw

			T c1 = cos(hz); T s1 = sin(hz);
			T c2 = cos(hx); T s2 = sin(hx);
			T c3 = cos(hy); T s3 = sin(hy);

			q.x = (s1 * c2 * c3) - (c1 * s2 * s3);
			q.y = (c1 * s2 * c3) + (s1 * c2 * s3);
			q.z = (c1 * c2 * s3) - (s1 * s2 * c3);
			q.w = (c1 * c2 * c3) + (s1 * s2 * s3);

			normalize(q);
		}

		template <typename T>
		inline void set_rotation_by_euler_angles(quat_t<T> & q, f32 theta_x, f32 theta_y, f32 theta_z)
		{
			set_from_euler_angles(q, { theta_x, theta_y, theta_z });
		}

		template <typename T>
		inline void set_rotation_by_direction_change(quat_t<T> & q, vec_t<T, 3> const & v0, vec_t<T, 3> const & v1)
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
				normalize(q);
			}
		}
	}
}