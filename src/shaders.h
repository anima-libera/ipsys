
/* Ipsys header file "shaders.h"
 * Related to OpenGL shaders.
 * Implementations are in "shaders.c". */

/* TODO: comment */

#ifndef IPSYS_HEADER_SHADERS__
#define IPSYS_HEADER_SHADERS__

#include <GL/glew.h>

extern GLuint g_shprog_draw_fade;
extern GLuint g_shprog_draw_ui_line;
extern GLuint g_shprog_draw_particles;
extern GLuint g_shprog_comp_iteruniv;

void shprog_build_all(void);

#endif /* IPSYS_HEADER_SHADERS__ */
