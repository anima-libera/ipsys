
#include "settings.h"
#include "shaders.h"
#include "opengl.h"

float g_setting_read_fade_factor = 0.0f;

void setting_set_fade_factor(GLfloat new_fade_factor)
{
	g_setting_read_fade_factor = new_fade_factor;
	glProgramUniform1f(g_shprog_draw_fade, 0, g_setting_read_fade_factor);
	glProgramUniform1f(g_shprog_draw_texture_fade, 0, g_setting_read_fade_factor);
}

float g_setting_read_part_display_radius = 0.0f;

void setting_set_part_display_radius(GLfloat new_fade_factor)
{
	g_setting_read_part_display_radius = new_fade_factor;
	glProgramUniform1f(g_shprog_draw_particles, 0, g_setting_read_part_display_radius);
}
