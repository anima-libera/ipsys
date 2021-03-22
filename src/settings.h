
/* APSILG header file "settings.h"
 * Read and write universe and global application settings.
 * Implementations are in "settings.c". */

#ifndef __APSILG_header_settings__
#define __APSILG_header_settings__

#include <GL/glew.h>

/* Read-only fade factor setting current value. */
extern float g_setting_read_fade_factor;

/* Set the fade factor setting to the given value. */
void setting_set_fade_factor(GLfloat f);

#endif /* __APSILG_header_settings__ */
