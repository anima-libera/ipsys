
#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
out vec2 g_texture_pos;

void main()
{
	g_texture_pos = vec2(0.0, 1.0);
	gl_Position = vec4(-1.0, +1.0, 0.0, 0.0);
	EmitVertex();
	g_texture_pos = vec2(1.0, 1.0);
	gl_Position = vec4(+1.0, +1.0, 0.0, 0.0);
	EmitVertex();
	g_texture_pos = vec2(0.0, 0.0);
	gl_Position = vec4(-1.0, -1.0, 0.0, 0.0);
	EmitVertex();
	g_texture_pos = vec2(1.0, 0.0);
	gl_Position = vec4(+1.0, -1.0, 0.0, 0.0);
	EmitVertex();
	EndPrimitive();
}
