
#version 430 core
layout(location = 0) uniform float u_part_radius;
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in vec3 v_color[];
in float v_angle[];
in vec2 v_oldpos[];
out vec3 g_color;
#define TAU 6.28318530717

void main()
{
	g_color = v_color[0];

	float angle = v_angle[0];
	gl_Position = gl_in[0].gl_Position +
		vec4(u_part_radius * cos(angle), u_part_radius * sin(angle), 0.0, 0.0);
	EmitVertex();

	angle += TAU/3.0;
	gl_Position = gl_in[0].gl_Position +
		vec4(u_part_radius * cos(angle), u_part_radius * sin(angle), 0.0, 0.0);
	EmitVertex();

	angle += TAU/3.0;
	gl_Position = gl_in[0].gl_Position +
		vec4(u_part_radius * cos(angle), u_part_radius * sin(angle), 0.0, 0.0);
	EmitVertex();

	gl_Position = vec4(v_oldpos[0], 0.0, 1.0);
	EmitVertex();

	EndPrimitive();
}
