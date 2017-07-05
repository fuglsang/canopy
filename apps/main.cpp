#include "ca/core.h"
#include "ca/math.h"
#include "ca/mem.h"
#include "ca/mem_chunkarena.h"
#include "ca/gfx.h"
#include "ca/sys.h"
#include "ca/gfx_vertexdecl.h"
#include "ca/gfx_shaderdecl.h"

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

	{
		fvec3_t axis1 = { 0.0f, 0.0f, 1.0f };
		fvec3_t axis2 = normalize_copy_of(fvec3_t{ 1.0f, 0.0f, 0.0f });

		fquat_t rotation_around_axis1;
		set_rotation_by_axis_angle(rotation_around_axis1, axis1, pi_4);

		axis2 = rotation_around_axis1 * axis2;

		CA_LOG("axis1 %f,%f,%f rot(axis2) %f,%f,%f", axis1.x, axis1.y, axis1.z, axis2.x, axis2.y, axis2.z);
	}

	sys::reset_clock();

	int x = 0;
	while (x++ < 10)
	{
		CA_LOG("clock is %f", sys::clockf_micro());
		sys::thread_sleep(60);
	}

	size_t reserved_heap = 1024 * 1024 * 4;
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
	sys::create_window(&window, "hello win32", { 1000, 50, 500, 500 });
	sys::window_show(&window);
	{
		sys::windoweventhandler_t eventhandler;
		auto eventhandler_anon = [](sys::window_t * window, sys::windowevent msg)
		{
			CA_LOG("window %p sends %d", window, msg);
		};
		core::create_eventhandler(&eventhandler, &window.event, CA_DELEGATE_ANON(&eventhandler_anon));

		mem::heaparena_t gfx_heap;
		mem::create_arena(&gfx_heap, CA_APP_HEAP, 1024 * 1024 * 2);

		gfx::device_t device;
		gfx::create_device(&device, &gfx_heap);

		char const vs_glsl[] =
			"#version 420\n"
			""
			"layout(location = 0) in vec2 in_position;"
			"layout(location = 1) in vec3 in_color;"
			"layout(location = 0) out vec3 out_color;"
			""
			"void main() {"
			"	gl_Position = vec4(in_position, 0.0, 1.0);"
			"	out_color = in_color;"
			"}";

		char const fs_glsl[] =
			"#version 420\n"
			""
			"layout(location = 0) in vec3 in_color;"
			"layout(location = 0) out vec4 out_color;"
			""
			"layout(set = 0, binding = 0) uniform UBO"
			"{"
			"	vec3 cb_color;"
			"};"
			""
			"void main()"
			"{"
			"	out_color = vec4(in_color * cb_color, 1.0);"
			"}";

		gfx::shader_t vs;
		gfx::create_shader(&vs, &device, gfx::SHADERSTAGE_VERTEX, vs_glsl, sizeof(vs_glsl));

		gfx::shader_t fs;
		gfx::create_shader(&fs, &device, gfx::SHADERSTAGE_FRAGMENT, fs_glsl, sizeof(fs_glsl));

		gfx::shaderdecl_t xdecl;
		gfx::declare_property(&xdecl, 0, gfx::SHADERPROP_UNIFORM_BUFFER, gfx::SHADERSTAGE_FRAGMENT);

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
				&swapchain.textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE, { 0.1f, 0.0f, 0.1f, 1.0f }, 1.0f, 0,
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
					&swapchain->textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE, { 0.1f, 0.0f, 0.1f, 1.0f }, 1.0f, 0,
				};
				gfx::create_framebuffer(&framedata[i].framebuffer, swapchain->device, attachments, 1);
			}
		};
		core::create_eventhandler(&handle_swapchain_recreated, &swapchain.recreated, CA_DELEGATE_ANON(&anon_recreate_framedata));

		struct global_t
		{
			fvec3_t color;
		};

		gfx::buffer_t ubuf;
		gfx::create_buffer(&ubuf, &device, gfx::BUFFERTYPE_UNIFORM, gfx::BUFFERMEMORYTYPE_MAPPABLE_COHERENT, sizeof(global_t));

		struct vertex_t
		{
			fvec2_t position;
			fvec3_t color;
		};

		gfx::buffer_t vbuf;
		gfx::create_buffer(&vbuf, &device, gfx::BUFFERTYPE_VERTEX, gfx::BUFFERMEMORYTYPE_MAPPABLE_COHERENT, sizeof(vertex_t) * 3 * 25);

		gfx::shader_t shaders[2] = { vs, fs };

		gfx::vertexdecl_t vdecl;
		gfx::declare_vertexbuffer(&vdecl, 0, sizeof(vertex_t));
		gfx::declare_vertexattrib(&vdecl, 0, &vertex_t::position);
		gfx::declare_vertexattrib(&vdecl, 1, &vertex_t::color);

		gfx::pipeline_t pipeline;
		gfx::create_pipeline(&pipeline, &framedata[0].framebuffer, shaders, 2, &xdecl, &vdecl);

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
				f32 s = math::tau * sys::clockf() * 0.1f;
				f32 k = math::sin(s) * 0.5f + 0.5f;

				gfx::fence_wait_signaled(&frame->submitted);
				gfx::fence_reset_signaled(&frame->submitted);

				gfx::cmdbuffer_reset(&frame->cmdbuffer);
				gfx::cmdbuffer_begin(&frame->cmdbuffer);
				gfx::cmdbuffer_begin_renderpass(&frame->cmdbuffer, &frame->framebuffer);

				gfx::cmdbuffer_bind_pipeline(&frame->cmdbuffer, &pipeline);
				gfx::cmdbuffer_set_viewport(&frame->cmdbuffer, 0, 0, swapchain.width, swapchain.height);
				gfx::cmdbuffer_set_scissor(&frame->cmdbuffer, 0, 0, swapchain.width, swapchain.height);
				
				gfx::cmdbuffer_bind_property(&frame->cmdbuffer, &pipeline, 0, &ubuf);
				{
					global_t * g = static_cast<global_t *>(gfx::buffer_map(&ubuf, 0, sizeof(global_t)));
					
					g->color = { k, 1.0f - k, 1.0f };
					
					gfx::buffer_unmap(&ubuf);
				}

				gfx::cmdbuffer_bind_vertexbuffer(&frame->cmdbuffer, &vbuf, 0);
				{
					vertex_t * v = static_cast<vertex_t *>(gfx::buffer_map(&vbuf, 0, sizeof(vertex_t) * 3 * 25));

					fmat2_t rot;
					set_rotation_by_angle(rot, s);

					fvec2_t stepx = { 2.0f / 6.0f, 0.0f };
					fvec2_t stepy = { 0.0f, 2.0f / 6.0f };
					fvec2_t step0 = fvec2_t{ -1.0f, -1.0f } + stepx + stepy;

					for (u32 y = 0; y != 5; y++)
					{
						for (u32 x = 0; x != 5; x++)
						{
							f32 scale = 2.0f / 10.0f + 0.05f * math::sin(f32(x + y) + s * 5.0f);

							fvec2_t offset = f32(x) * stepx + f32(y) * stepy + step0;

							v->position = rot * scale * fvec2_t{ -0.6f, -0.4f } + offset;
							v->color = { k * 1.0f, (1.0f - k) * 1.0f, 0.0f };
							v++;

							v->position = rot * scale * fvec2_t{ 0.6f, -0.4f } + offset;
							v->color = { 0.0f, k * 1.0f, (1.0f - k) * 1.0f };
							v++;

							v->position = rot * scale * fvec2_t{ 0.0f, 0.7f } + offset;
							v->color = { (1.0f - k) * 1.0f, 0.0f, k * 1.0f };
							v++;
						}
					}

					gfx::buffer_unmap(&vbuf);
				}
				gfx::cmdbuffer_draw(&frame->cmdbuffer, 0, 3 * 25);
				
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
		gfx::destroy_buffer(&vbuf);
		gfx::destroy_buffer(&ubuf);

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