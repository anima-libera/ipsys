
#version 430 core
layout(location = 0) uniform float u_fade_f;
out vec4 out_color;

void main()
{
	out_color = vec4(u_fade_f, u_fade_f, u_fade_f, 1.0);
}
