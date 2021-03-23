
#version 430 core
layout(location = 0) uniform float fade_f;
out vec4 out_color;

void main()
{
	out_color = vec4(fade_f, fade_f, fade_f, 1.0);
}
