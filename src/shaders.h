
/* OpenGL shader programs declarations and building functions.
 * Implementations are in "shaders.c". */

#ifndef IPSYS_HEADER_SHADERS__
#define IPSYS_HEADER_SHADERS__

#include "opengl.h"

/* These are OpenGL shader program ids that can be given to glUseProgram.
 * They are set to valid shader program id values by a call to shprog_build_all
 * and unset by a call to shprog_destroy_all. */
extern GLuint g_shprog_draw_particles;
extern GLuint g_shprog_draw_ui_simple;
extern GLuint g_shprog_draw_fade;
extern GLuint g_shprog_draw_texture_fade;
extern GLuint g_shprog_draw_texture_fade_2;
extern GLuint g_shprog_comp_iteruniv;

/* Builds all the shader programs.
 * Returns -1 if anything goes wrong, returns 0 if everything goes well. */
int shprog_build_all(void);

/* Destroys all the shader programs built by a call to shprog_build_all. */
void shprog_destroy_all(void);

#endif /* IPSYS_HEADER_SHADERS__ */
