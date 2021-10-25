
/* Univesre parameters, laws and particles.
 * Implementations are in "universe.c". */

/* TODO: Comment. */

#ifndef IPSYS_HEADER_UNIVERSE__
#define IPSYS_HEADER_UNIVERSE__

#include "random.h"
#include "opengl.h"

/* Particle struct that holds all the data describing what is exclusive to a
 * single particle.
 * Note that it is packed and that every field is 32-bit wide. */
struct __attribute__((packed)) part_t
{
	GLfloat x, y;
	GLfloat speed;
	GLfloat angle;
	GLfloat pressure;
	GLfloat r, g, b;
	GLfloat oldx, oldy;
	GLuint type;
	GLuint age;
};
typedef struct part_t part_t;

#define CHANGE_TYPE_LAW_NUMBER 3
struct __attribute__((packed)) change_type_law_t
{
	GLuint used;
	GLuint has_speed_min; GLfloat speed_min;
	GLuint has_speed_max; GLfloat speed_max;
	GLuint has_pressure_min; GLfloat pressure_min;
	GLuint has_pressure_max; GLfloat pressure_max;
	GLuint has_age_min; GLuint age_min;
	GLuint has_age_max; GLuint age_max;
	GLuint new_type; GLfloat probability;
};
typedef struct change_type_law_t change_type_law_t;
struct __attribute__((packed)) part_type_t
{
	GLfloat br, bg, bb; /* color base */
	GLfloat sr, sg, sb; /* color speed */
	GLfloat pr, pg, pb; /* color pressure */
	change_type_law_t change_type_law_array[CHANGE_TYPE_LAW_NUMBER];
};
typedef struct part_type_t part_type_t;

#define PIL_STEP_NUMBER 90
struct __attribute__((packed)) pil_step_t
{
	GLfloat offset;
	GLfloat slope;
};
typedef struct pil_step_t pil_step_t;
struct __attribute__((packed)) pil_t
{
	pil_step_t steps[PIL_STEP_NUMBER];
};
typedef struct pil_t pil_t;
struct __attribute__((packed)) pil_set_t
{
	pil_t attraction;
	pil_t angle;
	pil_t speed; /* pos: speed -> +inf (1->*2); neg: speed -> 0 (-1->/2) */
};
typedef struct pil_set_t pil_set_t;

struct __attribute__((packed)) universe_info_t
{
	GLuint part_number;
	GLuint type_number;
	GLuint change_type_law_number;
	GLuint pil_step_number;
	GLfloat pil_step_dist;
};
typedef struct universe_info_t universe_info_t;

void randomize_colors(part_type_t* type_table, unsigned int type_number,
	rg_t* rg);
void randomize_change_laws(part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number, rg_t* rg);
void disable_change_laws(part_type_t* type_table, unsigned int type_number,
	unsigned int change_type_law_number);
void randomize_parts(part_t* part_array, unsigned int part_number,
	unsigned int type_number, rg_t* rg);
void randomize_pils(pil_set_t* pil_set_table, unsigned int type_number,
	rg_t* rg);
void mutate_pils(pil_set_t* pil_set_table, unsigned int type_number,
	rg_t* rg);

#endif /* IPSYS_HEADER_UNIVERSE__ */
