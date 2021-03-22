
#include "settings.h"
#include "shaders.h"
#include <GL/glew.h>

float g_setting_read_fade_factor = 0.0f;

void setting_set_fade_factor(GLfloat f)
{
	glProgramUniform1f(g_shprog_draw_fade, 0, f);
	g_setting_read_fade_factor = f;
}
