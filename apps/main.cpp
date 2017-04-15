#include "ca/math_vec.h"
#include "ca/math_mat.h"
#include "ca/math_util.h"
#include "ca/math_bezier.h"
#include "ca/core_delegate.h"
#include "ca/core_log.h"
#include "ca/sys_clock.h"
#include "ca/sys_trap.h"

using namespace ca;
using namespace ca::core;
using namespace ca::math;

struct blah_t
{
	void test3()
	{
		CA_LOG("hello from test3");
	}
};

void test2()
{
	CA_LOG("hello from test2");
}

void main(int argc, char** argv)
{
	vec_t<f32, 2> a = { 1.0f,2.0f };
	vec_t<f32, 2> b = { 2.0f,2.0f };

	auto v = a + b * b / a - b;

	CA_LOG("%f,%f", v.x, v.y);

	blah_t * bla = new blah_t();

	auto f = make_delegate<decltype(&test2), &test2>();
	auto g = make_delegate<decltype(&blah_t::test3), &blah_t::test3>(bla);

	f();

	mat4_t A, B;
	mat4_t M = A + B;
	vec4_t r;

	r = M * r;

	M += B;

	int x = 0;
	while (x++ < 10)
	{
		CA_LOG("clock is %f\n", (f32)sys::clock_micro());
	}

	getchar();

	//ca::core::delegate_t<void()> d = ca::core::delegate_bind<decltype(test2), test2>();
}