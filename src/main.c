
#include "utils.h"
#include "dbgmsg.h"
#include "random.h"
#include "shaders.h"
#include "universe.h"
#include "settings.h"
#include "serialization.h"
#include "opengl.h"
#include "ui.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#ifdef cplusplus__
#error C compiler required
#endif

SDL_Window* g_window = NULL;
SDL_GLContext g_opengl_context = NULL;

struct __attribute__((packed)) just_vertex_t
{
	GLfloat x, y;
};
typedef struct just_vertex_t just_vertex_t;

int g_callback_button_test_flag = 0;
void callback_test(widget_t* widget)
{
	(void)widget;
	g_callback_button_test_flag = 1;
}

int g_callback_slider_1_flag = 0;
void callback_slider_1(widget_t* widget)
{
	(void)widget;
	g_callback_slider_1_flag = 1;
}

#define MAX_ITER_PER_FRAME 32

int g_callback_slider_2_flag = 0;
void callback_slider_2(widget_t* widget)
{
	g_callback_slider_2_flag = 1;
	widget->slider.value = roundf(widget->slider.value * (float)(MAX_ITER_PER_FRAME - 1)) /
		(float)(MAX_ITER_PER_FRAME - 1);
}

int main(int argc, char const** argv)
{
	l_beginning:;

	/* Parse command line arguments. */

	char const* arg_type_number = NULL;
	char const* arg_ipsysd_filepath = NULL;
	int no_universe = 0;

	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			//assert(0);
		}
		else if (strcmp(argv[i], "-n") == 0)
		{
			no_universe = 1;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			arg_ipsysd_filepath = argv[++i];
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			arg_type_number = argv[++i];
		}
		else
		{
			fprintf(stderr, "The command line parser is overwhelmed! "
				"It is still young..\n");
			break;
		}
	}

	int type_number = -1;
	if (arg_type_number != NULL)
	{
		unsigned int i;
		for (i = 0; arg_type_number[i] != '\0'; i++)
		{
			char c = arg_type_number[i];
			if (c < '0' || '9' < c)
			{
				fprintf(stderr, "The type number command line argument "
					"contains non-decimal-digit characters.\n");
				goto l_after_type_number;
			}
		}
		if (i == 0)
		{
			fprintf(stderr, "The type number command line argument "
				"is empty.\n");
			goto l_after_type_number;
		}
		type_number = 0;
		for (i = 0; arg_type_number[i] != '\0'; i++)
		{
			type_number = type_number * 10 + (arg_type_number[i] - '0');
		}
		if (type_number == 0)
		{
			fprintf(stderr, "The type number command line argument "
				"is zero.\n");
			type_number = -1;
			goto l_after_type_number;
		}
		printf("type number is %d\n", type_number);
	}
	l_after_type_number:

	/* Initialize SDL2 and OpenGL. */

	/* Initialize the SDL2 library and the GLEW OpenGL extension loader.
	 * Create the unique global window g_window and the unique global OpenGL
	 * context g_opengl_context. */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		error_sdl2_fail("SDL_Init");
		return EXIT_FAILURE;
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	g_window = SDL_CreateWindow("ipsys",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 800,
		SDL_WINDOW_OPENGL);
	if (g_window == NULL)
	{
		error_sdl2_fail("SDL_CreateWindow");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MAJOR_VERSION");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MINOR_VERSION");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_PROFILE_MASK");
		return EXIT_FAILURE;
	}
	g_opengl_context = SDL_GL_CreateContext(g_window);
	if (g_opengl_context == NULL)
	{
		error_sdl2_fail("SDL_GL_CreateContext");
		return EXIT_FAILURE;
	}
	#if USE_GLEW
		GLenum glew_init_result = glewInit();
		if (glew_init_result != GLEW_OK)
		{
			fprintf(stderr, "GLEW error: glewInit failed: \"%s\"\n",
				glewGetErrorString(glew_init_result));
		}
		fprintf(stderr, "Using GLEW\n");
	#else
		fprintf(stderr, "Not using GLEW\n");
	#endif
	glEnable(GL_MULTISAMPLE);
	if (SDL_GL_SetSwapInterval(-1) != 0)
	{
		SDL_GL_SetSwapInterval(1);
	}

	enable_opengl_dbgmsg();

	GLuint vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	if (shprog_build_all() != 0)
	{
		return EXIT_FAILURE;
	}

	/* Universe setup. */

	rg_t rg;
	rg_time_seed(&rg);

	universe_info_t info = {0};
	part_type_t* type_table = NULL;
	pil_set_t* pil_set_table = NULL;

	if (arg_ipsysd_filepath != NULL)
	{
		//universe_info_t info;
		//pil_set_t* pil_set_table;
		//part_type_t* type_table;
		deserialize_ipsysd_file(arg_ipsysd_filepath,
			&info, &pil_set_table, &type_table);

		info.change_type_law_number = CHANGE_TYPE_LAW_NUMBER;
		disable_change_laws(type_table,
			info.type_number, CHANGE_TYPE_LAW_NUMBER);
	}
	else
	{
		unsigned int tn =
			type_number >= 1 ? type_number :
			rg_int(&rg, 0, 3) != 0 ? 2 :
			rg_int(&rg, 1, 4);
		info.part_number = (256 * 6);
		info.type_number = tn;
		info.change_type_law_number = CHANGE_TYPE_LAW_NUMBER;
		info.pil_step_number = PIL_STEP_NUMBER;
		info.pil_step_dist = 0.006f;

		type_table = malloc(tn * sizeof(part_type_t));
		randomize_colors(type_table, tn, &rg);
		//randomize_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER, &rg);
		disable_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER);

		pil_set_table = malloc(tn*tn * sizeof(pil_set_t));
		randomize_pils(pil_set_table, tn, &rg);
	}

	unsigned int tn = info.type_number;
	unsigned int tnu = tn;

	GLuint buf_info_id;
	glGenBuffers(1, &buf_info_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_info_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof info,
		&info, GL_STATIC_DRAW);

	GLuint buf_type_id;
	glGenBuffers(1, &buf_type_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
	glBufferData(GL_ARRAY_BUFFER, tn * sizeof(part_type_t),
		type_table, GL_STATIC_DRAW);
	
	GLuint buf_pil_set_id;
	glGenBuffers(1, &buf_pil_set_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
	glBufferData(GL_ARRAY_BUFFER, tn*tn * sizeof(pil_set_t),
		pil_set_table, GL_STATIC_DRAW);
	
	#define PARTICLE_NUMBER (256 * 6)
	part_t part_array[PARTICLE_NUMBER] = {0};

	randomize_parts(part_array, PARTICLE_NUMBER, tnu, &rg);

	GLuint buf_part_curr_id;
	glGenBuffers(1, &buf_part_curr_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof part_array,
		part_array, GL_DYNAMIC_DRAW);

	GLuint buf_part_next_id;
	glGenBuffers(1, &buf_part_next_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_part_next_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof part_array,
		part_array, GL_DYNAMIC_DRAW);

	#define WORK_GROUP_SIZE 256

	setting_set_fade_factor(0.05f);

	unsigned int iteration_number_per_frame = 4;

	/* Universe rendering setup. */

	int render_each_iteration = 1;

	int no_fading = 0;

	unsigned int univ_rendering_index = 0;
	GLuint univ_texture_double_id[2];
	glGenTextures(2, univ_texture_double_id);
	GLuint univ_fbo_double_id[2];
	glGenFramebuffers(2, univ_fbo_double_id);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, univ_texture_double_id[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, univ_fbo_double_id[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, univ_texture_double_id[i], 0);
		GLenum const draw_buffer_array[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, draw_buffer_array);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(0);
			/* TODO: Get a true error message. */
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	just_vertex_t const univ_full_rect_array[4] = {
		{.x = +1.0f, .y = +1.0f},
		{.x = -1.0f, .y = +1.0f},
		{.x = +1.0f, .y = -1.0f},
		{.x = -1.0f, .y = -1.0f},
	};

	GLuint buf_univ_full_rect_id;
	glGenBuffers(1, &buf_univ_full_rect_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_univ_full_rect_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof univ_full_rect_array,
		univ_full_rect_array, GL_STATIC_DRAW);

	/* Ui fabric setup. */

	ui_fabric_t ui_fabric = {0};
	uipt_init(&ui_fabric.ui_line_table, sizeof(ui_line_t), line_drawcall_callback);
	uipt_init(&ui_fabric.ui_triangle_table, sizeof(ui_triangle_t), triangle_drawcall_callback);
	font_init(&ui_fabric.font);
	uipt_init(&ui_fabric.ui_gchar_table, sizeof(gchar_t), gchar_drawcall_callback);

	/* UI setup. */

	ui_vertex_t ui_bg_vertex_array[4] = {
		{.x = 800.0f, .y = 800.0f},
		{.x =   0.0f, .y = 800.0f},
		{.x = 800.0f, .y =   0.0f},
		{.x =   0.0f, .y =   0.0f},
	};

	for (unsigned int i = 0; i < 4; i++)
	{
		ui_bg_vertex_array[i].r = 0.0f;
		ui_bg_vertex_array[i].g = 0.05f;
		ui_bg_vertex_array[i].b = 0.2f;
	}

	GLuint buf_ui_bg_id;
	glGenBuffers(1, &buf_ui_bg_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_bg_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof ui_bg_vertex_array,
		ui_bg_vertex_array, GL_STATIC_DRAW);

	widget_manager_t wm = {.next_x = 20.0f, .next_y = 800.0f - 40.0f};

	widget_t widget_slider_test_arr[2];
	widget_init_slider(&ui_fabric, &widget_slider_test_arr[0],
		1.0f - g_setting_read_fade_factor / SETTING_FADE_FACTOR_MAX,
		0.0f, 0.0f, 760.0f, 20.0f, "TRACE", callback_slider_1);
	widget_manager_give(&wm, &ui_fabric, &widget_slider_test_arr[0]);
	widget_init_slider(&ui_fabric, &widget_slider_test_arr[1],
		((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f),
		0.0f, 0.0f, 760.0f, 20.0f, "ITERATION NUMBER PER FRAME", callback_slider_2);
	widget_manager_give(&wm, &ui_fabric, &widget_slider_test_arr[1]);
	
	widget_t widget_button_test_arr[1];
	for (unsigned int i = 0; i < 1; i++)
	{
		widget_init_button(&ui_fabric, &widget_button_test_arr[i],
			0.0f, 0.0f, 200.0f, 20.0f, "RANDOMIZE LAWS", callback_test);
		widget_manager_give(&wm, &ui_fabric, &widget_button_test_arr[i]);
	}

	/* Core loop. */

	unsigned int t = 0;
	int running = 1;
	int restart = 0;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = 0;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.x >= 800)
					{
						float const x = event.button.x - 800;
						float const y = 800 - event.button.y;

						if (widget_manager_clic(&wm, &ui_fabric, x, y))
						{
							break;
						}
					}
					else
					{
						float sign;
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							sign = 1.0f;
						}
						else if (event.button.button == SDL_BUTTON_RIGHT)
						{
							sign = -1.0f;
						}
						else
						{
							break;
						}
						float cx = (float)event.button.x / 400.0f - 1.0f;
						float cy = -((float)event.button.y / 400.0f - 1.0f);
						glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
						part_t* part_array = glMapBuffer(GL_ARRAY_BUFFER,
							GL_READ_WRITE);
						for (unsigned int i = 0; i < PARTICLE_NUMBER; i++)
						{
							float x = part_array[i].x;
							float y = part_array[i].y;
							float square_dist = (x-cx)*(x-cx) + (y-cy)*(y-cy);
							if (square_dist >= 0.3f * 0.3f)
							{
								continue;
							}
							float speed = part_array[i].speed;
							float angle = part_array[i].angle;
							float vx = cosf(angle) * speed;
							float vy = sinf(angle) * speed;
							if (square_dist < 0.05f * 0.05f)
							{
								if (sign < 0.0f)
								{
									square_dist = 0.05f;
								}
								else
								{
									square_dist = 0.05f * 0.05f;
								}
							}
							vx += ((x - cx) / square_dist) * 0.001f * sign;
							vy += ((y - cy) / square_dist) * 0.001f * sign;
							part_array[i].speed = sqrtf(vx * vx + vy * vy);
							part_array[i].angle = atan2f(vy, vx);
						}
						glUnmapBuffer(GL_ARRAY_BUFFER);
					}
				break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = 0;
						break;

						case SDLK_q:
							running = 0;
							restart = 1;
						break;

						case SDLK_w:
							no_fading = !no_fading;
						break;

						case SDLK_t:
							render_each_iteration = !render_each_iteration;
						break;

						case SDLK_c:
							randomize_colors(type_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_p:
							randomize_parts(part_array,
								PARTICLE_NUMBER, tnu, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
							glBufferData(GL_ARRAY_BUFFER,
								PARTICLE_NUMBER * sizeof(part_t),
								part_array, GL_DYNAMIC_DRAW);
							glBindBuffer(GL_ARRAY_BUFFER, buf_part_next_id);
							glBufferData(GL_ARRAY_BUFFER,
								PARTICLE_NUMBER * sizeof(part_t),
								part_array, GL_DYNAMIC_DRAW);
						break;

						case SDLK_r:
							randomize_pils(pil_set_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_f:
							mutate_pils(pil_set_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_y:
							randomize_change_laws(type_table, tn,
								CHANGE_TYPE_LAW_NUMBER, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_h:
							disable_change_laws(type_table, tn,
								CHANGE_TYPE_LAW_NUMBER);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_s:
							serialize_universe_rules("uwu.ipsysd",
								&info, pil_set_table, type_table);
						break;
					}
				break;
			}
		}

		if (g_callback_slider_1_flag)
		{
			g_callback_slider_1_flag = 0;
			float const value = (1.0f - widget_slider_test_arr[0].slider.value) *
				SETTING_FADE_FACTOR_MAX;
			setting_set_fade_factor(value);
		}
		if (g_callback_slider_2_flag)
		{
			g_callback_slider_2_flag = 0;
			unsigned int const value =
				roundf(widget_slider_test_arr[1].slider.value *
					((float)MAX_ITER_PER_FRAME - 1.0f) + 1.0f);
			iteration_number_per_frame = value;
		}
		if (g_callback_button_test_flag)
		{
			g_callback_button_test_flag = 0;
			randomize_pils(pil_set_table, tn, &rg);
			glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
			glBufferData(GL_ARRAY_BUFFER,
				tn*tn * sizeof(pil_set_t),
				pil_set_table, GL_STATIC_DRAW);
		}

		t++;

		/* Render the UI. */
		{
			#define ATTRIB_LOCATION_POS 0
			#define ATTRIB_LOCATION_COLOR 1

			glViewport(800, 0, 800, 800);
			glUseProgram(g_shprog_draw_ui_simple);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			
			/* Clear UI. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_bg_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glUseProgram(0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR

			/* Draw the UI fabric. */
			{
				uipt_draw(&ui_fabric.ui_triangle_table, NULL);
				uipt_draw(&ui_fabric.ui_line_table, NULL);
				uipt_draw(&ui_fabric.ui_gchar_table, &ui_fabric.font);
			}
		}

		if (!no_universe)
		{
			/* Fade-to-black effect in the universe. */
			if (no_fading)
			{
				glBindFramebuffer(GL_FRAMEBUFFER,
					univ_fbo_double_id[univ_rendering_index]);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER,
					univ_fbo_double_id[univ_rendering_index]);
				glViewport(0, 0, 800, 800);

				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, univ_texture_double_id[1-univ_rendering_index]);

				#define ATTRIB_LOCATION_POS 0

				glUseProgram(g_shprog_draw_texture_fade_2);
				glProgramUniform1f(g_shprog_draw_texture_fade_2, 0, g_setting_read_fade_factor);
				glProgramUniform1i(g_shprog_draw_texture_fade_2, 1, 0);

				glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
				
				glBindBuffer(GL_ARRAY_BUFFER, buf_univ_full_rect_id);
				glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
					GL_FALSE, sizeof(just_vertex_t),
					(void*)offsetof(just_vertex_t, x));
				
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				glUseProgram(0);

				#undef ATTRIB_LOCATION_POS
			}

			/* Universe iterations. */
			for (unsigned int i = 0; i < iteration_number_per_frame; i++)
			{
				if (render_each_iteration)
				{
					glProgramUniform1i(g_shprog_comp_iteruniv, 0, 1);
				}
				else
				{
					glProgramUniform1i(g_shprog_comp_iteruniv, 0, i == 0);
				}
				glUseProgram(g_shprog_comp_iteruniv);
				
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_part_curr_id);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_part_next_id);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_type_id);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buf_pil_set_id);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buf_info_id);

				glDispatchCompute(PARTICLE_NUMBER / WORK_GROUP_SIZE, 1, 1);
				glUseProgram(0);
				glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

				SWAP(GLuint, buf_part_curr_id, buf_part_next_id);

				/* Render the particles in the universe. */
				if (render_each_iteration || i == iteration_number_per_frame-1)
				{
					glBindFramebuffer(GL_FRAMEBUFFER,
						univ_fbo_double_id[univ_rendering_index]);
					glViewport(0, 0, 800, 800);

					#define ATTRIB_LOCATION_POS 0
					#define ATTRIB_LOCATION_COLOR 1
					#define ATTRIB_LOCATION_ANGLE 2
					#define ATTRIB_LOCATION_OLDPOS 3

					glUseProgram(g_shprog_draw_particles);
					glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
					glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
					glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
					glEnableVertexAttribArray(ATTRIB_LOCATION_OLDPOS);
					
					glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
					glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
						GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, x));
					glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
						GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, r));
					glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
						GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, angle));
					glVertexAttribPointer(ATTRIB_LOCATION_OLDPOS, 2, GL_FLOAT,
						GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, oldx));

					glDrawArrays(GL_POINTS, 0, PARTICLE_NUMBER);
					
					glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
					glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
					glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
					glDisableVertexAttribArray(ATTRIB_LOCATION_OLDPOS);
					glUseProgram(0);

					#undef ATTRIB_LOCATION_POS
					#undef ATTRIB_LOCATION_COLOR
					#undef ATTRIB_LOCATION_ANGLE
					#undef ATTRIB_LOCATION_OLDPOS

					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}
			}

			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, univ_fbo_double_id[univ_rendering_index]);
				glBlitFramebuffer(0, 0, 800, 800, 0, 0, 800, 800, GL_COLOR_BUFFER_BIT, GL_NEAREST);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			}

			univ_rendering_index = 1 - univ_rendering_index;
		}

		SDL_GL_SwapWindow(g_window);
	}

	SDL_GL_DeleteContext(g_opengl_context);
	g_opengl_context = NULL;
	SDL_DestroyWindow(g_window);
	g_window = NULL;
	SDL_Quit();

	if (restart)
	{
		printf("Restarting (beware memory leaks)\n");
		goto l_beginning;
	}
	return 0;
}
