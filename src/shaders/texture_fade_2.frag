
#version 430 core
layout(location = 0) uniform float u_fade_f;
layout(location = 1) uniform sampler2D u_texture;
in vec2 v_texture_pos;
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(texture(u_texture, gl_FragCoord.xy / 800.0));
	out_color -= vec4(u_fade_f, u_fade_f, u_fade_f, 0.0);
}
