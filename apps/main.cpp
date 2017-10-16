#include "ca/core.h"
#include "ca/math.h"
#include "ca/mem.h"
#include "ca/mem_chunkarena.h"
#include "ca/mem_stackarena.h"
#include "ca/gfx.h"
#include "ca/sys.h"
#include "ca/math_ray_isect.h"

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

void test4(int x, char c, int y)
{
	CA_LOG("hello from test4: x=%d, c=%d, y=%d", x, c, y);
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
	faabb2_t aabb = { { -1.0f, -1.0f }, { 1.0f, 1.0f } };
	fray2_t ray = { { -1.1f, 0.0f }, { 1.0f, 0.0f } };
	
	f32 ray_t = 0.0f;
	if (ray_isect_aabb(ray, aabb, &ray_t))
		CA_LOG("ray hit, t = %.3f", ray_t);
	else
		CA_LOG("ray missed");

	//while (true);

	fvec2_t a = { 1.0f,2.0f };
	fvec2_t b = { 2.0f,2.0f };

	auto v = a + b * b / a - b;
	auto vvvv = a < b;
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
	h(2, 'a', 3);
	int harg = 77;
	h(harg, 'b', 9);

	fmat4_t A, B;
	fmat4_t M = A + B;
	fvec4_t r;

	fmat4_t rot;
	fquat_t q;
	set_identity(q);
	set_rotation_by_from_to_direction(q, { 1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	set_rotation_by_euler_angles(q, { 0.0f,math::pi,math::tau });

	set_rotation_by_angle_x(rot, math::pi_2);
	set_rotation_by_angle_y(rot, math::pi_2);
	set_rotation_by_angle_z(rot, math::pi_2);
	set_rotation_by_axis_angle(rot, { 1.0f,0.0f,0.0f }, math::pi_2);
	set_rotation_by_quaternion(rot, q);
	set_rotation_by_forward_up(rot, { 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f });
	
	r = M * r;

	M += B;

	{
		fvec3_t axis1 = { 0.0f, 0.0f, 1.0f };
		fvec3_t axis2 = normalize(fvec3_t{ 1.0f, 0.0f, 0.0f });

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
	sys::create_window(&window, "hello vulkan", { 1000, 50, 550, 550 });
	//sys::create_window(&window, "hello vulkan", { 1000, 50, 400, 225 });
	sys::window_show(&window);
	{
		auto fn_log_windowevent = [](sys::window_t * window, sys::windowevent msg)
		{
			CA_LOG("window %p sends %d", window, msg);
		};
		core::eventhandler_t<sys::window_t *, sys::windowevent> on_window_event;
		core::create_eventhandler(&on_window_event, &window.event, CA_DELEGATE_ANON(&fn_log_windowevent));

		mem::heaparena_t gfx_heap;
		mem::create_arena(&gfx_heap, CA_APP_HEAP, 1024 * 1024 * 2);

		gfx::device_t device;
		gfx::create_device(&device, &gfx_heap);

		gfx::swapchain_t swapchain;
		gfx::create_swapchain(&swapchain, &device, &window, gfx::SWAPMODE_VSYNC);

		char const vs_glsl[] =
		R"glsl(
			#version 420

			layout(location = 0) in vec3 in_position;
			layout(location = 1) in vec3 in_color;
			layout(location = 0) out vec3 out_color;
			
			layout(set = 0, binding = 0) uniform cb
			{
				mat4 cb_view_projection;
				vec3 cb_color_tint;
			};

			void main()
			{
				gl_Position = cb_view_projection * vec4(in_position, 1.0);
				out_color = in_color * cb_color_tint;
			}
		)glsl";

		char const fs_glsl[] =
		R"glsl(
			#version 420

			layout(location = 0) in vec3 in_color;
			layout(location = 0) out vec4 out_color;
			
			void main()
			{
				out_color = vec4(in_color, 1.0);
			}
		)glsl";

		gfx::shader_t vs;
		gfx::create_shader(&vs, &device, gfx::SHADERSTAGE_VERTEX, vs_glsl, sizeof(vs_glsl));

		gfx::shader_t fs;
		gfx::create_shader(&fs, &device, gfx::SHADERSTAGE_FRAGMENT, fs_glsl, sizeof(fs_glsl));
		
		gfx::uniformsetdecl_t uniformsetdecl;
		gfx::uniformsetdecl_binding(&uniformsetdecl, 0, gfx::UNIFORMTYPE_CONSTANT_BUFFER, gfx::SHADERSTAGE_VERTEX);

		gfx::uniformsetlayout_t uniformsetlayout;
		gfx::create_uniformsetlayout(&uniformsetlayout, &device, &uniformsetdecl);
		
		gfx::uniformpool_t uniformpool;
		gfx::create_uniformpool(&uniformpool, &device, swapchain.length, swapchain.length);

		gfx::cmdpool_t cmdpool;
		gfx::create_cmdpool(&cmdpool, &device);

		struct vertex_t
		{
			fvec3_t position;
			fvec3_t color;
		};

		struct camera_t
		{
			fmat4_t view_projection;
			fvec3_t color_tint;
		};

		struct framedata_t
		{
			gfx::fence_t submitted;
			gfx::cmdbuffer_t cmdbuffer;
			gfx::buffer_t vertexbuffer;
			gfx::buffer_t uniformbuffer;
			gfx::uniformset_t uniformset;
			gfx::framebuffer_t framebuffer;
		};

		framedata_t * framedata = mem::arena_alloc<framedata_t>(CA_APP_HEAP, swapchain.length);
		gfx::semaphore_t * frame_acquired = mem::arena_alloc<gfx::semaphore_t>(CA_APP_HEAP, swapchain.length);
		gfx::semaphore_t * frame_presentable = mem::arena_alloc<gfx::semaphore_t>(CA_APP_HEAP, swapchain.length);

		u32 const max_vertices = 100000;

		for (u32 i = 0; i != swapchain.length; i++)
		{
			gfx::create_fence(&framedata[i].submitted, &device, true);
			gfx::create_cmdbuffer(&framedata[i].cmdbuffer, &cmdpool);
			gfx::create_buffer(&framedata[i].vertexbuffer, &device, gfx::BUFFERTYPE_VERTEX, gfx::BUFFERMEMORYTYPE_MAPPABLE_COHERENT, sizeof(vertex_t) * max_vertices);
			gfx::create_buffer(&framedata[i].uniformbuffer, &device, gfx::BUFFERTYPE_UNIFORM_CONSTANT, gfx::BUFFERMEMORYTYPE_MAPPABLE_COHERENT, sizeof(camera_t));
			gfx::create_uniformset(&framedata[i].uniformset, &uniformpool, &uniformsetlayout);

			gfx::rendertarget_t attachments[1] = {
				&swapchain.textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE, { 0.1f, 0.0f, 0.1f, 1.0f }, 1.0f, 0,
			};

			gfx::create_framebuffer(&framedata[i].framebuffer, &device, attachments, 1);
			gfx::create_semaphore(&frame_acquired[i], &device);
			gfx::create_semaphore(&frame_presentable[i], &device);
		}

		auto fn_recreate_framedata = [&framedata](gfx::swapchain_t * swapchain)
		{
			for (u32 i = 0; i != swapchain->length; i++)
			{
				gfx::rendertarget_t attachments[1] = {
					&swapchain->textures[i], gfx::RENDERTARGETLOADOP_CLEAR, gfx::RENDERTARGETSTOREOP_STORE, { 0.1f, 0.0f, 0.1f, 1.0f }, 1.0f, 0,
				};
				
				gfx::destroy_framebuffer(&framedata[i].framebuffer);
				gfx::create_framebuffer(&framedata[i].framebuffer, swapchain->device, attachments, 1);
			}
		};
		core::eventhandler_t<gfx::swapchain_t *> on_swapchain_recreated;
		core::create_eventhandler(&on_swapchain_recreated, &swapchain.recreated, CA_DELEGATE_ANON(&fn_recreate_framedata));

		gfx::vertexdecl_t vdecl;
		gfx::vertexdecl_buffer(&vdecl, 0, sizeof(vertex_t));
		gfx::vertexdecl_attrib(&vdecl, 0, &vertex_t::position);
		gfx::vertexdecl_attrib(&vdecl, 1, &vertex_t::color);

		gfx::shader_t shaders[2] = { vs, fs };

		gfx::pipeline_t pipeline;
		gfx::create_pipeline(&pipeline, &framedata[0].framebuffer, shaders, 2, &uniformsetlayout, &vdecl);

		u32 acquired_count = 0;
		u32 acquired_index = 0;
		u32 frame_index = 0;

		u32 point_dim = 32;

		while (sys::window_poll(&window))
		{
			if (window.keystate.key_down[input::KEY_A])
				CA_LOG("pressed 'a'");

			if (window.keystate.key_down[input::KEY_UP])
				point_dim <<= 1;
			if (window.keystate.key_down[input::KEY_DOWN])
				point_dim >>= 1;

			point_dim = clamp(point_dim, 4u, 128u);

			if (window.keystate.key_down[input::KEY_ESCAPE])
				break;

			if ((acquired_count++ % 60) == 0)
			{
				CA_LOG("time = %f", sys::clockf());
			}

			gfx::swapchain_acquire(&swapchain, &frame_acquired[frame_index], nullptr, &acquired_index);

			framedata_t * frame = &framedata[acquired_index];
			{
				f32 s = math::tau * sys::clockf() * 0.1f;
				f32 k = math::sin(s) * 0.5f + 0.5f;

				gfx::fence_wait_reset(&frame->submitted);

				gfx::cmdbuffer_reset(&frame->cmdbuffer);
				gfx::cmdbuffer_begin(&frame->cmdbuffer);
				gfx::cmdbuffer_begin_renderpass(&frame->cmdbuffer, &frame->framebuffer);

				gfx::cmdbuffer_bind_pipeline(&frame->cmdbuffer, &pipeline);
				gfx::cmdbuffer_set_viewport(&frame->cmdbuffer, 0, 0, swapchain.width, swapchain.height);
				gfx::cmdbuffer_set_scissor(&frame->cmdbuffer, 0, 0, swapchain.width, swapchain.height);
				
				gfx::uniformset_update_index(&frame->uniformset, 0, &frame->uniformbuffer);
				
				gfx::cmdbuffer_bind_uniformset(&frame->cmdbuffer, &pipeline, 0, &frame->uniformset);

				fvec3_t cam_position = { 0.0f, 1.0f, 1.0f };
				fvec3_t cam_forward = { 0.0f, 0.0f, 1.0f };
				fvec3_t cam_up = { 0.0f, 1.0f, 0.0f };

				u32 num_vertices = 0;

				gfx::cmdbuffer_bind_vertexbuffer(&frame->cmdbuffer, &frame->vertexbuffer, 0);
				{
					vertex_t * v = static_cast<vertex_t *>(gfx::buffer_map(&frame->vertexbuffer, 0, sizeof(vertex_t) * max_vertices));

					fbezierpatch3_t patch =
					{
						// p00--p01--p02--p03
						-1.5f,  3.0f, -1.5f,
						-0.5f,  0.0f, -1.5f,
						 0.5f,  0.0f, -1.5f,
						 1.5f,  3.0f, -1.5f,
						// p10--p11--p12--p13
						-1.5f,  0.0f, -0.5f,
						-0.5f,  1.0f, -0.5f,
						 0.5f, -1.0f, -0.5f,
						 1.5f,  3.0f, -0.5f,
						// p20--p21--p22--p23
						-1.5f,  0.0f,  0.5f,
						-0.5f,  1.0f,  0.5f,
						 0.5f, -1.0f,  0.5f,
						 1.5f,  2.0f,  0.5f,
						// p30--p31--p32--p33
						-1.5f,  0.0f,  1.5f,
						-0.5f,  0.0f,  1.5f,
						 0.5f,  0.0f,  1.5f,
						 1.5f,  0.0f,  1.5f,
					};

					fvec3_t ray_col = { 5.0f, 5.0f, 5.0f };
					fray3_t ray = {
						{ 1.25f*cos(0.25f*s), 5.0f, 1.25f*sin(0.25f*s) },// origin
						{ 0.0f, -1.0f, 0.0f },// direction
					};

					/*{
						v->color = ray_col;
						v->position = ray.origin;
						v++;
						v->color = ray_col;
						v->position = ray.origin + ray.direction * 0.3f;
						v++;
						num_vertices += 2;
					}*/

					//f32 isect_t;
					fvec2_t isect_st;

					if (ray_isect_bezierpatch(ray, patch, 50, &isect_st))
					{
						fvec3_t isect_x;
						fvec3_t isect_n;
						fvec3_t isect_vs;
						fvec3_t isect_vt;

						eval(patch, isect_st, &isect_x, &isect_vs, &isect_vt);
						isect_vs = normalize(isect_vs);
						isect_vt = normalize(isect_vt);
						isect_n = cross(isect_vs, isect_vt);

						cam_position = isect_x;
						cam_forward = -normalize(isect_x);
						cam_up = isect_n;

						fvec3_t cam_right = cross(cam_forward, cam_up);
						cam_forward = normalize(cross(cam_up, cam_right));

						//CA_LOG("ray.origin %.3f, %.3f HIT t = %.3f", ray.origin.x, ray.origin.z, isect_t);

						//v->color = ray_col;
						//v->position = isect_x;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x + 0.1f * isect_n;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x - 0.1f * isect_vs;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x + 0.1f * isect_vs;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x - 0.1f * isect_vt;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x + 0.1f * isect_vt;
						//v++;
						//num_vertices += 6;
					}

					fray3_t car_ray = ray;
					fvec2_t car_isect_st;
					car_ray.origin *= 0.8f;
					if (ray_isect_bezierpatch(car_ray, patch, 50, &car_isect_st))
					{
						fvec3_t isect_x;
						fvec3_t isect_n;
						fvec3_t isect_vs;
						fvec3_t isect_vt;

						eval(patch, car_isect_st, &isect_x, &isect_vs, &isect_vt);
						isect_vs = normalize(isect_vs);
						isect_vt = normalize(isect_vt);
						isect_n = cross(isect_vs, isect_vt);

						//v->color = ray_col;
						//v->position = isect_x;
						//v++;
						//v->color = ray_col;
						//v->position = isect_x + 0.1f * isect_n;
						//v++;

						//num_vertices += 2;
					}

					uvec2_t point_count = { point_dim, point_dim };
					fvec3_t * points = mem::arena_alloc<fvec3_t>(CA_APP_STACK, point_dim * point_dim);

					sample(patch, points, point_count);

					fvec3_t curve_color = { 1.0f, 1.0f, 1.0f };

					for (u32 i = 1; i != point_count.y; i++, num_vertices += 2)
					{
						v->color = curve_color;
						v->position = points[point_count.x * (i - 1)];
						v++;
						v->color = curve_color;
						v->position = points[point_count.x * (i)];
						v++;
					}

					for (u32 j = 1; j != point_count.x; j++, num_vertices += 2)
					{
						v->color = curve_color;
						v->position = points[j - 1];
						v++;
						v->color = curve_color;
						v->position = points[j];
						v++;
					}

					for (u32 i = 1; i != point_count.y; i++)
					{
						for (u32 j = 1; j != point_count.x; j++, num_vertices += 4)
						{
							v->color = curve_color;
							v->position = points[j + point_count.x * i - 1];
							v++;
							v->color = curve_color;
							v->position = points[j + point_count.x * i];
							v++;

							v->color = curve_color;
							v->position = points[j + point_count.x * (i - 1)];
							v++;
							v->color = curve_color;
							v->position = points[j + point_count.x * i];
							v++;
						}
					}

					mem::arena_free(CA_APP_STACK, points);

					//----------

					/*
					fmat3_t rot = mat3_rotation(s);
					fmat3_t inc = mat3_rotation(rad_deg * 2.0f);

					fvec3_t stepx = { 2.0f / 6.0f, 0.0f, 0.0f };
					fvec3_t stepy = { 0.0f, 2.0f / 6.0f, 0.0f };
					fvec3_t step0 = fvec3_t{ -1.0f, -1.0f, 0.0f } + stepx + stepy;

					for (u32 y = 0; y != 5; y++)
					{
						for (u32 x = 0; x != 5; x++, num_vertices += 4)
						{
							f32 scale = 2.0f / 10.0f + 0.05f * math::sin(f32(x + y) + s * 5.0f);

							fvec3_t offset = f32(x) * stepx + f32(y) * stepy + step0;

							v->position = rot * scale * fvec3_t{ -0.6f, -0.4f, 0.0f } + offset;
							v->color = { k * 1.0f, (1.0f - k) * 1.0f, 0.0f };
							v++;

							v->position = rot * scale * fvec3_t{ 0.6f, -0.4f, 0.0f } + offset;
							v->color = { 0.0f, k * 1.0f, (1.0f - k) * 1.0f };
							v++;

							v->position = rot * scale * fvec3_t{ 0.0f, 0.7f, 0.0f } + offset;
							v->color = { (1.0f - k) * 1.0f, 0.0f, k * 1.0f };
							v++;

							rot = inc * rot;
						}
					}
					*/

					gfx::buffer_unmap(&frame->vertexbuffer);
				}

				// update uniforms
				{
					camera_t * g = static_cast<camera_t *>(gfx::buffer_map(&frame->uniformbuffer, 0, sizeof(camera_t)));

					cam_position = cam_position + 0.1f * cam_up;
					
					f32 cam_aspect = f32(swapchain.width) / f32(swapchain.height);

					fmat4_t M_projection = mat4_scaling(fvec3_t{ 1.0f, -1.0f, 1.0f }) * mat4_perspective(rad_deg * 90.0f, cam_aspect, 0.01f, 1000.0f);
					fmat4_t M_view = mat4_look_at(cam_position, cam_position + cam_forward, cam_up);
					fmat4_t M_view_inv = inverse(M_view);

					g->view_projection = transpose(M_projection * M_view_inv);
					g->color_tint = { k, 1.0f - k, 1.0f };

					gfx::buffer_unmap(&frame->uniformbuffer);
				}

				gfx::cmdbuffer_draw(&frame->cmdbuffer, 0, num_vertices);

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
			gfx::destroy_uniformset(&framedata[i].uniformset);
			gfx::destroy_buffer(&framedata[i].uniformbuffer);
			gfx::destroy_buffer(&framedata[i].vertexbuffer);
			gfx::destroy_cmdbuffer(&framedata[i].cmdbuffer);
			gfx::destroy_fence(&framedata[i].submitted);
		}
		
		gfx::destroy_cmdpool(&cmdpool);
		gfx::destroy_swapchain(&swapchain);
		gfx::destroy_uniformsetlayout(&uniformsetlayout);
		gfx::destroy_uniformpool(&uniformpool);
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