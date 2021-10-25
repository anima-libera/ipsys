
#include "universe.h"
#include "random.h"
#include "utils.h"

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

void randomize_change_laws(part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number, rg_t* rg)
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

void disable_change_laws(part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < change_type_law_number; ++j)
	{
		change_type_law_t* ctl = &type_table[i].change_type_law_array[j];
		ctl->used = 0;
	}
}

void randomize_parts(part_t* part_array, unsigned int part_number,
	unsigned int type_number, rg_t* rg)
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
		for (int s = 1; s < PIL_STEP_NUMBER; ++s)
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
		for (int s = 1; s < PIL_STEP_NUMBER; ++s)
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
		for (int s = 1; s < PIL_STEP_NUMBER; ++s)
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

void mutate_pils(pil_set_t* pil_set_table, unsigned int type_number,
	rg_t* rg)
{
	for (unsigned int i = 0; i < type_number; ++i)
	for (unsigned int j = 0; j < type_number; ++j)
	{
		pil_set_t* pil_set = &pil_set_table[i * type_number + j];

		for (int s = 0; s < PIL_STEP_NUMBER; ++s)
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
