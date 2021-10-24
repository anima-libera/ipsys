
#include "embedded.h"
#include "opengl.h"
#include <stdio.h>
#include <stdlib.h>

/* Parameters akin to dbg_... can be NULL, but should actually provide
 * information that describes the shader type and/or shader source/sources
 * that end up given to glShaderSource when creating a shader or a shader
 * program (such as a file path or a variable name) for the error message to be
 * more helpful, if any. */

/* Creates an OpenGL shader object with the given GLSL source code, then
 * compiles it and returns its id.
 * Returns 0 if compilation fails. */
static GLuint shader_src_compile(const char* shader_src, GLuint shader_type,
	const char* dbg_info)
{
	GLuint shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, (const GLchar* const*)&shader_src, NULL);
	glCompileShader(shader_id);
	GLint compile_status = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
	if (compile_status == GL_FALSE)
	{
		GLint length = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		if (length == 0)
		{
			fprintf(stderr,
				"OpenGL shader compilation error (%s) "
				"without an OpenGL error message\n",
				(dbg_info != NULL) ? dbg_info : "\0");
			/* Is it even possible to reach that point? */
		}
		else
		{
			GLchar* message = malloc(length * sizeof(GLchar));
			glGetShaderInfoLog(shader_id, length, &length, message);
			if (message[length-1] == '\n')
			{
				/* It looks better without this. */
				message[length-1] = '\0';
			}
			fprintf(stderr,
				"OpenGL shader compilation error (%s): "
				"\x1b[31m\"%s\"\x1b[39m\n",
				(dbg_info != NULL) ? dbg_info : "\0", (const char*)message);
			free(message);
		}
		glDeleteShader(shader_id);
		return 0;
	}
	return shader_id;
}

/* Creates an OpenGL shader program object and attaches the given shaders to
 * it, then links it and returns its id.
 * The src_geom parameter can be NULL as a geometry shader is optional in the
 * OpenGL rendering pipeline. 
 * Returns 0 if the compilation or linkage fails. */
static GLuint shprog_build_vgf(
	const char* src_vert, const char* src_geom, const char* src_frag,
	const char* dbg_vert, const char* dbg_geom, const char* dbg_frag,
	const char* dbg_info)
{
	GLuint shader_vert_id = shader_src_compile(src_vert,
		GL_VERTEX_SHADER, dbg_vert);
	if (shader_vert_id == 0)
	{
		return 0;
	}
	GLuint shader_geom_id;
	if (src_geom != NULL)
	{
		shader_geom_id = shader_src_compile(src_geom,
			GL_GEOMETRY_SHADER, dbg_geom);
		if (shader_geom_id == 0)
		{
			glDeleteShader(shader_vert_id);
			return 0;
		}
	}
	GLuint shader_frag_id = shader_src_compile(src_frag,
		GL_FRAGMENT_SHADER, dbg_frag);
	if (shader_frag_id == 0)
	{
		glDeleteShader(shader_vert_id);
		if (src_geom != NULL)
		{
			glDeleteShader(shader_geom_id);
		}
		return 0;
	}

	GLuint shprog_id = glCreateProgram();
	glAttachShader(shprog_id, shader_vert_id);
	if (src_geom != NULL)
	{
		glAttachShader(shprog_id, shader_geom_id);
	}
	glAttachShader(shprog_id, shader_frag_id);

	glLinkProgram(shprog_id);
	GLint link_status = 0;
	glGetProgramiv(shprog_id, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(shprog_id, GL_INFO_LOG_LENGTH, &length);
		if (length == 0)
		{
			fprintf(stderr,
				"OpenGL shader program linkage error (%s) "
				"without an OpenGL error message\n",
				(dbg_info != NULL) ? dbg_info : "\0");
			/* Is it even possible to reach that point? */
		}
		else
		{
			GLchar* message = malloc(length * sizeof(GLchar));
			glGetProgramInfoLog(shprog_id, length, &length, message);
			if (message[length-1] == '\n')
			{
				/* It looks better without this. */
				message[length-1] = '\0';
			}
			fprintf(stderr,
				"OpenGL shader program linkage error (%s): "
				"\x1b[31m\"%s\"\x1b[39m\n",
				(dbg_info != NULL) ? dbg_info : "\0", (const char*)message);
			free(message);
		}
		glDetachShader(shprog_id, shader_vert_id);
		glDeleteShader(shader_vert_id);
		if (src_geom != NULL)
		{
			glDetachShader(shprog_id, shader_geom_id);
			glDeleteShader(shader_geom_id);
		}
		glDetachShader(shprog_id, shader_frag_id);
		glDeleteShader(shader_frag_id);
		glDeleteProgram(shprog_id);
		return 0;
	}

	glDetachShader(shprog_id, shader_vert_id);
	glDeleteShader(shader_vert_id);
	if (src_geom != NULL)
	{
		glDetachShader(shprog_id, shader_geom_id);
		glDeleteShader(shader_geom_id);
	}
	glDetachShader(shprog_id, shader_frag_id);
	glDeleteShader(shader_frag_id);
	return shprog_id;
}

#if 1
/* Creates an OpenGL shader program object and attaches the given compute
 * shader to it, then links it and returns its id.
 * Returns 0 if the compilation or linkage fails. */
static GLuint shprog_build_comp(const char* src_comp, const char* dbg_info)
{
	GLuint shader_comp_id = shader_src_compile(src_comp,
		GL_COMPUTE_SHADER, dbg_info);
	if (shader_comp_id == 0)
	{
		return 0;
	}
	GLuint shprog_id = glCreateProgram();
	glAttachShader(shprog_id, shader_comp_id);
	glLinkProgram(shprog_id);
	GLint link_status = 0;
	glGetProgramiv(shprog_id, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		GLint length = 0;
		glGetProgramiv(shprog_id, GL_INFO_LOG_LENGTH, &length);
		if (length == 0)
		{
			fprintf(stderr,
				"OpenGL shader program linkage error (%s) "
				"without an OpenGL error message\n",
				(dbg_info != NULL) ? dbg_info : "\0");
			/* Is it even possible to reach that point? */
		}
		else
		{
			GLchar* message = malloc(length * sizeof(GLchar));
			glGetProgramInfoLog(shprog_id, length, &length, message);
			if (message[length-1] == '\n')
			{
				/* It looks better without this. */
				message[length-1] = '\0';
			}
			fprintf(stderr,
				"OpenGL shader program linkage error (%s): "
				"\x1b[31m\"%s\"\x1b[39m\n",
				(dbg_info != NULL) ? dbg_info : "\0", (const char*)message);
			free(message);
		}
		glDetachShader(shprog_id, shader_comp_id);
		glDeleteShader(shader_comp_id);
		glDeleteProgram(shprog_id);
		return 0;
	}
	glDetachShader(shprog_id, shader_comp_id);
	glDeleteShader(shader_comp_id);
	return shprog_id;
}
#endif

GLuint g_shprog_draw_particles = 0;
GLuint g_shprog_draw_ui_simple = 0;
GLuint g_shprog_draw_fade = 0;
GLuint g_shprog_draw_texture_fade = 0;
GLuint g_shprog_draw_texture_fade_2 = 0;
GLuint g_shprog_comp_iteruniv = 0;

int shprog_build_all(void)
{
	#define BUILD_VGF(shprog_, vert_, geom_, frag_) \
		do \
		{ \
			shprog_ = shprog_build_vgf(vert_, geom_, frag_, \
				#vert_, #geom_, #frag_, #shprog_); \
			if (shprog_ == 0) \
			{ \
				return -1; \
			} \
		} while (0)
	#define BUILD_COMP(shprog_, comp_) \
		do \
		{ \
			shprog_ = shprog_build_comp(comp_, #comp_ ", " #shprog_); \
			if (shprog_ == 0) \
			{ \
				return -1; \
			} \
		} while (0)

	BUILD_VGF(g_shprog_draw_particles,
		g_particles_vert, g_particles_geom, g_particles_frag);
	BUILD_VGF(g_shprog_draw_ui_simple,
		g_ui_simple_vert, NULL, g_ui_simple_frag);
	BUILD_VGF(g_shprog_draw_fade,
		g_fade_vert, g_fade_geom, g_fade_frag);
	BUILD_VGF(g_shprog_draw_texture_fade,
		g_texture_fade_vert, g_texture_fade_geom, g_texture_fade_frag);
	BUILD_VGF(g_shprog_draw_texture_fade_2,
		g_texture_fade_2_vert, NULL, g_texture_fade_2_frag);

	BUILD_COMP(g_shprog_comp_iteruniv,
		g_iteruniv_comp);

	#undef BUILD_VGF
	#undef BUILD_COMP
	return 0;
}

void shprog_destroy_all(void)
{
	#define CLEANUP_SHPROG(shprog_) \
		do \
		{ \
			if (shprog_ != 0) \
			{ \
				glDeleteProgram(shprog_); \
				shprog_ = 0; \
			} \
		} while (0)

	CLEANUP_SHPROG(g_shprog_draw_particles);
	CLEANUP_SHPROG(g_shprog_draw_ui_simple);
	CLEANUP_SHPROG(g_shprog_draw_fade);
	CLEANUP_SHPROG(g_shprog_draw_texture_fade);
	CLEANUP_SHPROG(g_shprog_draw_texture_fade_2);

	CLEANUP_SHPROG(g_shprog_comp_iteruniv);

	#undef CLEANUP_SHPROG
}
