
#version 430 core
layout(location = 0) in vec2 in_pos;
out vec2 v_texture_pos;

void main()
{
	gl_Position = vec4(in_pos, 0.0, 1.0);
	v_texture_pos = in_pos.xy;
}
