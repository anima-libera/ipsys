
/* Simulation settings.
 * Implementations are in "settings.c". */

#ifndef IPSYS_HEADER_SETTINGS__
#define IPSYS_HEADER_SETTINGS__

#include "opengl.h"

/* Read-only fade factor setting current value. */
extern float g_setting_read_fade_factor;
#define SETTING_FADE_FACTOR_MIN 0.0f
#define SETTING_FADE_FACTOR_MAX 0.2f
void setting_set_fade_factor(GLfloat new_fade_factor);

/* Read-only particle display radius setting current value. */
extern float g_setting_read_part_display_radius;
#define SETTING_PART_DISPLAY_RADIUS_MIN 0.0f
#define SETTING_PART_DISPLAY_RADIUS_MAX 0.05f
void setting_set_part_display_radius(GLfloat new_part_radius);

#endif /* IPSYS_HEADER_SETTINGS__ */
