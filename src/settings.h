
/* Simulation settings.
 * Implementations are in "settings.c". */

#ifndef IPSYS_HEADER_SETTINGS__
#define IPSYS_HEADER_SETTINGS__

#include <GL/glew.h>

/* Read-only fade factor setting current value. */
extern float g_setting_read_fade_factor;
#define SETTING_FADE_FACTOR_MIN 0.0f
#define SETTING_FADE_FACTOR_MAX 0.2f

void setting_set_fade_factor(GLfloat new_fade_factor);

#endif /* IPSYS_HEADER_SETTINGS__ */
