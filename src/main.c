
#include "utils.h"
#include "dbgmsg.h"
#include "random.h"
#include "shaders.h"
#include "universe.h"
#include "settings.h"
#include "serialization.h"
#include "opengl.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#ifdef cplusplus__
#error C compiler required
#endif

struct __attribute__((packed)) ui_vertex_t
{
	GLfloat x, y;
	GLfloat r, g, b;
};
typedef struct ui_vertex_t ui_vertex_t;

struct __attribute__((packed)) just_vertex_t
{
	GLfloat x, y;
};
typedef struct just_vertex_t just_vertex_t;

SDL_Window* g_window = NULL;
SDL_GLContext g_opengl_context = NULL;

struct font_t
{
	unsigned int texture_side;
	unsigned char* texture_data;
	GLuint texture_id;
};
typedef struct font_t font_t;

/* Graphical character, one that is displayed on screen. */
struct gchar_t
{
	float x, y, w, h;
	float font_x, font_y, font_w, font_h;
};
typedef struct gchar_t gchar_t;

/* An OpenGL buffer of graphical characters. */
struct text_layer_t
{
	unsigned int gchar_maximum_number;
	unsigned int gchar_number;
	gchar_t* gchar_array;
	GLuint buf_gchars_id;
};
typedef struct text_layer_t text_layer_t;

#if 0
enum widget_type_t
{
	WIDGET_SLIDER,
	WIDGET_BUTTON,
};
typedef enum widget_type_t widget_type_t;

struct widget_slider_t
{
	int uwu;
};
typedef struct widget_slider_t widget_slider_t;

struct widget_button_t
{
	int uwu;
};
typedef struct widget_button_t widget_button_t;

struct widget_t
{
	float w, h;
	widget_type_t type;
	union
	{
		widget_slider_t slider;
		widget_button_t button;
	};
};
typedef struct widget_t widget_t;
#endif

struct uip_block_t
{
	unsigned int len;
	unsigned int index;
};
typedef struct uip_block_t uip_block_t;

typedef void (*drawcall_callback_t)(GLuint opengl_buffer_id, unsigned int prim_count);

struct uipt_t
{
	unsigned int block_da_len;
	unsigned int block_da_cap;
	uip_block_t* block_da_arr;
	unsigned int prim_da_len;
	unsigned int prim_da_cap;
	void* prim_da_arr;
	GLuint prim_opengl_buffer_id;
	int needs_prim_buffer_sync;
	int needs_prim_buffer_sync_alloc;
	unsigned int sizeof_prim;
	drawcall_callback_t drawcall_callback;
};
typedef struct uipt_t uipt_t;

struct ui_line_t
{
	ui_vertex_t a, b;
};
typedef struct ui_line_t ui_line_t;

struct ui_triangle_t
{
	ui_vertex_t a, b, c;
};
typedef struct ui_triangle_t ui_triangle_t;

void uipt_init(uipt_t* uipt, unsigned int sizeof_prim, drawcall_callback_t drawcall_callback)
{
	uipt->block_da_len = 0;
	uipt->block_da_cap = 0;
	uipt->block_da_arr = NULL;
	uipt->prim_da_len = 0;
	uipt->prim_da_cap = 0;
	uipt->prim_da_arr = NULL;
	glGenBuffers(1, &uipt->prim_opengl_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uipt->prim_opengl_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, 0, uipt->prim_da_arr, GL_DYNAMIC_DRAW);
	uipt->needs_prim_buffer_sync = 1;
	uipt->needs_prim_buffer_sync_alloc = 0;
	uipt->sizeof_prim = sizeof_prim;
	uipt->drawcall_callback = drawcall_callback;
}

unsigned int uipt_alloc_prim_block(uipt_t* uipt, unsigned int len)
{
	TYPE_DA_LENGTHEN(uipt->prim_da_len += len, uipt->prim_da_cap, uipt->prim_da_arr,
		void, uipt->sizeof_prim);
	unsigned int prim_i = uipt->prim_da_len - len;

	unsigned int block_i;
	for (block_i = 0; block_i < uipt->block_da_len; block_i++)
	{
		if (uipt->block_da_arr[block_i].len == 0)
		{
			goto l_block_i_found;
		}
	}
	DA_LENGTHEN(uipt->block_da_len += 1, uipt->block_da_cap, uipt->block_da_arr, uip_block_t);
	block_i = uipt->block_da_len - 1;

	l_block_i_found:
	
	uipt->block_da_arr[block_i].len = len;
	uipt->block_da_arr[block_i].index = prim_i;

	uipt->needs_prim_buffer_sync_alloc = 1;

	return block_i;
}

void* uipt_get_prim_block(uipt_t* uipt, unsigned int block_index)
{
	return (char*)uipt->prim_da_arr + uipt->block_da_arr[block_index].index * uipt->sizeof_prim;
}

void uipt_draw(uipt_t* uipt)
{
	if (uipt->needs_prim_buffer_sync_alloc)
	{
		glBindBuffer(GL_ARRAY_BUFFER, uipt->prim_opengl_buffer_id);
		glBufferData(GL_ARRAY_BUFFER,
			uipt->prim_da_len * uipt->sizeof_prim, uipt->prim_da_arr,
			GL_DYNAMIC_DRAW);
	}
	else if (uipt->needs_prim_buffer_sync)
	{
		glBindBuffer(GL_ARRAY_BUFFER, uipt->prim_opengl_buffer_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			uipt->prim_da_len * uipt->sizeof_prim, uipt->prim_da_arr);
	}
	uipt->needs_prim_buffer_sync_alloc = 0;
	uipt->needs_prim_buffer_sync = 0;

	uipt->drawcall_callback(uipt->prim_opengl_buffer_id, uipt->prim_da_len);
}

void uipt_needs_sync(uipt_t* uipt)
{
	uipt->needs_prim_buffer_sync = 1;
}

void line_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count)
{
	#define ATTRIB_LOCATION_POS 0
	#define ATTRIB_LOCATION_COLOR 1

	glViewport(800, 0, 800, 800);
	glUseProgram(g_shprog_draw_ui_simple);
	glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
	glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, opengl_buffer_id);
	glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
		GL_FALSE, sizeof(ui_vertex_t),
		(void*)offsetof(ui_vertex_t, x));
	glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
		GL_FALSE, sizeof(ui_vertex_t),
		(void*)offsetof(ui_vertex_t, r));
	glDrawArrays(GL_LINES, 0, prim_count * 2);
	
	glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
	glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
	glUseProgram(0);

	#undef ATTRIB_LOCATION_POS
	#undef ATTRIB_LOCATION_COLOR
}

void triangle_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count)
{
	#define ATTRIB_LOCATION_POS 0
	#define ATTRIB_LOCATION_COLOR 1

	glViewport(800, 0, 800, 800);
	glUseProgram(g_shprog_draw_ui_simple);
	glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
	glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, opengl_buffer_id);
	glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
		GL_FALSE, sizeof(ui_vertex_t),
		(void*)offsetof(ui_vertex_t, x));
	glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
		GL_FALSE, sizeof(ui_vertex_t),
		(void*)offsetof(ui_vertex_t, r));
	glDrawArrays(GL_TRIANGLES, 0, prim_count * 3);
	
	glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
	glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
	glUseProgram(0);

	#undef ATTRIB_LOCATION_POS
	#undef ATTRIB_LOCATION_COLOR
}

int main(int argc, const char** argv)
{
	l_beginning:;

	/* Parse command line arguments. */

	const char* arg_type_number = NULL;
	const char* arg_ipsysd_filepath = NULL;

	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			assert(0);
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			arg_ipsysd_filepath = argv[++i];
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			arg_type_number = argv[++i];
		}
		else
		{
			fprintf(stderr, "The command line parser is overwhelmed! "
				"It is still young..\n");
			break;
		}
	}

	int type_number = -1;
	if (arg_type_number != NULL)
	{
		unsigned int i;
		for (i = 0; arg_type_number[i] != '\0'; i++)
		{
			char c = arg_type_number[i];
			if (c < '0' || '9' < c)
			{
				fprintf(stderr, "The type number command line argument "
					"contains non-decimal-digit characters.\n");
				goto l_after_type_number;
			}
		}
		if (i == 0)
		{
			fprintf(stderr, "The type number command line argument "
				"is empty.\n");
			goto l_after_type_number;
		}
		type_number = 0;
		for (i = 0; arg_type_number[i] != '\0'; i++)
		{
			type_number = type_number * 10 + (arg_type_number[i] - '0');
		}
		if (type_number == 0)
		{
			fprintf(stderr, "The type number command line argument "
				"is zero.\n");
			type_number = -1;
			goto l_after_type_number;
		}
		printf("type number is %d\n", type_number);
	}
	l_after_type_number:

	/* Initialize SDL2 and OpenGL. */

	/* Initialize the SDL2 library and the GLEW OpenGL extension loader.
	 * Create the unique global window g_window and the unique global OpenGL
	 * context g_opengl_context. */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		error_sdl2_fail("SDL_Init");
		return EXIT_FAILURE;
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	g_window = SDL_CreateWindow("ipsys",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 800,
		SDL_WINDOW_OPENGL);
	if (g_window == NULL)
	{
		error_sdl2_fail("SDL_CreateWindow");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MAJOR_VERSION");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MINOR_VERSION");
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_PROFILE_MASK");
		return EXIT_FAILURE;
	}
	g_opengl_context = SDL_GL_CreateContext(g_window);
	if (g_opengl_context == NULL)
	{
		error_sdl2_fail("SDL_GL_CreateContext");
		return EXIT_FAILURE;
	}
	#if USE_GLEW
		GLenum glew_init_result = glewInit();
		if (glew_init_result != GLEW_OK)
		{
			fprintf(stderr, "GLEW error: glewInit failed: \"%s\"\n",
				glewGetErrorString(glew_init_result));
		}
		fprintf(stderr, "Using GLEW\n");
	#else
		fprintf(stderr, "Not using GLEW\n");
	#endif
	glEnable(GL_MULTISAMPLE);
	if (SDL_GL_SetSwapInterval(-1) != 0)
	{
		SDL_GL_SetSwapInterval(1);
	}

	enable_opengl_dbgmsg();

	GLuint vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	if (shprog_build_all() != 0)
	{
		return EXIT_FAILURE;
	}

	/* Universe setup. */

	rg_t rg;
	rg_time_seed(&rg);

	universe_info_t info = {0};
	part_type_t* type_table = NULL;
	pil_set_t* pil_set_table = NULL;

	if (arg_ipsysd_filepath != NULL)
	{
		//universe_info_t info;
		//pil_set_t* pil_set_table;
		//part_type_t* type_table;
		deserialize_ipsysd_file(arg_ipsysd_filepath,
			&info, &pil_set_table, &type_table);

		info.change_type_law_number = CHANGE_TYPE_LAW_NUMBER;
		disable_change_laws(type_table,
			info.type_number, CHANGE_TYPE_LAW_NUMBER);
	}
	else
	{
		unsigned int tn =
			type_number >= 1 ? type_number :
			rg_int(&rg, 0, 3) != 0 ? 2 :
			rg_int(&rg, 1, 4);
		info.part_number = (256 * 6);
		info.type_number = tn;
		info.change_type_law_number = CHANGE_TYPE_LAW_NUMBER;
		info.pil_step_number = PIL_STEP_NUMBER;
		info.pil_step_dist = 0.006f;

		type_table = malloc(tn * sizeof(part_type_t));
		randomize_colors(type_table, tn, &rg);
		//randomize_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER, &rg);
		disable_change_laws(type_table, tn, CHANGE_TYPE_LAW_NUMBER);

		pil_set_table = malloc(tn*tn * sizeof(pil_set_t));
		randomize_pils(pil_set_table, tn, &rg);
	}

	unsigned int tn = info.type_number;
	unsigned int tnu = tn;

	GLuint buf_info_id;
	glGenBuffers(1, &buf_info_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_info_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof info,
		&info, GL_STATIC_DRAW);

	GLuint buf_type_id;
	glGenBuffers(1, &buf_type_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
	glBufferData(GL_ARRAY_BUFFER, tn * sizeof(part_type_t),
		type_table, GL_STATIC_DRAW);
	
	GLuint buf_pil_set_id;
	glGenBuffers(1, &buf_pil_set_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
	glBufferData(GL_ARRAY_BUFFER, tn*tn * sizeof(pil_set_t),
		pil_set_table, GL_STATIC_DRAW);
	
	#define PARTICLE_NUMBER (256 * 6)
	part_t part_array[PARTICLE_NUMBER] = {0};

	randomize_parts(part_array, PARTICLE_NUMBER, tnu, &rg);

	GLuint buf_part_curr_id;
	glGenBuffers(1, &buf_part_curr_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof part_array,
		part_array, GL_DYNAMIC_DRAW);

	GLuint buf_part_next_id;
	glGenBuffers(1, &buf_part_next_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_part_next_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof part_array,
		part_array, GL_DYNAMIC_DRAW);

	#define WORK_GROUP_SIZE 256

	setting_set_fade_factor(0.05f);

	#define MAX_ITER_PER_FRAME 32
	unsigned int iteration_number_per_frame = 4;

	/* Universe rendering setup. */

	int render_each_iteration = 1;

	int no_fading = 0;

	unsigned int univ_rendering_index = 0;
	GLuint univ_texture_double_id[2];
	glGenTextures(2, univ_texture_double_id);
	GLuint univ_fbo_double_id[2];
	glGenFramebuffers(2, univ_fbo_double_id);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, univ_texture_double_id[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, univ_fbo_double_id[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, univ_texture_double_id[i], 0);
		const GLenum draw_buffer_array[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, draw_buffer_array);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(0);
			/* TODO: Get a true error message. */
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	just_vertex_t univ_full_rect_array[4] = {
		{.x = +1.0f, .y = +1.0f},
		{.x = -1.0f, .y = +1.0f},
		{.x = +1.0f, .y = -1.0f},
		{.x = -1.0f, .y = -1.0f},
	};

	GLuint buf_univ_full_rect_id;
	glGenBuffers(1, &buf_univ_full_rect_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_univ_full_rect_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof univ_full_rect_array,
		univ_full_rect_array, GL_STATIC_DRAW);

	/* Ui primitive tables setup. */

	uipt_t ui_line_table;
	uipt_init(&ui_line_table, sizeof(ui_line_t), line_drawcall_callback);
	unsigned int line_block_index = uipt_alloc_prim_block(&ui_line_table, 4);
	ui_line_t* line_block = uipt_get_prim_block(&ui_line_table, line_block_index);
	line_block[0].a = (ui_vertex_t){.x = 200.5f, .y = 200.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[0].b = (ui_vertex_t){.x = 600.5f, .y = 200.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].a = (ui_vertex_t){.x = 600.5f, .y = 200.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].b = (ui_vertex_t){.x = 600.5f, .y = 600.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].a = (ui_vertex_t){.x = 600.5f, .y = 600.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].b = (ui_vertex_t){.x = 200.5f, .y = 600.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].a = (ui_vertex_t){.x = 200.5f, .y = 600.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].b = (ui_vertex_t){.x = 200.5f, .y = 200.5f, .r = 1.0f, .g = 1.0f, .b = 1.0f};

	uipt_t ui_triangle_table;
	uipt_init(&ui_triangle_table, sizeof(ui_triangle_t), triangle_drawcall_callback);
	unsigned int triangle_block_index = uipt_alloc_prim_block(&ui_triangle_table, 2);
	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_triangle_table, triangle_block_index);
	triangle_block[0].a = (ui_vertex_t){.x = 200.5f, .y = 200.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};
	triangle_block[0].b = (ui_vertex_t){.x = 200.5f, .y = 600.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};
	triangle_block[0].c = (ui_vertex_t){.x = 600.5f, .y = 600.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};
	triangle_block[1].a = (ui_vertex_t){.x = 200.5f, .y = 200.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};
	triangle_block[1].b = (ui_vertex_t){.x = 600.5f, .y = 600.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};
	triangle_block[1].c = (ui_vertex_t){.x = 600.5f, .y = 200.5f, .r = 0.0f, .g = 0.3f, .b = 0.3f};

	/* Font and text setup. */

	#define FONT_TEXTURE_SIDE 256
	unsigned char* font_texture_data = calloc(FONT_TEXTURE_SIDE * FONT_TEXTURE_SIDE, 1);
	font_texture_data[2 + 16 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[1 + 15 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[3 + 15 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 + 14 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 + 14 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 + 13 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 + 13 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[1 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[2 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[3 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 + 11 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 + 11 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 + 10 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 + 10 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[0 +  9 * FONT_TEXTURE_SIDE] = 255;
	font_texture_data[4 +  9 * FONT_TEXTURE_SIDE] = 255;

	GLuint font_texture_id;
	glGenTextures(1, &font_texture_id);
	glBindTexture(GL_TEXTURE_2D, font_texture_id);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RED, FONT_TEXTURE_SIDE, FONT_TEXTURE_SIDE, 0, GL_RED, GL_UNSIGNED_BYTE, font_texture_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned int gchar_maximum_number;
	unsigned int gchar_number;
	gchar_t* gchar_array;
	GLuint buf_gchars_id;

	gchar_maximum_number = 16;
	gchar_number = 0;
	gchar_array = calloc(gchar_maximum_number, sizeof(gchar_t));
	gchar_t* new_gchar;
	new_gchar = &gchar_array[gchar_number++];
	new_gchar->x = (((6.0f))/800.0f)*2.0f-1.0f;
	new_gchar->y = ((800.0f-(49.0f))/800.0f)*2.0f-1.0f;
	new_gchar->w = 0.08f;
	new_gchar->h = 0.08f;
	new_gchar->font_x = 0.0f;
	new_gchar->font_y = 0.0f;
	new_gchar->font_w = 0.1f;
	new_gchar->font_h = 0.1f;
	new_gchar = &gchar_array[gchar_number++];
	new_gchar->x = (((6.0f + 8.0f))/800.0f)*2.0f-1.0f;
	new_gchar->y = ((800.0f-(49.0f))/800.0f)*2.0f-1.0f;
	new_gchar->w = 0.08f;
	new_gchar->h = 0.08f;
	new_gchar->font_x = 0.0f;
	new_gchar->font_y = 0.0f;
	new_gchar->font_w = 0.1f;
	new_gchar->font_h = 0.1f;
	glGenBuffers(1, &buf_gchars_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_gchars_id);
	glBufferData(GL_ARRAY_BUFFER, gchar_maximum_number * sizeof(gchar_t),
		gchar_array, GL_DYNAMIC_DRAW);

	/* UI setup. */

	const float ui_margin = 6.5f;
	const float ui_size = 20.0f;
	const float ui_length = (800.0f - ui_margin) - ui_margin;

	#define BAR_NUMBER 2

	ui_vertex_t ui_line_vertex_array[BAR_NUMBER * 4] = {
		{.x = ui_margin,          .y = 800.0f - ui_margin          },
		{.x = 800.0f - ui_margin, .y = 800.0f - ui_margin          },
		{.x = 800.0f - ui_margin, .y = 800.0f - ui_margin - ui_size},
		{.x = ui_margin,          .y = 800.0f - ui_margin - ui_size},
	};

	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);
		ui_line_vertex_array[i * 4 + 0].x = ui_margin;
		ui_line_vertex_array[i * 4 + 1].x = 800.0f - ui_margin;
		ui_line_vertex_array[i * 4 + 2].x = 800.0f - ui_margin;
		ui_line_vertex_array[i * 4 + 3].x = ui_margin;
		ui_line_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
		ui_line_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
		ui_line_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
		ui_line_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;
	}

	for (unsigned int i = 0; i < BAR_NUMBER * 4; i++)
	{
		ui_line_vertex_array[i].r = 1.0f;
		ui_line_vertex_array[i].g = 1.0f;
		ui_line_vertex_array[i].b = 1.0f;
	}

	GLuint buf_ui_line_id;
	glGenBuffers(1, &buf_ui_line_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_line_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof ui_line_vertex_array,
		ui_line_vertex_array, GL_STATIC_DRAW);

	GLuint ui_line_vertex_index_array[BAR_NUMBER * 8];
	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		ui_line_vertex_index_array[i * 8 + 0] = i * 4 + 0;
		ui_line_vertex_index_array[i * 8 + 1] = i * 4 + 1;
		ui_line_vertex_index_array[i * 8 + 2] = i * 4 + 1;
		ui_line_vertex_index_array[i * 8 + 3] = i * 4 + 2;
		ui_line_vertex_index_array[i * 8 + 4] = i * 4 + 2;
		ui_line_vertex_index_array[i * 8 + 5] = i * 4 + 3;
		ui_line_vertex_index_array[i * 8 + 6] = i * 4 + 3;
		ui_line_vertex_index_array[i * 8 + 7] = i * 4 + 0;
	}

	GLuint buf_ui_line_index_id;
	glGenBuffers(1, &buf_ui_line_index_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_line_index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ui_line_vertex_index_array,
		ui_line_vertex_index_array, GL_STATIC_DRAW);

	ui_vertex_t ui_rect_vertex_array[BAR_NUMBER * 4];

	float ui_rect_filled_x[BAR_NUMBER] = {
		ui_length * (1.0f - g_setting_read_fade_factor / SETTING_FADE_FACTOR_MAX),
		ui_length * (1.0f - ((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f)),
	};

	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);
		ui_rect_vertex_array[i * 4 + 0].x = 800.0f - ui_margin - ui_rect_filled_x[i];
		ui_rect_vertex_array[i * 4 + 1].x = ui_margin;
		ui_rect_vertex_array[i * 4 + 2].x = 800.0f - ui_margin - ui_rect_filled_x[i];
		ui_rect_vertex_array[i * 4 + 3].x = ui_margin;
		ui_rect_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
		ui_rect_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
		ui_rect_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
		ui_rect_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;
	}

	for (unsigned int i = 0; i < BAR_NUMBER * 4; i++)
	{
		ui_rect_vertex_array[i].r = 0.02f;
		ui_rect_vertex_array[i].g = 0.8f;
		ui_rect_vertex_array[i].b = 0.4f;
	}

	GLuint buf_ui_rect_id;
	glGenBuffers(1, &buf_ui_rect_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof ui_rect_vertex_array,
		ui_rect_vertex_array, GL_STATIC_DRAW);

	GLuint ui_rect_vertex_index_array[BAR_NUMBER * 6];
	for (unsigned int i = 0; i < BAR_NUMBER; i++)
	{
		ui_rect_vertex_index_array[i * 6 + 0] = i * 4 + 0;
		ui_rect_vertex_index_array[i * 6 + 1] = i * 4 + 1;
		ui_rect_vertex_index_array[i * 6 + 2] = i * 4 + 2;
		ui_rect_vertex_index_array[i * 6 + 3] = i * 4 + 1;
		ui_rect_vertex_index_array[i * 6 + 4] = i * 4 + 2;
		ui_rect_vertex_index_array[i * 6 + 5] = i * 4 + 3;
	}

	GLuint buf_ui_rect_index_id;
	glGenBuffers(1, &buf_ui_rect_index_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_rect_index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ui_rect_vertex_index_array,
		ui_rect_vertex_index_array, GL_STATIC_DRAW);

	ui_vertex_t ui_bg_vertex_array[4] = {
		{.x = 800.0f, .y = 800.0f},
		{.x =   0.0f, .y = 800.0f},
		{.x = 800.0f, .y =   0.0f},
		{.x =   0.0f, .y =   0.0f},
	};

	for (unsigned int i = 0; i < 4; i++)
	{
		ui_bg_vertex_array[i].r = 0.0f;
		ui_bg_vertex_array[i].g = 0.05f;
		ui_bg_vertex_array[i].b = 0.2f;
	}

	GLuint buf_ui_bg_id;
	glGenBuffers(1, &buf_ui_bg_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ui_bg_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof ui_bg_vertex_array,
		ui_bg_vertex_array, GL_STATIC_DRAW);

	/* Core loop. */

	unsigned int t = 0;
	int running = 1;
	int restart = 0;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = 0;
				break;

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.x >= 800)
					{
						const float x = event.button.x - 800;
						const float y = 800 - event.button.y;

						for (unsigned int i = 0; i < BAR_NUMBER; i++)
						{
							float ui_setup_y = 800.0f - (float)i * (2.0f * ui_margin + ui_size);

							if (ui_margin <= x &&
								x <= 800.0f - ui_margin &&
								ui_setup_y - ui_margin - ui_size <= y &&
								y <= ui_setup_y - ui_margin)
							{
								if (i == 0)
								{
									const float value =
										(x - ui_margin) / ui_length *
										SETTING_FADE_FACTOR_MAX;
									setting_set_fade_factor(value);

									ui_rect_filled_x[0] =
										ui_length *
										(1.0f - g_setting_read_fade_factor /
											SETTING_FADE_FACTOR_MAX);
								}
								else if (i == 1)
								{
									const unsigned int value =
										roundf((x - ui_margin) / ui_length * ((float)(MAX_ITER_PER_FRAME) - 1.0f) + 1.0f);
									iteration_number_per_frame = value;
								
									ui_rect_filled_x[1] =
										ui_length *
										(1.0f - ((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f));
								}

								ui_rect_vertex_array[i * 4 + 0].x = 800.0f - ui_margin - ui_rect_filled_x[i];
								ui_rect_vertex_array[i * 4 + 1].x = ui_margin;
								ui_rect_vertex_array[i * 4 + 2].x = 800.0f - ui_margin - ui_rect_filled_x[i];
								ui_rect_vertex_array[i * 4 + 3].x = ui_margin;
								ui_rect_vertex_array[i * 4 + 0].y = ui_setup_y - ui_margin;
								ui_rect_vertex_array[i * 4 + 1].y = ui_setup_y - ui_margin;
								ui_rect_vertex_array[i * 4 + 2].y = ui_setup_y - ui_margin - ui_size;
								ui_rect_vertex_array[i * 4 + 3].y = ui_setup_y - ui_margin - ui_size;

								glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
								glBufferData(GL_ARRAY_BUFFER,
									BAR_NUMBER * 4 * sizeof(ui_vertex_t),
									ui_rect_vertex_array, GL_STATIC_DRAW);
							}
						}
					}
					else
					{
						float sign;
						if (event.button.button == SDL_BUTTON_LEFT)
						{
							sign = 1.0f;
						}
						else if (event.button.button == SDL_BUTTON_RIGHT)
						{
							sign = -1.0f;
						}
						else
						{
							break;
						}
						float cx = (float)event.button.x / 400.0f - 1.0f;
						float cy = -((float)event.button.y / 400.0f - 1.0f);
						glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
						part_t* part_array = glMapBuffer(GL_ARRAY_BUFFER,
							GL_READ_WRITE);
						for (unsigned int i = 0; i < PARTICLE_NUMBER; i++)
						{
							float x = part_array[i].x;
							float y = part_array[i].y;
							float square_dist = (x-cx)*(x-cx) + (y-cy)*(y-cy);
							if (square_dist >= 0.3f * 0.3f)
							{
								continue;
							}
							float speed = part_array[i].speed;
							float angle = part_array[i].angle;
							float vx = cosf(angle) * speed;
							float vy = sinf(angle) * speed;
							if (square_dist < 0.05f * 0.05f)
							{
								if (sign < 0.0f)
								{
									square_dist = 0.05f;
								}
								else
								{
									square_dist = 0.05f * 0.05f;
								}
							}
							vx += ((x - cx) / square_dist) * 0.001f * sign;
							vy += ((y - cy) / square_dist) * 0.001f * sign;
							part_array[i].speed = sqrtf(vx * vx + vy * vy);
							part_array[i].angle = atan2f(vy, vx);
						}
						glUnmapBuffer(GL_ARRAY_BUFFER);
					}
				break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = 0;
						break;

						case SDLK_q:
							running = 0;
							restart = 1;
						break;

						case SDLK_w:
							no_fading = !no_fading;
						break;

						case SDLK_t:
							render_each_iteration = !render_each_iteration;
						break;

						case SDLK_c:
							randomize_colors(type_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_p:
							randomize_parts(part_array,
								PARTICLE_NUMBER, tnu, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
							glBufferData(GL_ARRAY_BUFFER,
								PARTICLE_NUMBER * sizeof(part_t),
								part_array, GL_DYNAMIC_DRAW);
							glBindBuffer(GL_ARRAY_BUFFER, buf_part_next_id);
							glBufferData(GL_ARRAY_BUFFER,
								PARTICLE_NUMBER * sizeof(part_t),
								part_array, GL_DYNAMIC_DRAW);
						break;

						case SDLK_r:
							randomize_pils(pil_set_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_f:
							mutate_pils(pil_set_table, tn, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn*tn * sizeof(pil_set_t),
								pil_set_table, GL_STATIC_DRAW);
						break;

						case SDLK_y:
							randomize_change_laws(type_table, tn,
								CHANGE_TYPE_LAW_NUMBER, &rg);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_h:
							disable_change_laws(type_table, tn,
								CHANGE_TYPE_LAW_NUMBER);
							glBindBuffer(GL_ARRAY_BUFFER, buf_type_id);
							glBufferData(GL_ARRAY_BUFFER,
								tn * sizeof(part_type_t),
								type_table, GL_STATIC_DRAW);
						break;

						case SDLK_s:
							serialize_universe_rules("uwu.ipsysd",
								&info, pil_set_table, type_table);
						break;
					}
				break;
			}
		}

		t++;

		/* Test. */
		{
			float v = cosf((float)t * 0.1f);

			ui_line_t* line_block = uipt_get_prim_block(&ui_line_table, line_block_index);
			line_block[0].a.x = 200.5f - 100.0f * v;
			line_block[0].b.x = 600.5f + 100.0f * v;
			line_block[1].a.x = 600.5f + 100.0f * v;
			line_block[1].b.x = 600.5f + 100.0f * v;
			line_block[2].a.x = 600.5f + 100.0f * v;
			line_block[2].b.x = 200.5f - 100.0f * v;
			line_block[3].a.x = 200.5f - 100.0f * v;
			line_block[3].b.x = 200.5f - 100.0f * v;
			uipt_needs_sync(&ui_line_table);

			ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_triangle_table, triangle_block_index);
			triangle_block[0].a.x = 200.5f - 100.0f * v;
			triangle_block[0].b.x = 200.5f - 100.0f * v;
			triangle_block[0].c.x = 600.5f + 100.0f * v;
			triangle_block[1].a.x = 200.5f - 100.0f * v;
			triangle_block[1].b.x = 600.5f + 100.0f * v;
			triangle_block[1].c.x = 600.5f + 100.0f * v;
			uipt_needs_sync(&ui_triangle_table);
		}

		/* Render the UI. */
		{
			#define ATTRIB_LOCATION_POS 0
			#define ATTRIB_LOCATION_COLOR 1

			glViewport(800, 0, 800, 800);
			glUseProgram(g_shprog_draw_ui_simple);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			
			/* Clear UI. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_bg_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			/* Render rect bar filling. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_rect_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_rect_index_id);
			glDrawElements(GL_TRIANGLES, 6 * BAR_NUMBER, GL_UNSIGNED_INT,
				(void*)0);

			/* Render rect lines. */
			glBindBuffer(GL_ARRAY_BUFFER, buf_ui_line_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(ui_vertex_t),
				(void*)offsetof(ui_vertex_t, r));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_ui_line_index_id);
			glDrawElements(GL_LINES, 8 * BAR_NUMBER, GL_UNSIGNED_INT,
				(void*)0);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glUseProgram(0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR
		}

		/* Test. */
		{
			uipt_draw(&ui_triangle_table);
			uipt_draw(&ui_line_table);
		}

		/* Text test. */
		if (gchar_number > 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, font_texture_id);
			glProgramUniform1i(g_shprog_draw_gchars, 1, 0);
	
			#define ATTRIB_LOCATION_POS_XYWH 0
			#define ATTRIB_LOCATION_FONT_XYWH 1

			glViewport(800, 0, 800, 800);
			glUseProgram(g_shprog_draw_gchars);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
			glEnableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);

			glProgramUniform1f(g_shprog_draw_gchars, 0, 1.0f); /* Aspect ratio. */
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_gchars_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS_XYWH, 4, GL_FLOAT,
				GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_FONT_XYWH, 4, GL_FLOAT,
				GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, font_x));

			glDrawArrays(GL_POINTS, 0, gchar_number);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
			glDisableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);
			glUseProgram(0);

			#undef ATTRIB_LOCATION_POS_XYWH
			#undef ATTRIB_LOCATION_FONT_XYWH
		}

		/* Fade-to-black effect in the universe. */
		if (no_fading)
		{
			glBindFramebuffer(GL_FRAMEBUFFER,
				univ_fbo_double_id[univ_rendering_index]);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER,
				univ_fbo_double_id[univ_rendering_index]);
			glViewport(0, 0, 800, 800);

			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, univ_texture_double_id[1-univ_rendering_index]);

			#define ATTRIB_LOCATION_POS 0

			glUseProgram(g_shprog_draw_texture_fade_2);
			glProgramUniform1f(g_shprog_draw_texture_fade_2, 0, g_setting_read_fade_factor);
			glProgramUniform1i(g_shprog_draw_texture_fade_2, 1, 0);

			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_univ_full_rect_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(just_vertex_t),
				(void*)offsetof(just_vertex_t, x));
			
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glUseProgram(0);

			#undef ATTRIB_LOCATION_POS
		}

		/* Universe iterations. */
		for (unsigned int i = 0; i < iteration_number_per_frame; i++)
		{
			if (render_each_iteration)
			{
				glProgramUniform1i(g_shprog_comp_iteruniv, 0, 1);
			}
			else
			{
				glProgramUniform1i(g_shprog_comp_iteruniv, 0, i == 0);
			}
			glUseProgram(g_shprog_comp_iteruniv);
			
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_part_curr_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_part_next_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_type_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buf_pil_set_id);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buf_info_id);

			glDispatchCompute(PARTICLE_NUMBER / WORK_GROUP_SIZE, 1, 1);
			glUseProgram((GLuint)0);
			glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

			SWAP(GLuint, buf_part_curr_id, buf_part_next_id);

			/* Render the particles in the universe. */
			if (render_each_iteration || i == iteration_number_per_frame-1)
			{
				glBindFramebuffer(GL_FRAMEBUFFER,
					univ_fbo_double_id[univ_rendering_index]);
				glViewport(0, 0, 800, 800);

				#define ATTRIB_LOCATION_POS 0
				#define ATTRIB_LOCATION_COLOR 1
				#define ATTRIB_LOCATION_ANGLE 2
				#define ATTRIB_LOCATION_OLDPOS 3

				glUseProgram(g_shprog_draw_particles);
				glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
				glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
				glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
				glEnableVertexAttribArray(ATTRIB_LOCATION_OLDPOS);
				
				glBindBuffer(GL_ARRAY_BUFFER, buf_part_curr_id);
				glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
					GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, x));
				glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
					GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, r));
				glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
					GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, angle));
				glVertexAttribPointer(ATTRIB_LOCATION_OLDPOS, 2, GL_FLOAT,
					GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, oldx));

				glDrawArrays(GL_POINTS, 0, PARTICLE_NUMBER);
				
				glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
				glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
				glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
				glDisableVertexAttribArray(ATTRIB_LOCATION_OLDPOS);
				glUseProgram(0);

				#undef ATTRIB_LOCATION_POS
				#undef ATTRIB_LOCATION_COLOR
				#undef ATTRIB_LOCATION_ANGLE
				#undef ATTRIB_LOCATION_OLDPOS

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}

		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, univ_fbo_double_id[univ_rendering_index]);
			glBlitFramebuffer(0, 0, 800, 800, 0, 0, 800, 800, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		}

		univ_rendering_index = 1 - univ_rendering_index;

		SDL_GL_SwapWindow(g_window);
	}

	SDL_GL_DeleteContext(g_opengl_context);
	g_opengl_context = NULL;
	SDL_DestroyWindow(g_window);
	g_window = NULL;
	SDL_Quit();

	if (restart)
	{
		printf("Restarting (beware memory leaks)\n");
		goto l_beginning;
	}
	return 0;
}
