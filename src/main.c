
#include "utils.h"
#include "window.h"
#include "dbgmsg.h"
#include "random.h"
#include "shaders.h"
#include "universe.h"
#include "settings.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

void randomize_colors(part_type_t* type_table, unsigned int type_number)
{
	for (unsigned int i = 0; i < type_number; ++i)
	{
		type_table[i].br = rg_float(g_rg, 0.0f, 1.0f);
		type_table[i].bg = rg_float(g_rg, 0.0f, 1.0f);
		type_table[i].bb = rg_float(g_rg, 0.0f, 1.0f);
		type_table[i].sr = rg_float(g_rg, -100.0f, 100.0f);
		type_table[i].sg = rg_float(g_rg, -100.0f, 100.0f);
		type_table[i].sb = rg_float(g_rg, -100.0f, 100.0f);
		type_table[i].pr = rg_float(g_rg, -100.0f, 100.0f);
		type_table[i].pg = rg_float(g_rg, -100.0f, 100.0f);
		type_table[i].pb = rg_float(g_rg, -100.0f, 100.0f);
	}
}

void randomize_change_laws(
	part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < change_type_law_number; ++j)
	{
		change_type_law_t* ctl = &type_table[i].change_type_law_array[j];
		ctl->used = (type_number > 1) * rg_uint(g_rg, 0, 1);
		if (!ctl->used)
		{
			continue;
		}
		ctl->has_speed_min = 0;//rg_uint(g_rg, 0, 1);
		ctl->has_speed_max = 0;//rg_uint(g_rg, 0, 1);
		ctl->has_pressure_min = 1;//rg_uint(g_rg, 0, 1);
		ctl->has_pressure_max = 0;//rg_uint(g_rg, 0, 1);
		ctl->has_age_min = 1;//rg_uint(g_rg, 0, 1);
		ctl->has_age_max = 0;//rg_uint(g_rg, 0, 1);
		ctl->speed_min = rg_float(g_rg, 0.0f, 0.02f);
		ctl->speed_max = rg_float(g_rg, 0.0f, 0.02f);
		ORDER(float, ctl->speed_min, ctl->speed_max);
		ctl->pressure_min = rg_float(g_rg, 0.0f, 1.5f);
		ctl->pressure_max = rg_float(g_rg, 0.0f, 1.5f);
		ORDER(float, ctl->pressure_min, ctl->pressure_max);
		ctl->age_min = rg_uint(g_rg, 0, 200);
		ctl->age_max = rg_uint(g_rg, 0, 200);
		ORDER(unsigned int, ctl->age_min, ctl->age_max);
		do {
			ctl->new_type = rg_uint(g_rg, 0, type_number-1);
		} while (ctl->new_type == i);
		ctl->probability = rg_float(g_rg, 0.0f, 0.1f);
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
	unsigned int part_number, unsigned int type_number)
{
	for (unsigned int i = 0; i < part_number; ++i)
	{
		part_array[i].x = rg_float(g_rg, -1.0f, 1.0f);
		part_array[i].y = rg_float(g_rg, -1.0f, 1.0f);
		part_array[i].speed = 0.0f;
		part_array[i].angle = rg_float(g_rg, 0.0f, TAU);
		part_array[i].type = rg_uint(g_rg, 0, type_number-1);
		part_array[i].age = 0;
	}
}

void randomize_pils(pil_set_t* pil_set_table, unsigned int type_number)
{
	int continuous = 1;//rg_int(g_rg, 1, 5);
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < type_number; ++j)
	{
		pil_set_t* pil_set = &pil_set_table[i * type_number + j];

		/* attraction */
		pil_set->attraction.steps[0].offset = rg_int(g_rg, 0, 8) == 0 ?
			rg_float(g_rg, -0.0010f, 0.0010f) :
			rg_float(g_rg, 0.0003f, 0.0005f);
		pil_set->attraction.steps[0].slope = 0.0f;
		for (int s = 1; s < PISL_STEP_NUMBER; ++s)
		{
			if (rg_int(g_rg, 0, continuous) == 0)
			{
				if (rg_int(g_rg, 0, s) > 5)
				{
					pil_set->attraction.steps[s].offset = 0.0f;
					pil_set->attraction.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(g_rg, 0, 15) == 0) ?
						0.0007f / ((float)(s)) :
						0.0007f / ((float)(s*s));
					pil_set->attraction.steps[s].offset =
						rg_float(g_rg, -e/* *0.75f */, e);
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
			if (rg_int(g_rg, 0, continuous) == 0)
			{
				if (rg_int(g_rg, 0, 5) != 0)
				{
					pil_set->angle.steps[s].offset = 0.0f;
					pil_set->angle.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(g_rg, 0, 4) == 0) ?
						TAU/4.0f : 
						TAU/10.0f;
					pil_set->angle.steps[s].offset =
						rg_float(g_rg, -e, e);
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
		pil_set->speed.steps[0].offset = (rg_int(g_rg, 0, 4) == 0) ?
			rg_float(g_rg, -0.0010f, 0.0010f) : 
			0.0f;
		pil_set->speed.steps[0].slope = 0.0f;
		for (int s = 1; s < PISL_STEP_NUMBER; ++s)
		{
			if (rg_int(g_rg, 0, continuous) == 0)
			{
				if (rg_int(g_rg, 0, 10) != 0)
				{
					pil_set->speed.steps[s].offset = 0.0f;
					pil_set->speed.steps[s].slope = 0.0f;
				}
				else
				{
					float e = (rg_int(g_rg, 0, 4) == 0) ?
						0.0007f / ((float)(s)) :
						0.0007f / ((float)(s*s));
					pil_set->speed.steps[s].offset =
						rg_float(g_rg, -e, e);
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

void mutate_pils(pil_set_t* pil_set_table, unsigned int type_number)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < type_number; ++j)
	{
		pil_set_t* pil_set = &pil_set_table[i * type_number + j];

		for (int s = 0; s < PISL_STEP_NUMBER; ++s)
		{
			#define MUTATE(x_, f_, v_) x_ += rg_float(g_rg, \
				-f_ * (x_ > 0.0f ? v_ : 1.0f), \
				+f_ * (x_ < 0.0f ? v_ : 1.0f))
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

int main(int argc, const char** argv)
{
	(void)argc; (void)argv; /* Unused for now... */

	if (init_g_graphics() != 0)
	{
		return -1;
	}
	enable_opengl_dbgmsg();

	GLuint vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	shprog_build_all();

	g_rg = rg_create_timeseeded(0);

	universe_info_t info = {0};
	info.type_number = rg_uint(g_rg, 1, 6);
	unsigned int tn = info.type_number;
	unsigned int tnu = (rg_uint(g_rg, 0, 1) == 0) ?
		2 : rg_uint(g_rg, 1, (tn > 3) ? 3 : tn);

	GLuint buf_info_id;
	glGenBuffers(1, &buf_info_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_info_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(universe_info_t),
		&info, GL_STATIC_DRAW);

	part_type_t* type_table = xmalloc(tn * sizeof(part_type_t));
	
	randomize_colors(type_table, tn);

	//randomize_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER);
	disable_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER);

	GLuint buf_type_id;
	glGenBuffers(1, &buf_type_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
	glBufferData(GL_ARRAY_BUFFER, tn * sizeof(part_type_t),
		type_table, GL_STATIC_DRAW);

	pil_set_t* pil_set_table = xmalloc(tn*tn * sizeof(pil_set_t));

	randomize_pils(pil_set_table, tn);

	GLuint buf_pil_set_id;
	glGenBuffers(1, &buf_pil_set_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
	glBufferData(GL_ARRAY_BUFFER, tn*tn * sizeof(pil_set_t),
		pil_set_table, GL_STATIC_DRAW);

	#define PARTICLE_NUMBER (256 * 6)
	part_t part_array[PARTICLE_NUMBER] = {0};

	randomize_parts(part_array, PARTICLE_NUMBER, tnu);

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

	int running = 1;
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
						setting_set_fade_factor(
							((float)event.button.x-800.0f)/4000.0f);
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

						case SDLK_c:
							randomize_colors(type_table, tn);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_p:
							randomize_parts(part_array,
								PARTICLE_NUMBER, tnu);
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
							randomize_pils(pil_set_table, tn);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_f:
							mutate_pils(pil_set_table, tn);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_y:
							randomize_change_laws(type_table, tn,
								CHANGE_TYPE_LAW_NUMBER);
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

		{
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
		}

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

		SDL_GL_SwapWindow(g_window);
	}

	rg_destroy(g_rg);
	cleanup_g_graphics();
	return 0;
}
