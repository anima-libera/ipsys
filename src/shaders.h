
/* ipsi header file "shaders.h"
 * Related to OpenGL shaders.
 * Implementations are in "shaders.c". */

/* TODO: comment */

#ifndef ipsi_header_shaders__
#define ipsi_header_shaders__

#include <GL/glew.h>

extern GLuint g_shprog_draw_fade;
extern GLuint g_shprog_draw_particles;
extern GLuint g_shprog_comp_iteruniv;

void shprog_build_all(void);

void shprog_build_fade(void);
void shprog_build_particles(void);
void shprog_build_iteruniv(void);

#endif /* ipsi_header_shaders__ */
