
/* Ipsys header file "window.h"
 * Related to the SDL2 window and the OpenGL context.
 * Implementations are in "window.c". */

#ifndef IPSYS_HEADER_WINDOW__
#define IPSYS_HEADER_WINDOW__

#include <GL/glew.h>
#include <SDL2/SDL.h>

/* The unique global APSILG window.
 * It is created by init_g_graphics and destroyed by cleanup_g_graphics. */
extern SDL_Window* g_window;

/* The unique global SDL2-window-compatible OpenGL context usd by APSILG.
 * It is created by init_g_graphics and destroyed by cleanup_g_graphics. */
extern SDL_GLContext g_opengl_context;

#if 0
/* Initialize the SDL2 library and the GLEW OpenGL extension loader.
 * Create the unique global window g_window and the unique global OpenGL
 * context g_opengl_context.
 * Return -1 if anything goes wrong, return 0 if everything goes well.
 * This function should be called once at the beginning of execution, and
 * a unique maching call to cleanup_g_graphics should be done at the end of
 * execution. */
int init_g_graphics(void);

/* Clean up and destroy what is initialized and created by init_g_graphics. */
void cleanup_g_graphics(void);
#endif

#endif /* IPSYS_HEADER_WINDOW__ */
