
#include "utils.h"
#include "dbgmsg.h"
#include "random.h"
#include "shaders.h"
#include "universe.h"
#include "settings.h"
#include "opengl.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

void randomize_colors(part_type_t* type_table, unsigned int type_number,
	rg_t* rg)
{
	for (unsigned int i = 0; i < type_number; ++i)
	{
		type_table[i].br = rg_float(rg, 0.0f, 1.0f);
		type_table[i].bg = rg_float(rg, 0.0f, 1.0f);
		type_table[i].bb = rg_float(rg, 0.0f, 1.0f);
		type_table[i].sr = rg_float(rg, -100.0f, 100.0f);
		type_table[i].sg = rg_float(rg, -100.0f, 100.0f);
		type_table[i].sb = rg_float(rg, -100.0f, 100.0f);
		type_table[i].pr = rg_float(rg, -100.0f, 100.0f);
		type_table[i].pg = rg_float(rg, -100.0f, 100.0f);
		type_table[i].pb = rg_float(rg, -100.0f, 100.0f);
	}
}

void randomize_change_laws(
	part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number,
	rg_t* rg)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < change_type_law_number; ++j)
	{
		change_type_law_t* ctl = &type_table[i].change_type_law_array[j];
		ctl->used = (type_number > 1) * rg_int(rg, 0, 1);
		if (!ctl->used)
		{
			continue;
		}
		ctl->has_speed_min = 0;//rg_int(rg, 0, 1);
		ctl->has_speed_max = 0;//rg_int(rg, 0, 1);
		ctl->has_pressure_min = 1;//rg_int(rg, 0, 1);
		ctl->has_pressure_max = 0;//rg_int(rg, 0, 1);
		ctl->has_age_min = 1;//rg_int(rg, 0, 1);
		ctl->has_age_max = 0;//rg_int(rg, 0, 1);
		ctl->speed_min = rg_float(rg, 0.0f, 0.02f);
		ctl->speed_max = rg_float(rg, 0.0f, 0.02f);
		ORDER(float, ctl->speed_min, ctl->speed_max);
		ctl->pressure_min = rg_float(rg, 0.0f, 1.5f);
		ctl->pressure_max = rg_float(rg, 0.0f, 1.5f);
		ORDER(float, ctl->pressure_min, ctl->pressure_max);
		ctl->age_min = rg_int(rg, 0, 200);
		ctl->age_max = rg_int(rg, 0, 200);
		ORDER(unsigned int, ctl->age_min, ctl->age_max);
		do {
			ctl->new_type = rg_int(rg, 0, type_number-1);
		} while (ctl->new_type == i);
		ctl->probability = rg_float(rg, 0.0f, 0.1f);
	}
}

void disable_change_laws(
	part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < change_type_law_number; ++j)
	{
		change_type_law_t* ctl = &type_table[i].change_type_law_array[j];
		ctl->used = 0;
	}
}

void randomize_parts(
	part_t* part_array,
	unsigned int part_number, unsigned int type_number,
	rg_t* rg)
{
	for (unsigned int i = 0; i < part_number; ++i)
	{
		part_array[i].x = rg_float(rg, -1.0f, 1.0f);
		part_array[i].y = rg_float(rg, -1.0f, 1.0f);
		part_array[i].speed = 0.0f;
		part_array[i].angle = rg_float(rg, 0.0f, TAU);
		part_array[i].type = rg_int(rg, 0, type_number-1);
		part_array[i].age = 0;
	}
}

void randomize_pils(pil_set_t* pil_set_table, unsigned int type_number,
	rg_t* rg)
{
	int continuous = 1;//rg_int(rg, 1, 5);
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < type_number; ++j)
	{
		pil_set_t* pil_set = &pil_set_table[i * type_number + j];

		/* attraction */
		pil_set->attraction.steps[0].offset = rg_int(rg, 0, 8) == 0 ?
			rg_float(rg, -0.0010f, 0.0010f) :
			rg_float(rg, 0.0003f, 0.0005f);
		pil_set->attraction.steps[0].slope = 0.0f;
		for (int s = 1; s < PISL_STEP_NUMBER; ++s)
		{
			if (rg_int(rg, 0, continuous) == 0)
			{
				if (rg_int(rg, 0, s) > 5)
				{
					pil_set->attraction.steps[s].offset = 0.0f;
					pil_set->attraction.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(rg, 0, 15) == 0) ?
						0.0007f / ((float)(s)) :
						0.0007f / ((float)(s*s));
					pil_set->attraction.steps[s].offset =
						rg_float(rg, -e/* *0.75f */, e);
					pil_set->attraction.steps[s].slope = 0.0f;
				}
			}
			else
			{
				pil_set->attraction.steps[s].offset =
					pil_set->attraction.steps[s-1].offset;
				pil_set->attraction.steps[s].slope = 0.0f;
			}
		}

		/* angle */
		pil_set->angle.steps[0].offset = 0.0f;
		pil_set->angle.steps[0].slope = 0.0f;
		for (int s = 1; s < PISL_STEP_NUMBER; ++s)
		{
			if (rg_int(rg, 0, continuous) == 0)
			{
				if (rg_int(rg, 0, 5) != 0)
				{
					pil_set->angle.steps[s].offset = 0.0f;
					pil_set->angle.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(rg, 0, 4) == 0) ?
						TAU/4.0f : 
						TAU/10.0f;
					pil_set->angle.steps[s].offset =
						rg_float(rg, -e, e);
					pil_set->angle.steps[s].slope = 0.0f;
				}
			}
			else
			{
				pil_set->angle.steps[s].offset =
					pil_set->angle.steps[s-1].offset;
				pil_set->angle.steps[s].slope = 0.0f;
			}
		}

		/* speed */
		pil_set->speed.steps[0].offset = (rg_int(rg, 0, 4) == 0) ?
			rg_float(rg, -0.0010f, 0.0010f) : 
			0.0f;
		pil_set->speed.steps[0].slope = 0.0f;
		for (int s = 1; s < PISL_STEP_NUMBER; ++s)
		{
			if (rg_int(rg, 0, continuous) == 0)
			{
				if (rg_int(rg, 0, 10) != 0)
				{
					pil_set->speed.steps[s].offset = 0.0f;
					pil_set->speed.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(rg, 0, 4) == 0) ?
						0.0007f / ((float)(s)) :
						0.0007f / ((float)(s*s));
					pil_set->speed.steps[s].offset =
						rg_float(rg, -e, e);
					pil_set->speed.steps[s].slope = 0.0f;
				}
			}
			else
			{
				pil_set->speed.steps[s].offset =
					pil_set->speed.steps[s-1].offset;
				pil_set->speed.steps[s].slope = 0.0f;
			}
		}
	}
}

void mutate_pils(pil_set_t* pil_set_table, unsigned int type_number, rg_t* rg)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < type_number; ++j)
	{
		pil_set_t* pil_set = &pil_set_table[i * type_number + j];

		for (int s = 0; s < PISL_STEP_NUMBER; ++s)
		{
			#define MUTATE(x_, f_, v_) x_ += rg_float(rg, \
				-f_ * (x_ > 0.0f ? v_ : 1.0f), \
				+f_ * (x_ < 0.0f ? v_ : 1.0f)) / ((float)(s+60) / 60.0f)

			MUTATE(pil_set->attraction.steps[s].offset, 0.000001f, 1.1f);
			MUTATE(pil_set->attraction.steps[s].slope, 0.000001f, 1.1f);
			MUTATE(pil_set->angle.steps[s].offset, 0.000001f, 1.1f);
			MUTATE(pil_set->angle.steps[s].slope, 0.000001f, 1.1f);
			MUTATE(pil_set->speed.steps[s].offset, 0.0000002f, 1.1f);
			MUTATE(pil_set->speed.steps[s].slope, 0.0000002f, 1.1f);

			#undef MUTATE
		}
	}
}

struct __attribute__((packed)) ui_vertex_t
{
	GLfloat x, y;
	GLfloat r, g, b;
};
typedef struct ui_vertex_t ui_vertex_t;

SDL_Window* g_window = NULL;
SDL_GLContext g_opengl_context = NULL;

int main(int argc, const char** argv)
{
	l_beginning:;

	/* Parse command line arguments. */

	const char* arg_type_number = NULL;

	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			;
		}
		else if (arg_type_number == NULL)
		{
			arg_type_number = argv[i];
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
	unsigned int tn =
		type_number >= 1 ? type_number :
		rg_int(&rg, 0, 3) != 0 ? 2 :
		rg_int(&rg, 1, 4);
	unsigned int tnu = tn;
	info.type_number = tn;

	GLuint buf_info_id;
	glGenBuffers(1, &buf_info_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_info_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(universe_info_t),
		&info, GL_STATIC_DRAW);

	part_type_t* type_table = malloc(tn * sizeof(part_type_t));
	
	randomize_colors(type_table, tn, &rg);

	//randomize_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER, &rg);
	disable_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER);

	GLuint buf_type_id;
	glGenBuffers(1, &buf_type_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
	glBufferData(GL_ARRAY_BUFFER, tn * sizeof(part_type_t),
		type_table, GL_STATIC_DRAW);

	pil_set_t* pil_set_table = malloc(tn*tn * sizeof(pil_set_t));

	randomize_pils(pil_set_table, tn, &rg);

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
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUMBER * sizeof(part_t),
		part_array, GL_DYNAMIC_DRAW);

	GLuint buf_part_next_id;
	glGenBuffers(1, &buf_part_next_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_part_next_id);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_NUMBER * sizeof(part_t),
		part_array, GL_DYNAMIC_DRAW);

	#define WORK_GROUP_SIZE 256

	setting_set_fade_factor(0.05f);

	#define MAX_ITER_PER_FRAME 16
	unsigned int iteration_number_per_frame = 4;

	/* UI setup. */

	const float ui_margin = 6.5f;
	const float ui_size = 20.0f;
	const float ui_length = (800.0f - ui_margin) - ui_margin;

	#define BAR_NUMBER 2

	ui_vertex_t ui_line_vertex_array[BAR_NUMBER * 4] = {
		{.x = ui_margin,          .y = 800.0f - ui_margin          },
		{.x = 800.0f - ui_margin, .y = 800.0f - ui_margin          },
		{.x = 800.0f - ui_margin, .y = 800.0f - ui_margin - ui_size},
		{.x = ui_margin,          .y = 800.0f - ui_margin - ui_size},
	};

	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);
		ui_line_vertex_array[i * 4 + 0].x = ui_margin;
		ui_line_vertex_array[i * 4 + 1].x = 800.0f - ui_margin;
		ui_line_vertex_array[i * 4 + 2].x = 800.0f - ui_margin;
		ui_line_vertex_array[i * 4 + 3].x = ui_margin;
		ui_line_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
		ui_line_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
		ui_line_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
		ui_line_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;
	}

	for (unsigned int i = 0; i < BAR_NUMBER * 4; i++)
	{
		ui_line_vertex_array[i].r = 1.0f;
		ui_line_vertex_array[i].g = 1.0f;
		ui_line_vertex_array[i].b = 1.0f;
	}

	GLuint buf_ui_line_id;
	glGenBuffers(1, &buf_ui_line_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_line_id);
	glBufferData(GL_ARRAY_BUFFER, BAR_NUMBER * 4 * sizeof(ui_vertex_t),
		ui_line_vertex_array, GL_STATIC_DRAW);

	GLuint ui_line_vertex_index_array[BAR_NUMBER * 8];
	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		ui_line_vertex_index_array[i * 8 + 0] = i * 4 + 0;
		ui_line_vertex_index_array[i * 8 + 1] = i * 4 + 1;
		ui_line_vertex_index_array[i * 8 + 2] = i * 4 + 1;
		ui_line_vertex_index_array[i * 8 + 3] = i * 4 + 2;
		ui_line_vertex_index_array[i * 8 + 4] = i * 4 + 2;
		ui_line_vertex_index_array[i * 8 + 5] = i * 4 + 3;
		ui_line_vertex_index_array[i * 8 + 6] = i * 4 + 3;
		ui_line_vertex_index_array[i * 8 + 7] = i * 4 + 0;
	}

	GLuint buf_ui_line_index_id;
	glGenBuffers(1, &buf_ui_line_index_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_line_index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BAR_NUMBER * 8 * sizeof(GLuint),
		ui_line_vertex_index_array, GL_STATIC_DRAW);

	ui_vertex_t ui_rect_vertex_array[BAR_NUMBER * 4];

	float ui_rect_filled_x[BAR_NUMBER] = {
		ui_length * (1.0f - g_setting_read_fade_factor / SETTING_FADE_FACTOR_MAX),
		ui_length * (1.0f - ((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f)),
	};

	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);
		ui_rect_vertex_array[i * 4 + 0].x = 800.0f - ui_margin - ui_rect_filled_x[i];
		ui_rect_vertex_array[i * 4 + 1].x = ui_margin;
		ui_rect_vertex_array[i * 4 + 2].x = 800.0f - ui_margin - ui_rect_filled_x[i];
		ui_rect_vertex_array[i * 4 + 3].x = ui_margin;
		ui_rect_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
		ui_rect_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
		ui_rect_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
		ui_rect_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;
	}

	for (unsigned int i = 0; i < BAR_NUMBER * 4; i++)
	{
		ui_rect_vertex_array[i].r = 0.02f;
		ui_rect_vertex_array[i].g = 0.8f;
		ui_rect_vertex_array[i].b = 0.4f;
	}

	GLuint buf_ui_rect_id;
	glGenBuffers(1, &buf_ui_rect_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
	glBufferData(GL_ARRAY_BUFFER, BAR_NUMBER * 4 * sizeof(ui_vertex_t),
		ui_rect_vertex_array, GL_STATIC_DRAW);

	GLuint ui_rect_vertex_index_array[BAR_NUMBER * 6];
	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		ui_rect_vertex_index_array[i * 6 + 0] = i * 4 + 0;
		ui_rect_vertex_index_array[i * 6 + 1] = i * 4 + 1;
		ui_rect_vertex_index_array[i * 6 + 2] = i * 4 + 2;
		ui_rect_vertex_index_array[i * 6 + 3] = i * 4 + 1;
		ui_rect_vertex_index_array[i * 6 + 4] = i * 4 + 2;
		ui_rect_vertex_index_array[i * 6 + 5] = i * 4 + 3;
	}

	GLuint buf_ui_rect_index_id;
	glGenBuffers(1, &buf_ui_rect_index_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_rect_index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BAR_NUMBER * 6 * sizeof(GLuint),
		ui_rect_vertex_index_array, GL_STATIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(ui_vertex_t),
		ui_bg_vertex_array, GL_STATIC_DRAW);

	/* Core loop. */

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
						const float x = event.button.x - 800;
						const float y = 800 - event.button.y;

						for (unsigned int i = 0; i < BAR_NUMBER; i++)
						{
							float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);

							if (ui_margin <= x &&
								x <= 800.0f - ui_margin &&
								ui_setup_y - ui_margin - ui_size <= y &&
								y <= ui_setup_y - ui_margin)
							{
								if (i == 0)
								{
									const float value =
										(x - ui_margin) / ui_length *
										SETTING_FADE_FACTOR_MAX;
									setting_set_fade_factor(value);

									ui_rect_filled_x[0] =
										ui_length *
										(1.0f - g_setting_read_fade_factor /
											SETTING_FADE_FACTOR_MAX);
								}
								else if (i == 1)
								{
									const unsigned int value =
										roundf((x - ui_margin) / ui_length * ((float)(MAX_ITER_PER_FRAME) - 1.0f) + 1.0f);
									iteration_number_per_frame = value;
								
									ui_rect_filled_x[1] =
										ui_length *
										(1.0f - ((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f));
								}

								ui_rect_vertex_array[i * 4 + 0].x = 800.0f - ui_margin - ui_rect_filled_x[i];
								ui_rect_vertex_array[i * 4 + 1].x = ui_margin;
								ui_rect_vertex_array[i * 4 + 2].x = 800.0f - ui_margin - ui_rect_filled_x[i];
								ui_rect_vertex_array[i * 4 + 3].x = ui_margin;
								ui_rect_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
								ui_rect_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
								ui_rect_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
								ui_rect_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;

								glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
								glBufferData(GL_ARRAY_BUFFER,
									BAR_NUMBER * 4 * sizeof(ui_vertex_t),
									ui_rect_vertex_array, GL_STATIC_DRAW);
							}
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
					}
				break;
			}
		}

		/* Render the UI. */
		{
			#define ATTRIB_LOCATION_POS ((GLuint)0)
			#define ATTRIB_LOCATION_COLOR ((GLuint)1)

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
			
			/* Render rect bar filling. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_rect_index_id);
			glDrawElements(GL_TRIANGLES, 6 * BAR_NUMBER, GL_UNSIGNED_INT,
				(void*)0);

			/* Render rect lines. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_line_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_line_index_id);
			glDrawElements(GL_LINES, 8 * BAR_NUMBER, GL_UNSIGNED_INT,
				(void*)0);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glUseProgram((GLuint)0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR
		}

		/* Render fade in the universe. */
		{
			glViewport(0, 0, 800, 800);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			glBlendFunc(GL_ONE, GL_ONE);
			glUseProgram(g_shprog_draw_fade);
			glDrawArrays(GL_POINTS, 0, 1);
			glUseProgram((GLuint)0);
			glDisable(GL_BLEND);
		}
		/* Apparently, on machines equipped with mesa drivers,
		 * the fading using GL_BLEND doesn't work (more precisely,
		 * the color buffer is not affected by the previous block).
		 * In that case, `glClear(GL_COLOR_BUFFER_BIT)` is a good
		 * quick workaround. Setting the fade_factor setting to 0.0f
		 * and using `glUseProgram(g_shprog_draw_fade)` without blending
		 * also works and is a bit better. */
		#if 0
		glClearColor(0, 0, 0, 255);
		glClear(GL_COLOR_BUFFER_BIT);
		#endif
		#if 0
		{
			glViewport(0, 0, 800, 800);
			glUseProgram(g_shprog_draw_fade);
			glDrawArrays(GL_POINTS, 0, 1);
			glUseProgram((GLuint)0);
		}
		#endif

		for (unsigned int i = 0; i < iteration_number_per_frame; i++)
		{
			glProgramUniform1i(g_shprog_comp_iteruniv, 0, 1);
			glUseProgram(g_shprog_comp_iteruniv);
			
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_part_curr_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_part_next_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_type_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buf_pil_set_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buf_info_id);

			glDispatchCompute(PARTICLE_NUMBER / WORK_GROUP_SIZE, 1, 1);
			glUseProgram((GLuint)0);
			glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

			SWAP(GLuint, buf_part_curr_id, buf_part_next_id);

			/* Render the particles in the universe. */
			{
				#define ATTRIB_LOCATION_POS ((GLuint)0)
				#define ATTRIB_LOCATION_COLOR ((GLuint)1)
				#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
				#define ATTRIB_LOCATION_OLDPOS ((GLuint)3)

				glViewport(0, 0, 800, 800);
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
				glUseProgram((GLuint)0);

				#undef ATTRIB_LOCATION_POS
				#undef ATTRIB_LOCATION_COLOR
				#undef ATTRIB_LOCATION_ANGLE
				#undef ATTRIB_LOCATION_OLDPOS
			}
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
		goto l_beginning;
	}
	return 0;
}
