
/* Ipsys header file "dbgmsg.h"
 * Make easier debuging output related to OpenGL and SDL2.
 * Implementations are in "dbgmsg.c". */

#ifndef IPSYS_HEADER_DBGMSG__
#define IPSYS_HEADER_DBGMSG__

/* Enable the OpenGL debug message through callback calls feature.
 * OpenGL 4.3 or above is requiered for this function to work. */
void enable_opengl_dbgmsg(void);

/* Disable what is enabled by enable_opengl_dbgmsg. */
void disable_opengl_dbgmsg(void);

/* Print an error message to stderr. Should be called with the name of the SDL2
 * function that failed, or the operation involving the SDL2 that failed. */
void error_sdl2_fail(const char* operation);

#endif /* IPSYS_HEADER_DBGMSG__ */
