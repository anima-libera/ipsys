
/* ipsi header file "random.h"
 * Allow to generate nice pseudo random numbers.
 * Implementations are in "random.c". */

#ifndef ipsi_header_random__
#define ipsi_header_random__

#include <stdint.h> /* uint32_t */

/* Type of the seed object used to initialise a rg_t object.
 * Note that the size of a rg_t object can be considered as part of the full
 * seed since it also influences the rg's output. */
typedef uint32_t rg_seed_t;

/* An APSILG random generator state object. */
typedef struct rg_t rg_t;
struct rg_t
{
	int version; /* Ensures backward compatibility */
	rg_seed_t seed;
	int size;
	uint32_t* lcg_array;
	int cur;
};

#define RG_DEFAULT_VERSION 1
#define RG_DEFAULT_SIZE 41 /* A prime might help ? */

/* The global random generator. */
extern rg_t* g_rg;

/* Create a rg with the given size and initialise it with the given seed.
 * If the size parameter is 0, a default size will be used instead, thus it is
 * recomended to set the size parameter to 0. */
rg_t* rg_create_seeded(int size, rg_seed_t seed);

/* Same as the rg_create_seeded function, but use the system's timestamp to
 * seed the created rg. */
rg_t* rg_create_timeseeded(int size);

/* Free the given rg. */
void rg_destroy(rg_t* rg);

/* Perform a rg step and return a random unsigned int. */
unsigned int rg_uint_full(rg_t* rg);

/* Perform a rg step and return a random unsigned int greater than or equeal to
 * inf and less than or equal to sup. */
unsigned int rg_uint(rg_t* rg, unsigned int inf, unsigned int sup);

/* Perform a rg step and return a random int. */
int rg_int_full(rg_t* rg);

/* Perform a rg step and return a random int greater than or equeal to inf and
 * less than or equal to sup. */
int rg_int(rg_t* rg, int inf, int sup);

/* Perform at least one rg step and return a random float (non-infinite and
 * non-NaN). */
float rg_float_full(rg_t* rg);

/* Perform a rg step and return a random float between 0.0f and 1.0f. */
float rg_float_unit(rg_t* rg);

/* Perform a rg step and return a random float between inf and sup. */
float rg_float(rg_t* rg, float inf, float sup);

/* Perform a rg step and return a random seed object. */
rg_seed_t rg_rg_seed(rg_t* rg);

#endif /* ipsi_header_random__ */
