#include "ca/core.h"
#include "ca/math.h"
#include "ca/mem.h"
#include "ca/gfx.h"
#include "ca/sys.h"

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

void test4(int x)
{
	CA_LOG("hello from test4: x=%d", x);
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
	auto h = make_delegate<decltype(&test4), &test4>();

	f();
	g();
	h(2);
	int harg = 77;
	h(harg);

	mat4_t A, B;
	mat4_t M = A + B;
	vec4_t r;

	r = M * r;

	M += B;

	sys::reset_clock();

	int x = 0;
	while (x++ < 10)
	{
		CA_LOG("clock is %f", sys::clockf_micro());
		sys::thread_sleep(60);
	}

	sys::window_t window;
	sys::create_window(&window, "hello win32", { 1000, 50, 320, 200 });
	sys::window_show(&window);
	{
		sys::windowlistener_t listener;
		auto listener_lambda = [](sys::window_t * window, sys::windowevent msg)
		{
			CA_LOG("window %p sends %d", window, msg);
		};
		auto listener_action = make_delegate(&listener_lambda);
		core::create_eventlistener(&listener, listener_action, &window.event);

		size_t heap_size = 128 * 1024 * 1024;
		void * heap_base = sys::heap_alloc(heap_size);
		{
			mem::heaparena_t heap;
			mem::create_arena(&heap, heap_base, heap_size);

			gfx::device_t device;
			gfx::create_device(&device, &heap);

			gfx::swapchain_t swapchain;
			gfx::create_swapchain(&swapchain, &device, &window, gfx::SWAPMODE_VSYNC);

			gfx::cmdpool_t cmdpool;
			gfx::create_cmdpool(&cmdpool, &device);

			gfx::cmdbuffer_t cmdbuffer;
			gfx::create_cmdbuffer(&cmdbuffer, &cmdpool);

			gfx::texture_t backbuffer;
			gfx::semaphore_t backbuffer_acquired;
			gfx::semaphore_t backbuffer_presentable;

			gfx::create_semaphore(&backbuffer_acquired, &device);
			gfx::create_semaphore(&backbuffer_presentable, &device);

			u32 step = 0;

			while (sys::window_poll(&window))
			{
				++step;
				if ((step % 600) == 0)
				{
					CA_LOG("time = %f", sys::clockf());
				}

				gfx::swapchain_acquire(&swapchain, &backbuffer_acquired, nullptr, &backbuffer);

				f32 s = math::tau * sys::clockf();
				f32 k = math::sin(s) * 0.5f + 0.5f;

				gfx::cmdbuffer_reset(&cmdbuffer);
				gfx::cmdbuffer_begin(&cmdbuffer);
				gfx::cmdbuffer_clear_color(&cmdbuffer, &backbuffer, { 0.5f * k, 0.0f, k, 0.0f });
				gfx::cmdbuffer_end(&cmdbuffer);

				gfx::device_submit(&device, &cmdbuffer, &backbuffer_acquired, &backbuffer_presentable, nullptr);

				gfx::swapchain_present(&swapchain, &backbuffer_presentable);
			}

			gfx::destroy_swapchain(&swapchain);
			gfx::destroy_device(&device);
		}
		sys::heap_free(heap_base);
	}
	sys::destroy_window(&window);

	int w = 3;
	while (w > 0)
	{
		CA_LOG("%d", w--);
		sys::thread_sleep(1000);
	}
	CA_LOG("%d END", w);
}