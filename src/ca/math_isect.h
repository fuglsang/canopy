#pragma once

#include "ca/math_ray.h"
#include "ca/math_aabb.h"

namespace ca
{
	namespace math
	{
		template <typename T, u32 N>
		bool ray_aabb(ray_t<vec_t<T, N>> const & ray, aabb_t<vec_t<T, N>> const & aabb, T * t)
		{
			// based on method by williams et al.
			// see: http://www.cs.utah.edu/~awilliam/box/box.pdf
			T tmin, tmax, tymin, tymax;
			vec_t<T, N> inv_dir = rcp(ray.direction);
			
			if (inv_dir.e[0] >= 0)
			{
				tmin = (aabb.min.e[0] - ray.origin.e[0]) * inv_dir.e[0];
				tmax = (aabb.max.e[0] - ray.origin.e[0]) * inv_dir.e[0];
			}
			else
			{
				tmin = (aabb.max.e[0] - ray.origin.e[0]) * inv_dir.e[0];
				tmax = (aabb.min.e[0] - ray.origin.e[0]) * inv_dir.e[0];
			}

			for (u32 i = 1; i != N; i++)
			{
				if (inv_dir.e[i] >= 0)
				{
					tymin = (aabb.min.e[i] - ray.origin.e[i]) * inv_dir.e[i];
					tymax = (aabb.max.e[i] - ray.origin.e[i]) * inv_dir.e[i];
				}
				else
				{
					tymin = (aabb.max.e[i] - ray.origin.e[i]) * inv_dir.e[i];
					tymax = (aabb.min.e[i] - ray.origin.e[i]) * inv_dir.e[i];
				}

				if ((tmin > tymax) || (tymin > tmax))
					return false;// missed

				if (tmin < tymin)
					tmin = tymin;
				if (tmax > tymax)
					tmax = tymax;
			}

			if (tmax <= 0)
				return false;// missed

			*t = tmin;
			return true;// hit
		}

		/* non-generic version
		template <typename T>
		bool ray3_aabb3(ray_t<vec3_t<T>> const & ray, aabb_t<vec3_t<T>> const & aabb, T * t)
		{
			// based on method by williams et al.
			// see: http://www.cs.utah.edu/~awilliam/box/box.pdf
			T tmin, tmax, tymin, tymax, tzmin, tzmax;
			vec3_t<T> inv_dir = rcp(ray.direction);

			if (inv_dir.x >= 0)
			{
				tmin = (aabb.min.x - ray.origin.x) * inv_dir.x;
				tmax = (aabb.max.x - ray.origin.x) * inv_dir.x;
			}
			else
			{
				tmin = (aabb.max.x - ray.origin.x) * inv_dir.x;
				tmax = (aabb.min.x - ray.origin.x) * inv_dir.x;
			}

			if (inv_dir.y >= 0)
			{
				tymin = (aabb.min.y - ray.origin.y) * inv_dir.y;
				tymax = (aabb.max.y - ray.origin.y) * inv_dir.y;
			}
			else
			{
				tymin = (aabb.max.y - ray.origin.y) * inv_dir.y;
				tymax = (aabb.min.y - ray.origin.y) * inv_dir.y;
			}
			
			if ((tmin > tymax) || (tymin > tmax))
				return false;// missed
			
			if (tmin < tymin)
				tmin = tymin;
			if (tmax > tymax)
				tmax = tymax;

			if (inv_dir.z >= 0)
			{
				tzmin = (aabb.min.z - ray.origin.z) * inv_dir.z;
				tzmax = (aabb.max.z - ray.origin.z) * inv_dir.z;
			}
			else
			{
				tzmin = (aabb.max.z - ray.origin.z) * inv_dir.z;
				tzmax = (aabb.min.z - ray.origin.z) * inv_dir.z;
			}

			if ((tmin > tzmax) || (tzmin > tmax))
				return false;// missed

			if (tmin < tzmin)
				tmin = tzmin;
			if (tmax > tzmax)
				tmax = tzmax;

			if (tmax <= 0)
				return false;// missed

			*t = tmin;
			return true;// hit
		}
		*/
	}
}