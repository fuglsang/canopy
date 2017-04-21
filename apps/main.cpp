#include "ca/gfx_device.h"
#include "ca/math_vec.h"
#include "ca/math_mat.h"
#include "ca/math_util.h"
#include "ca/math_bezier.h"
#include "ca/core_delegate.h"
#include "ca/core_log.h"
#include "ca/sys_clock.h"
#include "ca/sys_trap.h"
#include "ca/sys_heap.h"
#include "ca/sys_thread.h"
#include "ca/sys_window.h"

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
	g();

	mat4_t A, B;
	mat4_t M = A + B;
	vec4_t r;

	r = M * r;

	M += B;

	int x = 0;
	while (x++ < 10)
	{
		CA_LOG("clock is %f", (f32)sys::clock_micro());
		sys::thread_sleep(60);
	}

	sys::window_t window;
	sys::create_window(&window, "hello win32", { 50, 50, 320, 200 });
	sys::window_show(&window);
	sys::window_move(&window, { 300,50,400,400 });

	while (sys::window_poll_blocking(&window))
	{
		CA_LOG("pos %d, %d, dim %d, %d", window.coords.x, window.coords.y, window.coords.dx, window.coords.dy);
	}

	sys::destroy_window(&window);
	CA_LOG("destroyed window");

	size_t heap_size = 128 * 1024 * 1024;
	void * heap_base = sys::heap_alloc(heap_size);
	{
		mem::heaparena_t heap;
		mem::create_arena(&heap, heap_base, heap_size);

		gfx::device_t device;
		gfx::create_device(&device, &heap);

		gfx::destroy_device(&device);
	}
	sys::heap_free(heap_base);

	getchar();

	//ca::core::delegate_t<void()> d = ca::core::delegate_bind<decltype(test2), test2>();
}