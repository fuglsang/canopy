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

template <typename R, typename C, typename... P>
void resolve_arg(R(C::*)(P...))
{
	CA_LOG("member function pointer");
}

template <typename R, typename... P>
void resolve_arg(R(*)(P...))
{
	CA_LOG("free function pointer");
}

template <typename C>
void resolve_arg(C *)
{
	CA_LOG("class pointer");
}

void main(int argc, char** argv)
{
	vec_t<f32, 2> a = { 1.0f,2.0f };
	vec_t<f32, 2> b = { 2.0f,2.0f };

	auto v = a + b * b / a - b;

	CA_LOG("%f,%f", v.x, v.y);

	blah_t * bla = new blah_t();

	struct functor_t
	{
		void operator() ()
		{
			CA_LOG("functor");
		}
	};

	functor_t fobj;

	resolve_arg(&blah_t::test3);
	resolve_arg(&test2);
	resolve_arg(&fobj);



	auto f = CA_DELEGATE(&test2);
	auto g = CA_DELEGATE(&blah_t::test3, bla);
	auto h = CA_DELEGATE(&test4);
	auto l = CA_DELEGATE(&decltype(fobj)::operator(), &fobj);

	f();
	g();
	h(2);
	int harg = 77;
	h(harg);

	fmat4_t A, B;
	fmat4_t M = A + B;
	fvec4_t r;

	fmat3_t rot;
	fquat_t q;
	set_identity(q);
	set_rotation_by_direction_change(q, { 1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	set_rotation_by_euler_angles(q, { 0.0f,math::pi,math::tau });

	set_rotation_by_angle_x(rot, math::pi_2);
	set_rotation_by_angle_y(rot, math::pi_2);
	set_rotation_by_angle_z(rot, math::pi_2);
	set_rotation_by_axis_angle(rot, { 1.0f,0.0f,0.0f }, math::pi_2);
	set_rotation_by_quaternion(rot, q);
	set_rotation_by_look_direction(rot, { 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f });
	
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
		sys::windoweventhandler_t eventhandler;
		auto eventhandler_anon = [](sys::window_t * window, sys::windowevent msg)
		{
			CA_LOG("window %p sends %d", window, msg);
		};
		core::create_eventhandler(&eventhandler, &window.event, CA_DELEGATE_ANON(&eventhandler_anon));

		size_t heap_size = 128 * 1024 * 1024;
		void * heap_base = sys::heap_alloc(heap_size);
		{
			mem::heaparena_t heap;
			mem::create_arena(&heap, heap_base, heap_size);

			gfx::device_t device;
			gfx::create_device(&device, &heap);

			char const * vs_source =
				"#version 450 core\n"
				"layout(location = 0) in vec2 aVertex;\n"
				"layout(location = 1) in vec4 aColor;\n"
				"out vec4 vColor;\n"
				"void main()\n"
				"{\n"
				"    vColor = aColor;\n"
				"    gl_Position = vec4(aVertex, 0, 1);\n"
				"}\n";

			gfx::shader_t vs;
			gfx::create_shader(&vs, &device, vs_source, sizeof(vs_source));

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
				if ((step++ % 60) == 0)
				{
					CA_LOG("time = %f", sys::clockf());
				}

				gfx::swapchain_acquire(&swapchain, &backbuffer_acquired, nullptr, &backbuffer);

				gfx::renderattachment_t attachments[1] = {
					&backbuffer, gfx::RENDERLOADOP_CLEAR, gfx::RENDERSTOREOP_STORE, { 0.5f, 0.5f, 0.5f, 0.5f }, 1.0f, 0,
				};

				gfx::renderpass_t renderpass;
				gfx::create_renderpass(&renderpass, &device, attachments, 1);
				gfx::destroy_renderpass(&renderpass);

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
		sys::thread_sleep(200);
	}
	CA_LOG("%d END", w);
}