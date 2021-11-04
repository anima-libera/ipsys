
/* This file is overwritten at each compilation.
 * Do not modify, see "embedded.h" or "_comp.py" instead. */

/* Content of "src/shaders/particles.vert". */
const char* g_particles_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in vec3 in_color;\nlayout(location = 2) in float in_angle;\nlayout(location = 3) in vec2 in_oldpos;\nout vec3 v_color;\nout float v_angle;\nout float v_radius;\nout vec2 v_oldpos;\n\n#define DISPLAY_RADIUS 0.006\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_color = in_color;\n\tv_angle = in_angle;\n\tv_radius = DISPLAY_RADIUS;\n\tv_oldpos = in_oldpos;\n}\n";

/* Content of "src/shaders/particles.geom". */
const char* g_particles_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 4) out;\nin vec3 v_color[];\nin float v_angle[];\nin float v_radius[];\nin vec2 v_oldpos[];\nout vec3 g_color;\n#define TAU 6.28318530717\n\nvoid main()\n{\n\tg_color = v_color[0];\n\n\tfloat angle = v_angle[0];\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(v_radius[0] * cos(angle), v_radius[0] * sin(angle), 0.0, 0.0);\n\tEmitVertex();\n\n\tangle += TAU/3.0;\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(v_radius[0] * cos(angle), v_radius[0] * sin(angle), 0.0, 0.0);\n\tEmitVertex();\n\n\tangle += TAU/3.0;\n\tgl_Position = gl_in[0].gl_Position +\n\t\tvec4(v_radius[0] * cos(angle), v_radius[0] * sin(angle), 0.0, 0.0);\n\tEmitVertex();\n\n\tgl_Position = vec4(v_oldpos[0], 0.0, 1.0);\n\tEmitVertex();\n\n\tEndPrimitive();\n}\n";

/* Content of "src/shaders/particles.frag". */
const char* g_particles_frag = "\n#version 430 core\nin vec3 g_color;\nlayout(location = 0) out vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(g_color, 1.0);\n}\n";

/* Content of "src/shaders/ui_simple.vert". */
const char* g_ui_simple_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nlayout(location = 1) in vec3 in_color;\nout vec3 v_color;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos / 800.0 * 2.0 - 1.0, 0.0, 1.0);\n\tv_color = in_color;\n}\n";

/* Content of "src/shaders/ui_simple.frag". */
const char* g_ui_simple_frag = "\n#version 430 core\nin vec3 v_color;\nout vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(v_color, 1.0);\n}\n";

/* Content of "src/shaders/gchars.vert". */
const char* g_gchars_vert = "\n#version 430 core\nlayout(location = 0) in vec4 in_pos_xywh;\nlayout(location = 1) in vec4 in_font_xywh;\nout vec4 v_font_xywh;\n\nvoid main()\n{\n\tgl_Position = in_pos_xywh;\n\tv_font_xywh = in_font_xywh;\n}\n";

/* Content of "src/shaders/gchars.geom". */
const char* g_gchars_geom = "\n#version 430 core\nlayout(location = 0) uniform float u_aspect_ratio;\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 4) out;\nin vec4 v_font_xywh[];\nout vec2 g_font_pos;\n\nvoid main()\n{\n\tfloat x = gl_in[0].gl_Position.x;\n\tfloat y = gl_in[0].gl_Position.y;\n\tfloat w = gl_in[0].gl_Position.z;\n\tfloat h = gl_in[0].gl_Position.w;\n\tfloat f_x = v_font_xywh[0].x;\n\tfloat f_y = v_font_xywh[0].y;\n\tfloat f_w = v_font_xywh[0].z;\n\tfloat f_h = v_font_xywh[0].w;\n\n\tgl_Position = vec4(x, y * u_aspect_ratio, 0.0, 1.0);\n\tg_font_pos = vec2(f_x, f_y);\n\tEmitVertex();\n\n\tgl_Position = vec4(x + w, y * u_aspect_ratio, 0.0, 1.0);\n\tg_font_pos = vec2(f_x + f_w, f_y);\n\tEmitVertex();\n\n\tgl_Position = vec4(x, (y + h) * u_aspect_ratio, 0.0, 1.0);\n\tg_font_pos = vec2(f_x, f_y + f_h);\n\tEmitVertex();\n\n\tgl_Position = vec4(x + w, (y + h) * u_aspect_ratio, 0.0, 1.0);\n\tg_font_pos = vec2(f_x + f_w, f_y + f_h);\n\tEmitVertex();\n\n\tEndPrimitive();\n}\n";

/* Content of "src/shaders/gchars.frag". */
const char* g_gchars_frag = "\n#version 430 core\nlayout(location = 1) uniform sampler2D u_font_texture;\nin vec2 g_font_pos;\nout vec4 out_color;\n\nvoid main()\n{\n\tvec4 font_color = vec4(texture(u_font_texture, g_font_pos));\n\t\n\tif (font_color.r < 0.98)\n\t{\n\t\tdiscard;\n\t}\n\n\tout_color = vec4(font_color.r, font_color.r, font_color.r, 1.0);\n}\n";

/* Content of "src/shaders/fade.vert". */
const char* g_fade_vert = "\n#version 430 core\n\nvoid main()\n{\n\tgl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n}\n";

/* Content of "src/shaders/fade.geom". */
const char* g_fade_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 4) out;\n\nvoid main()\n{\n\tgl_Position = vec4(-1.0, +1.0, 0.0, 0.0);\n\tEmitVertex();\n\tgl_Position = vec4(+1.0, +1.0, 0.0, 0.0);\n\tEmitVertex();\n\tgl_Position = vec4(-1.0, -1.0, 0.0, 0.0);\n\tEmitVertex();\n\tgl_Position = vec4(+1.0, -1.0, 0.0, 0.0);\n\tEmitVertex();\n\tEndPrimitive();\n}\n";

/* Content of "src/shaders/fade.frag". */
const char* g_fade_frag = "\n#version 430 core\nlayout(location = 0) uniform float u_fade_f;\nlayout(location = 0) out vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(u_fade_f, u_fade_f, u_fade_f, 1.0);\n}\n";

/* Content of "src/shaders/texture_fade.vert". */
const char* g_texture_fade_vert = "\n#version 430 core\n\nvoid main()\n{\n\tgl_Position = vec4(0.0, 0.0, 0.0, 0.0);\n}\n";

/* Content of "src/shaders/texture_fade.geom". */
const char* g_texture_fade_geom = "\n#version 430 core\nlayout(points) in;\nlayout(triangle_strip, max_vertices = 4) out;\nout vec2 g_texture_pos;\n\nvoid main()\n{\n\tg_texture_pos = vec2(0.0, 1.0);\n\tgl_Position = vec4(-1.0, +1.0, 0.0, 0.0);\n\tEmitVertex();\n\tg_texture_pos = vec2(1.0, 1.0);\n\tgl_Position = vec4(+1.0, +1.0, 0.0, 0.0);\n\tEmitVertex();\n\tg_texture_pos = vec2(0.0, 0.0);\n\tgl_Position = vec4(-1.0, -1.0, 0.0, 0.0);\n\tEmitVertex();\n\tg_texture_pos = vec2(1.0, 0.0);\n\tgl_Position = vec4(+1.0, -1.0, 0.0, 0.0);\n\tEmitVertex();\n\tEndPrimitive();\n}\n";

/* Content of "src/shaders/texture_fade.frag". */
const char* g_texture_fade_frag = "\n#version 430 core\nlayout(location = 0) uniform float u_fade_f;\nlayout(location = 1) uniform sampler2D u_texture;\nin vec2 g_texture_pos;\nlayout(location = 0) out vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(texture(u_texture, gl_FragCoord.xy / 800.0));\n\tout_color -= vec4(u_fade_f, u_fade_f, u_fade_f, 0.0);\n}\n";

/* Content of "src/shaders/texture_fade_2.vert". */
const char* g_texture_fade_2_vert = "\n#version 430 core\nlayout(location = 0) in vec2 in_pos;\nout vec2 v_texture_pos;\n\nvoid main()\n{\n\tgl_Position = vec4(in_pos, 0.0, 1.0);\n\tv_texture_pos = in_pos.xy;\n}\n";

/* Content of "src/shaders/texture_fade_2.frag". */
const char* g_texture_fade_2_frag = "\n#version 430 core\nlayout(location = 0) uniform float u_fade_f;\nlayout(location = 1) uniform sampler2D u_texture;\nin vec2 v_texture_pos;\nlayout(location = 0) out vec4 out_color;\n\nvoid main()\n{\n\tout_color = vec4(texture(u_texture, gl_FragCoord.xy / 800.0));\n\tout_color -= vec4(u_fade_f, u_fade_f, u_fade_f, 0.0);\n}\n";

/* Content of "src/shaders/iteruniv.comp". */
const char* g_iteruniv_comp = "\n#version 450 core\nlayout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;\n\n/* PIL: Particle Interaction Law */\n#define PIL_STEP_DIST 0.006\n#define PIL_STEP_NUMBER 90\nstruct pil_val_step_t\n{\n\tfloat offset;\n\tfloat slope;\n};\nstruct pil_val_t\n{\n\tpil_val_step_t steps[PIL_STEP_NUMBER];\n};\nstruct pil_t\n{\n\tpil_val_t attraction;\n\tpil_val_t angle;\n\tpil_val_t speed;\n};\n\n/* CTL: Change Type Law */\n#define CTL_NUMBER 3\nstruct ctl_t\n{\n\tuint used;\n\tuint has_speed_min; float speed_min;\n\tuint has_speed_max; float speed_max;\n\tuint has_pressure_min; float pressure_min;\n\tuint has_pressure_max; float pressure_max;\n\tuint has_age_min; uint age_min;\n\tuint has_age_max; uint age_max;\n\tuint new_type; float probability;\n};\n\n/* Particle */\nstruct part_type_t\n{\n\tfloat br, bg, bb; /* color base */\n\tfloat sr, sg, sb; /* color speed */\n\tfloat pr, pg, pb; /* color pressure */\n\tctl_t ctl_array[CTL_NUMBER];\n};\nstruct part_t\n{\n\tfloat x, y; /* Can't be replaced by vec2 */\n\tfloat speed;\n\tfloat angle;\n\tfloat pressure;\n\tfloat r, g, b;\n\tfloat oldx, oldy;\n\tuint type;\n\tuint age;\n};\n\n#define PARTICLE_NUMBER (256 * 6)\n#define PHYSICAL_RADIUS 0.006\n#define MARGIN (PHYSICAL_RADIUS * 0.75)\n\n#define TAU 6.28318530717\n\nlayout(shared, binding = 0) buffer buffer_curr\n{\n\tpart_t curr[PARTICLE_NUMBER];\n};\nlayout(shared, binding = 1) buffer buffer_next\n{\n\tpart_t next[PARTICLE_NUMBER];\n};\nlayout(shared, binding = 2) buffer buffer_type\n{\n\tpart_type_t type_table[];\n};\nlayout(shared, binding = 3) buffer buffer_pil_set\n{\n\tpil_t pil_table[];\n};\nlayout(shared, binding = 4) buffer buffer_info\n{\n\tuint part_number;\n\tuint type_number;\n\tuint change_type_law_number;\n\tuint pil_step_number;\n\tfloat pil_step_dist;\n};\n\nlayout(location = 0) uniform int u_update_position;\n\nfloat rand(vec2 v){\n\treturn fract(sin(dot(v.xy, vec2(12.9898, 78.233))) * 43758.5453);\n}\n\nvoid main()\n{\n\tuint i = gl_GlobalInvocationID.x;\n\tif (i >= PARTICLE_NUMBER) return;\n\t\n\tnext[i] = curr[i];\n\n\tnext[i].age = curr[i].age + 1;\n\n\tvec2 v = curr[i].speed * vec2(cos(curr[i].angle), sin(curr[i].angle));\n\n\tnext[i].pressure = 0.0;\n\n\tfor (uint j = 0; j < PARTICLE_NUMBER; ++j)\n\t{\n\t\tif (j == i) continue;\n\n\t\tvec2 ij_vec = vec2(curr[i].x-curr[j].x, curr[i].y-curr[j].y);\n\t\tfloat d = length(ij_vec);\n\n\t\tif (d < 2.0 * PHYSICAL_RADIUS)\n\t\t{\n\t\t\tnext[i].pressure += 2.0 * PHYSICAL_RADIUS - d;\n\t\t}\n\n\t\tif (d == 0)\n\t\t{\n\t\t\td = 0.000001;\n\t\t\tij_vec = d * vec2(cos(float(i)), sin(float(i)));\n\t\t}\n\n\t\tuint step_index = uint(floor(d / PIL_STEP_DIST));\n\t\tif (step_index < PIL_STEP_NUMBER)\n\t\t{\n\t\t\tuint ij = curr[i].type * type_number + curr[j].type;\n\t\t\tpil_val_step_t s = pil_table[ij].attraction.steps[step_index];\n\t\t\tfloat step_start = float(step_index) * PIL_STEP_DIST;\n\t\t\tfloat f = s.offset + (d - step_start) * s.slope;\n\n\t\t\tfloat angle = atan(ij_vec.y, ij_vec.x);\n\t\t\ts = pil_table[ij].angle.steps[step_index];\n\t\t\tangle += s.offset + (d - step_start) * s.slope;\n\n\t\t\tv += f * vec2(cos(angle), sin(angle));\n\n\t\t\ts = pil_table[ij].speed.steps[step_index];\n\t\t\tfloat speedf = s.offset + (d - step_start) * s.slope;\n\t\t\tif (speedf < 0.0)\n\t\t\t{\n\t\t\t\tv /= -speedf+1.0;\n\t\t\t}\n\t\t\telse if (speedf > 0.0)\n\t\t\t{\n\t\t\t\tv *= speedf+1.0;\n\t\t\t}\n\t\t}\n\t}\n\n\t#define FRICTION_F 0.985\n\tv *= FRICTION_F;\n\n\tnext[i].x = curr[i].x + v.x;\n\tnext[i].y = curr[i].y + v.y;\n\n\t#define BONK_F 1.0\n\tif (next[i].x < -(1.0-MARGIN))\n\t{\n\t\tv.x = +abs(v.x);\n\t\tnext[i].x = -(1.0-MARGIN);\n\t\tv.x *= BONK_F; v.y *= BONK_F;\n\t}\n\telse if (next[i].x > (1.0-MARGIN))\n\t{\n\t\tv.x = -abs(v.x);\n\t\tnext[i].x = (1.0-MARGIN);\n\t\tv.x *= BONK_F; v.y *= BONK_F;\n\t}\n\tif (next[i].y < -(1.0-MARGIN))\n\t{\n\t\tv.y = +abs(v.y);\n\t\tnext[i].y = -(1.0-MARGIN);\n\t\tv.x *= BONK_F; v.y *= BONK_F;\n\t}\n\telse if (next[i].y > (1.0-MARGIN))\n\t{\n\t\tv.y = -abs(v.y);\n\t\tnext[i].y = (1.0-MARGIN);\n\t\tv.x *= BONK_F; v.y *= BONK_F;\n\t}\n\n\tfloat s = length(v);\n\tnext[i].speed = s;\n\tif (s > 0)\n\t{\n\t\tnext[i].angle = atan(v.y, v.x);\n\t}\n\n\tif (u_update_position != 0)\n\t{\n\t\tnext[i].oldx = curr[i].x;\n\t\tnext[i].oldy = curr[i].y;\n\t}\n\n\t#define T type_table[next[i].type]\n\tnext[i].r = T.br + next[i].speed * T.sr + next[i].pressure * T.pr;\n\tnext[i].g = T.bg + next[i].speed * T.sg + next[i].pressure * T.pg;\n\tnext[i].b = T.bb + next[i].speed * T.sb + next[i].pressure * T.pb;\n\t#undef T\n\n\t#if 1\n\t\tfor (uint j = 0; j < CTL_NUMBER; ++j)\n\t\t{\n\t\t\t#define CTL type_table[next[i].type].ctl_array[j]\n\t\t\tif (!(bool(CTL.used)))\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_speed_min) && next[i].speed < CTL.speed_min)\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_speed_max) && next[i].speed > CTL.speed_max)\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_pressure_min) && next[i].pressure < CTL.pressure_min)\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_pressure_max) && next[i].pressure > CTL.pressure_max)\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_age_min) && next[i].age < CTL.age_min)\n\t\t\t\tcontinue;\n\t\t\tif (bool(CTL.has_age_max) && next[i].age > CTL.age_max)\n\t\t\t\tcontinue;\n\t\t\tif (rand(vec2(next[i].x, next[i].y)) < CTL.probability)\n\t\t\t{\n\t\t\t\tnext[i].type = CTL.new_type;\n\t\t\t\tnext[i].age = 0;\n\t\t\t\tbreak;\n\t\t\t}\n\t\t\t#undef CTL\n\t\t}\n\n\t\t#if 0\n\t\t\tif (next[i].age > 100)\n\t\t\t{\n\t\t\t\tnext[i].type = (curr[i].type + 1) % type_number;\n\t\t\t\tnext[i].age = 0;\n\t\t\t}\n\t\t#endif\n\t#endif\n}\n";
