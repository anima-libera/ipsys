
/* Debuging output related to OpenGL and SDL2.
 * Implementations are in "dbgmsg.c". */

#ifndef IPSYS_HEADER_DBGMSG__
#define IPSYS_HEADER_DBGMSG__

/* Enables the OpenGL debug message through callback calls feature.
 * OpenGL 4.3 or above is requiered for this function to work. */
void enable_opengl_dbgmsg(void);

/* Disables what is enabled by enable_opengl_dbgmsg. */
void disable_opengl_dbgmsg(void);

/* Prints an error message to stderr.
 * Should be called with the name of SDL2 function that failed,
 * or a description of the operation involving SDL2 that failed. */
void error_sdl2_fail(const char* operation);

#endif /* IPSYS_HEADER_DBGMSG__ */
