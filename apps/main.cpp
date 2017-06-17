#include "ca/core.h"
#include "ca/math.h"
#include "ca/mem.h"
#include "ca/mem_chunkarena.h"
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

	size_t reserved_heap = 1024 * 1024 * 128;
	size_t reserved_stack_per_thread = 1024 * 1024 * 1;

	mem::initialize(reserved_heap, reserved_stack_per_thread);

	mem::chunkarena_t<80, 32> chunkarena;
	mem::create_arena(&chunkarena, CA_APP_HEAP, 600);

	void * p0 = mem::arena_alloc(&chunkarena, 80, 32);
	void * p1 = mem::arena_alloc(&chunkarena, 80, 32);
	void * p2 = mem::arena_alloc(&chunkarena, 80, 32);
	void * p3 = mem::arena_alloc(&chunkarena, 80, 32);

	mem::arena_free(&chunkarena, p2);
	mem::arena_free(&chunkarena, p0);
	mem::arena_free(&chunkarena, p3);
	mem::arena_free(&chunkarena, p1);

	u32 * free = chunkarena.allocator.index_head;
	while (free != chunkarena.allocator.index_tail)
	{
		CA_LOG("free: %d", *(free++));
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

		mem::heaparena_t gfx_heap;
		mem::create_arena(&gfx_heap, CA_APP_HEAP, 1024 * 1024 * 32);

		gfx::device_t device;
		gfx::create_device(&device, &gfx_heap);

		char const vs_glsl[] =
			"#version 400\n"
			"void main() {"
			"	vec2 pos[3] = vec2[3](vec2(-0.4, 0.4), vec2(0.4, 0.4), vec2(0.0, -0.4));"
			"	gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);"
			"}";

		gfx::shader_t vs;
		gfx::create_shader(&vs, &device, gfx::SHADERSTAGE_VERTEX, vs_glsl, sizeof(vs_glsl));

		char const fs_glsl[] =
			"#version 400\n"
			"layout(location = 0) out vec4 out_Color;"
			"void main()"
			"{"
			"	out_Color = vec4(0.0, 0.4, 1.0, 1.0);"
			"}";

		gfx::shader_t fs;
		gfx::create_shader(&fs, &device, gfx::SHADERSTAGE_FRAGMENT, fs_glsl, sizeof(fs_glsl));

		gfx::swapchain_t swapchain;
		gfx::create_swapchain(&swapchain, &device, &window, gfx::SWAPMODE_VSYNC);

		gfx::cmdpool_t cmdpool;
		gfx::create_cmdpool(&cmdpool, &device);

		struct framedata_t
		{
			gfx::fence_t submitted;
			gfx::cmdbuffer_t cmdbuffer;
			gfx::framebuffer_t framebuffer;
		};

		u32 frame_index = 0;
		framedata_t * framedata = mem::arena_alloc<framedata_t>(CA_APP_HEAP, swapchain.length);
		gfx::semaphore_t * frame_acquired = mem::arena_alloc<gfx::semaphore_t>(CA_APP_HEAP, swapchain.length);
		gfx::semaphore_t * frame_presentable = mem::arena_alloc<gfx::semaphore_t>(CA_APP_HEAP, swapchain.length);

		for (u32 i = 0; i != swapchain.length; i++)
		{
			gfx::create_fence(&framedata[i].submitted, &device, true);
			gfx::create_cmdbuffer(&framedata[i].cmdbuffer, &cmdpool);
			gfx::rendertarget_t attachments[1] = {
				&swapchain.textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE, { 1.0f, 0.0f, 0.5f, 1.0f }, 1.0f, 0,
			};			
			gfx::create_framebuffer(&framedata[i].framebuffer, &device, attachments, 1);			
			gfx::create_semaphore(&frame_acquired[i], &device);
			gfx::create_semaphore(&frame_presentable[i], &device);
		}

		core::eventhandler_t<gfx::swapchain_t *> handle_swapchain_recreated;
		auto anon_recreate_framedata = [&framedata](gfx::swapchain_t * swapchain)
		{
			for (u32 i = 0; i != swapchain->length; i++)
			{
				gfx::destroy_framebuffer(&framedata[i].framebuffer);
				gfx::rendertarget_t attachments[1] = {
					&swapchain->textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE,{ 1.0f, 0.0f, 0.5f, 1.0f }, 1.0f, 0,
				};
				gfx::create_framebuffer(&framedata[i].framebuffer, swapchain->device, attachments, 1);
			}
		};
		core::create_eventhandler(&handle_swapchain_recreated, &swapchain.recreated, CA_DELEGATE_ANON(&anon_recreate_framedata));

		gfx::pipeline_t pipeline;
		gfx::shader_t pipeline_shaders[2] = { vs, fs };
		gfx::create_pipeline(&pipeline, &framedata[0].framebuffer, pipeline_shaders, 2);

		u32 acquired_count = 0;
		u32 acquired_index;

		while (sys::window_poll(&window))
		{
			if ((acquired_count++ % 60) == 0)
			{
				CA_LOG("time = %f", sys::clockf());
			}

			gfx::swapchain_acquire(&swapchain, &frame_acquired[frame_index], nullptr, &acquired_index);

			framedata_t * frame = &framedata[acquired_index];
			{
				f32 s = math::tau * sys::clockf();
				f32 k = math::sin(s) * 0.5f + 0.5f;

				gfx::fence_wait_signaled(&frame->submitted);
				gfx::fence_reset_signaled(&frame->submitted);

				gfx::cmdbuffer_reset(&frame->cmdbuffer);
				gfx::cmdbuffer_begin(&frame->cmdbuffer);

				gfx::cmdbuffer_begin_renderpass(&frame->cmdbuffer, &frame->framebuffer);

				gfx::cmdbuffer_bind_pipeline(&frame->cmdbuffer, &pipeline);
				gfx::cmdbuffer_set_viewport(&frame->cmdbuffer, 0.1f * math::cos(0.2f * s) * swapchain.width, 0.1f * math::sin(0.2f * s) * swapchain.height, swapchain.width, swapchain.height);
				gfx::cmdbuffer_set_scissor(&frame->cmdbuffer, 0, 0, swapchain.width, swapchain.height);

				gfx::cmdbuffer_draw(&frame->cmdbuffer, 0, 3);
				
				// ...
				
				gfx::cmdbuffer_end_renderpass(&frame->cmdbuffer);
				gfx::cmdbuffer_end(&frame->cmdbuffer);
			}

			gfx::device_submit(&device, &frame->cmdbuffer, &frame_acquired[frame_index], &frame_presentable[frame_index], &frame->submitted);
			gfx::swapchain_present(&swapchain, &frame_presentable[frame_index], acquired_index);

			frame_index += 1;
			frame_index %= swapchain.length;
		}

		gfx::device_flush(&device);

		gfx::destroy_pipeline(&pipeline);

		for (u32 i = 0; i != swapchain.length; i++)
		{
			gfx::destroy_semaphore(&frame_presentable[i]);
			gfx::destroy_semaphore(&frame_acquired[i]);
			gfx::destroy_framebuffer(&framedata[i].framebuffer);
			gfx::destroy_cmdbuffer(&framedata[i].cmdbuffer);
			gfx::destroy_fence(&framedata[i].submitted);
		}
		
		gfx::destroy_cmdpool(&cmdpool);
		gfx::destroy_swapchain(&swapchain);
		gfx::destroy_shader(&fs);
		gfx::destroy_shader(&vs);
		gfx::destroy_device(&device);
	}	
	sys::destroy_window(&window);
	
	mem::finalize();

	int w = 3;
	while (w > 0)
	{
		CA_LOG("%d", w--);
		sys::thread_sleep(1000);
	}
	CA_LOG("%d END", w);
}