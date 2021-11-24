
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

struct rect_t
{
	float x, y, w, h;
};
typedef struct rect_t rect_t;

struct font_char_t
{
	rect_t rect;
	unsigned int w, h;
};
typedef struct font_char_t font_char_t;

struct font_t
{
	unsigned int texture_side;
	unsigned char* texture_data;
	GLuint texture_id;
	font_char_t char_arr['~' - ' '];
	float space_length;
	float implicit_space_length;
};
typedef struct font_t font_t;

/* Graphical character, one that is displayed on screen. */
struct gchar_t
{
	rect_t rect_ui;
	rect_t rect_font;
};
typedef struct gchar_t gchar_t;

void gchar_set(gchar_t* gchar, const font_t* font, char c, float x, float y)
{
	assert(c > ' ');
	const font_char_t* fc = &font->char_arr[c - ' '];
	assert(fc->w > 0 || fc->h > 0);
	gchar->rect_ui.x = x;
	gchar->rect_ui.y = y;
	gchar->rect_ui.w = (float)fc->w;
	gchar->rect_ui.h = (float)fc->h;
	gchar->rect_font = fc->rect;
}

struct uip_block_t
{
	unsigned int len;
	unsigned int index;
};
typedef struct uip_block_t uip_block_t;

typedef void (*drawcall_callback_t)(GLuint opengl_buffer_id, unsigned int prim_count, void* data);

struct uipt_t
{
	unsigned int block_da_len;
	unsigned int block_da_cap;
	uip_block_t* block_da_arr;
	unsigned int prim_da_len;
	unsigned int prim_da_cap;
	void* prim_da_arr;
	GLuint prim_opengl_buffer_id;
	unsigned int needs_prim_buffer_sync_inf;
	unsigned int needs_prim_buffer_sync_sup;
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
	uipt->needs_prim_buffer_sync_inf = INT32_MAX;
	uipt->needs_prim_buffer_sync_sup = 0;
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

unsigned int uipt_get_block_len(uipt_t* uipt, unsigned int block_index)
{
	return uipt->block_da_arr[block_index].len;
}

void uipt_draw(uipt_t* uipt, void* callback_data)
{
	if (uipt->needs_prim_buffer_sync_alloc)
	{
		glBindBuffer(GL_ARRAY_BUFFER, uipt->prim_opengl_buffer_id);
		glBufferData(GL_ARRAY_BUFFER,
			uipt->prim_da_len * uipt->sizeof_prim, uipt->prim_da_arr,
			GL_DYNAMIC_DRAW);
	}
	else if (uipt->needs_prim_buffer_sync_sup >= uipt->needs_prim_buffer_sync_inf)
	{
		//fprintf(stderr, "buffer_sync %d %d\n", uipt->needs_prim_buffer_sync_inf, uipt->needs_prim_buffer_sync_sup);
		glBindBuffer(GL_ARRAY_BUFFER, uipt->prim_opengl_buffer_id);
		glBufferSubData(GL_ARRAY_BUFFER,
			#if 0
				uipt->needs_prim_buffer_sync_inf * uipt->sizeof_prim,
				(uipt->needs_prim_buffer_sync_sup + 1 - uipt->needs_prim_buffer_sync_inf) * uipt->sizeof_prim,
			#else
				0,
				uipt->prim_da_len * uipt->sizeof_prim,
			#endif
			uipt->prim_da_arr);
		/* TODO: Fix alignment issues (on prim_da_arr) that make glBufferSubData behave badly
		 * when used with inf as an offset. */
	}
	uipt->needs_prim_buffer_sync_alloc = 0;
	uipt->needs_prim_buffer_sync_inf = INT32_MAX;
	uipt->needs_prim_buffer_sync_sup = 0;

	uipt->drawcall_callback(uipt->prim_opengl_buffer_id, uipt->prim_da_len, callback_data);
}

void uipt_needs_sync(uipt_t* uipt, unsigned int block_index)
{
	unsigned int inf = uipt->block_da_arr[block_index].index;
	unsigned int sup = inf + uipt->block_da_arr[block_index].len - 1;

	if (uipt->needs_prim_buffer_sync_sup < uipt->needs_prim_buffer_sync_inf)
	{
		uipt->needs_prim_buffer_sync_inf = inf;
		uipt->needs_prim_buffer_sync_sup = sup;
	}
	else
	{
		if (uipt->needs_prim_buffer_sync_inf > inf)
		{
			uipt->needs_prim_buffer_sync_inf = inf;
		}
		if (uipt->needs_prim_buffer_sync_sup < sup)
		{
			uipt->needs_prim_buffer_sync_sup = sup;
		}
	}
}

void line_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* unused)
{
	(void)unused;

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

void triangle_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* unused)
{
	(void)unused;

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

void gchar_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* data)
{
	font_t* font = data;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->texture_id);
	glProgramUniform1i(g_shprog_draw_gchars, 1, 0);

	#define ATTRIB_LOCATION_POS_XYWH 0
	#define ATTRIB_LOCATION_FONT_XYWH 1

	glViewport(800, 0, 800, 800);
	glUseProgram(g_shprog_draw_gchars);
	glEnableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
	glEnableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);

	glProgramUniform1f(g_shprog_draw_gchars, 0, 1.0f); /* Aspect ratio. */

	glBindBuffer(GL_ARRAY_BUFFER, opengl_buffer_id);
	glVertexAttribPointer(ATTRIB_LOCATION_POS_XYWH, 4, GL_FLOAT,
		GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, rect_ui));
	glVertexAttribPointer(ATTRIB_LOCATION_FONT_XYWH, 4, GL_FLOAT,
		GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, rect_font));

	glDrawArrays(GL_POINTS, 0, prim_count);

	glDisableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
	glDisableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);
	glUseProgram(0);

	#undef ATTRIB_LOCATION_POS_XYWH
	#undef ATTRIB_LOCATION_FONT_XYWH
}

struct gstring_t
{
	const char* string;
	unsigned int gchar_block_index;
	float w, h;
};
typedef struct gstring_t gstring_t;

void init_gstring(gstring_t* gstring, gchar_t* gchar_block, const char* string, const font_t* font, float x, float y)
{
	float x_offset = 0.0f;
	float h_max = 0.0f;
	int prev_is_char = 0;
	unsigned int block_i = 0;
	for (unsigned int string_i = 0; string[string_i] != '\0'; string_i++)
	{
		if (string[string_i] == ' ')
		{
			x_offset += font->space_length;
			prev_is_char = 0;
		}
		else
		{
			if (prev_is_char)
			{
				x_offset += font->implicit_space_length;
			}
			gchar_set(&gchar_block[block_i++], font, string[string_i], x + x_offset, y);
			const font_char_t* font_char = &font->char_arr[string[string_i] - ' '];
			x_offset += (float)font_char->w;
			if (h_max < (float)font_char->h)
			{
				h_max = (float)font_char->h;
			}
			prev_is_char = 1;
		}
	}
	gstring->w = x_offset;
	gstring->h = h_max;
	gstring->string = string;
}

gstring_t alloc_gstring(uipt_t* ui_gchar_table, const char* string, const font_t* font, float x, float y)
{
	unsigned int gchar_count = 0;
	for (unsigned int string_i = 0; string[string_i] != '\0'; string_i++)
	{
		if (string[string_i] != ' ')
		{
			gchar_count++;
		}
	}

	unsigned int gchar_block_index = uipt_alloc_prim_block(ui_gchar_table, gchar_count);
	gchar_t* gchar_block = uipt_get_prim_block(ui_gchar_table, gchar_block_index);
	gstring_t gstring;
	gstring.gchar_block_index = gchar_block_index;
	init_gstring(&gstring, gchar_block, string, font, x, y);
	return gstring;
}

void gstring_predict_dimensions(const char* string, const font_t* font, float* out_w, float* out_h)
{
	float x_offset = 0.0f;
	float h_max = 0.0f;
	int prev_is_char = 0;
	for (unsigned int string_i = 0; string[string_i] != '\0'; string_i++)
	{
		if (string[string_i] == ' ')
		{
			x_offset += font->space_length;
			prev_is_char = 0;
		}
		else
		{
			if (prev_is_char)
			{
				x_offset += font->implicit_space_length;
			}
			const font_char_t* font_char = &font->char_arr[string[string_i] - ' '];
			x_offset += (float)font_char->w;
			if (h_max < (float)font_char->h)
			{
				h_max = (float)font_char->h;
			}
			prev_is_char = 1;
		}
	}
	
	*out_w = x_offset;
	*out_h = h_max;
}

void gstring_get_dimensions(uipt_t* ui_gchar_table, unsigned int gchar_block_index, float* out_w, float* out_h)
{
	gchar_t* gchar_block = uipt_get_prim_block(ui_gchar_table, gchar_block_index);
	unsigned int len = uipt_get_block_len(ui_gchar_table, gchar_block_index);

	assert(len >= 1);

	float inf_x = gchar_block[0].rect_ui.x;
	float sup_x = gchar_block[0].rect_ui.x + gchar_block[0].rect_ui.w;
	float inf_y = gchar_block[0].rect_ui.y;
	float sup_y = gchar_block[0].rect_ui.y + gchar_block[0].rect_ui.h;

	for (unsigned int i = 1; i < len; i++)
	{
		if (inf_x > gchar_block[i].rect_ui.x)
		{
			inf_x = gchar_block[i].rect_ui.x;
		}
		if (sup_x < gchar_block[i].rect_ui.x + gchar_block[i].rect_ui.w)
		{
			sup_x = gchar_block[i].rect_ui.x + gchar_block[i].rect_ui.w;
		}
		if (inf_y > gchar_block[i].rect_ui.y)
		{
			inf_y = gchar_block[i].rect_ui.y;
		}
		if (sup_y < gchar_block[i].rect_ui.y + gchar_block[i].rect_ui.h)
		{
			sup_y = gchar_block[i].rect_ui.y + gchar_block[i].rect_ui.h;
		}
	}
	
	*out_w = sup_x - inf_x;
	*out_h = sup_y - inf_y;
}

struct ui_fabric_t
{
	uipt_t ui_line_table;
	uipt_t ui_triangle_table;
	font_t font;
	uipt_t ui_gchar_table;
};
typedef struct ui_fabric_t ui_fabric_t;

enum widget_type_t
{
	WIDGET_BUTTON,
	WIDGET_SLIDER,
};
typedef enum widget_type_t widget_type_t;

typedef struct widget_t widget_t;

struct widget_slider_t
{
	unsigned int line_block_index;
	unsigned int triangle_block_index;
	gstring_t gstring;
	void (*clic_callback)(widget_t* widget);
	float value;
};
typedef struct widget_slider_t widget_slider_t;

struct widget_button_t
{
	unsigned int line_block_index;
	unsigned int triangle_block_index;
	gstring_t gstring;
	void (*clic_callback)(widget_t* widget);
};
typedef struct widget_button_t widget_button_t;

struct widget_t
{
	float w, h;
	widget_type_t type;
	union
	{
		widget_button_t button;
		widget_slider_t slider;
	};
};

void widget_init_button(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h, char* text, void (*clic_callback)(widget_t* widget))
{
	widget->w = w;
	widget->h = h;

	widget->type = WIDGET_BUTTON;

	widget->button.clic_callback = clic_callback;

	const float xx = x + 0.5f;
	const float yy = y + 0.5f;

	widget->button.line_block_index = uipt_alloc_prim_block(&ui_fabric->ui_line_table, 4);
	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->button.line_block_index);
	line_block[0].a = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[0].b = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].a = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].a = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].a = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].b = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};

	widget->button.triangle_block_index = uipt_alloc_prim_block(&ui_fabric->ui_triangle_table, 2);
	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->button.triangle_block_index);
	triangle_block[0].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].c = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].c = (ui_vertex_t){.x = xx + w, .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};

	float gstring_w, gstring_h;
	gstring_predict_dimensions(text, &ui_fabric->font, &gstring_w, &gstring_h);

	widget->button.gstring = alloc_gstring(&ui_fabric->ui_gchar_table,
		text, &ui_fabric->font,
		floorf(x + (w - gstring_w) / 2.0f), floorf(y + (h - gstring_h) / 2.0f));
}

void widget_reposition_button(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h)
{
	widget->w = w;
	widget->h = h;

	const float xx = x + 0.5f;
	const float yy = y + 0.5f;

	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->button.line_block_index);
	line_block[0].a = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[0].b = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].a = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].a = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].a = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].b = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	uipt_needs_sync(&ui_fabric->ui_line_table, widget->button.line_block_index);

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->button.triangle_block_index);
	triangle_block[0].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].c = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].c = (ui_vertex_t){.x = xx + w, .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	uipt_needs_sync(&ui_fabric->ui_triangle_table, widget->button.triangle_block_index);

	gchar_t* gchar_block = uipt_get_prim_block(&ui_fabric->ui_gchar_table,
		widget->button.gstring.gchar_block_index);
	init_gstring(&widget->button.gstring, gchar_block,
		widget->button.gstring.string, &ui_fabric->font,
		floorf(x + (w - widget->button.gstring.w) / 2.0f),
		floorf(y + (h - widget->button.gstring.h) / 2.0f));
}

void widget_init_slider(ui_fabric_t* ui_fabric, widget_t* widget, float value,
	float x, float y, float w, float h, char* text, void (*clic_callback)(widget_t* widget))
{
	widget->w = w;
	widget->h = h;

	widget->type = WIDGET_SLIDER;

	widget->slider.value = value;

	widget->slider.clic_callback = clic_callback;

	const float xx = x + 0.5f;
	const float yy = y + 0.5f;
	const float v = widget->slider.value;

	widget->slider.line_block_index = uipt_alloc_prim_block(&ui_fabric->ui_line_table, 4);
	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->slider.line_block_index);
	line_block[0].a = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[0].b = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].a = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].a = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].a = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].b = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};

	widget->slider.triangle_block_index = uipt_alloc_prim_block(&ui_fabric->ui_triangle_table, 4);
	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	triangle_block[0].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].c = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].c = (ui_vertex_t){.x = xx + w, .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[2].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].c = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].b = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].c = (ui_vertex_t){.x = xx+v*w, .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};

	float gstring_w, gstring_h;
	gstring_predict_dimensions(text, &ui_fabric->font, &gstring_w, &gstring_h);

	widget->slider.gstring = alloc_gstring(&ui_fabric->ui_gchar_table,
		text, &ui_fabric->font,
		floorf(x + (w - gstring_w) / 2.0f), floorf(y + (h - gstring_h) / 2.0f));
}

void widget_reposition_slider(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h)
{
	widget->w = w;
	widget->h = h;

	const float xx = x + 0.5f;
	const float yy = y + 0.5f;
	const float v = widget->slider.value;

	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->slider.line_block_index);
	line_block[0].a = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[0].b = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].a = (ui_vertex_t){.x = xx + w, .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].a = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].a = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 1.0f, .g = 1.0f, .b = 1.0f};
	line_block[3].b = (ui_vertex_t){.x = xx,     .y = yy,     .r = 1.0f, .g = 1.0f, .b = 1.0f};
	uipt_needs_sync(&ui_fabric->ui_line_table, widget->slider.line_block_index);

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	triangle_block[0].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[0].c = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].b = (ui_vertex_t){.x = xx + w, .y = yy + h, .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[1].c = (ui_vertex_t){.x = xx + w, .y = yy    , .r = 0.0f, .g = 0.3f, .b = 0.2f};
	triangle_block[2].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].c = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].b = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].c = (ui_vertex_t){.x = xx+v*w, .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	uipt_needs_sync(&ui_fabric->ui_triangle_table, widget->slider.triangle_block_index);

	gchar_t* gchar_block = uipt_get_prim_block(&ui_fabric->ui_gchar_table,
		widget->slider.gstring.gchar_block_index);
	init_gstring(&widget->slider.gstring, gchar_block,
		widget->slider.gstring.string, &ui_fabric->font,
		floorf(x + (w - widget->slider.gstring.w) / 2.0f),
		floorf(y + (h - widget->slider.gstring.h) / 2.0f));
}

void widget_update_slider(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y)
{
	const float xx = x + 0.5f;
	const float yy = y + 0.5f;
	const float w = widget->w;
	const float h = widget->h;
	const float v = widget->slider.value;

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	triangle_block[2].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].b = (ui_vertex_t){.x = xx,     .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[2].c = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].a = (ui_vertex_t){.x = xx,     .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].b = (ui_vertex_t){.x = xx+v*w, .y = yy + h, .r = 0.1f, .g = 0.8f, .b = 0.0f};
	triangle_block[3].c = (ui_vertex_t){.x = xx+v*w, .y = yy    , .r = 0.1f, .g = 0.8f, .b = 0.0f};
	uipt_needs_sync(&ui_fabric->ui_triangle_table, widget->slider.triangle_block_index);
	//ui_fabric->ui_triangle_table.needs_prim_buffer_sync_alloc = 1;
	/* TODO: DO NOT use needs_prim_buffer_sync_alloc, it whould't be needed
	 * (but for some reason it doesn't work with BufferSubData for now). */
}

int widget_has_coords(widget_t* widget, float widget_x, float widget_y,
	float coords_x, float coords_y)
{
	switch (widget->type)
	{
		case WIDGET_BUTTON:
		case WIDGET_SLIDER:
			return widget_x <= coords_x && coords_x <= widget_x + widget->w &&
				widget_y <= coords_y && coords_y <= widget_y + widget->h;
		break;
	}
	return 0;
}

void widget_reposition(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h)
{
	switch (widget->type)
	{
		case WIDGET_BUTTON:
			widget_reposition_button(ui_fabric, widget, x, y, w, h);
		break;
		case WIDGET_SLIDER:
			widget_reposition_slider(ui_fabric, widget, x, y, w, h);
		break;
		default:
			assert(0);
		break;
	}
}

int g_callback_button_test_flag = 0;
void callback_test(widget_t* widget)
{
	g_callback_button_test_flag = 1;
}

#define MAX_ITER_PER_FRAME 32

int g_callback_slider_1_flag = 0;
void callback_slider_1(widget_t* widget)
{
	g_callback_slider_1_flag = 1;
}

int g_callback_slider_2_flag = 0;
void callback_slider_2(widget_t* widget)
{
	g_callback_slider_2_flag = 1;
	widget->slider.value = roundf(widget->slider.value * (float)(MAX_ITER_PER_FRAME - 1)) /
		(float)(MAX_ITER_PER_FRAME - 1);
}

struct pos_widget_t
{
	float x, y;
	widget_t* widget;
};
typedef struct pos_widget_t pos_widget_t;

struct widget_manager_t
{
	unsigned int len;
	unsigned int cap;
	pos_widget_t* arr;
	float next_x, next_y;
};
typedef struct widget_manager_t widget_manager_t;

void widget_manager_give(widget_manager_t* wm, ui_fabric_t* ui_fabric, widget_t* widget)
{
	DA_LENGTHEN(wm->len += 1, wm->cap, wm->arr, pos_widget_t);
	pos_widget_t* posw = &wm->arr[wm->len-1];
	posw->widget = widget;
	posw->x = wm->next_x;
	posw->y = wm->next_y;
	widget_reposition(ui_fabric, widget, posw->x, posw->y, widget->w, widget->h);
	wm->next_y -= widget->h + 20.0f;
}

int widget_manager_clic(widget_manager_t* wm, ui_fabric_t* ui_fabric, float x, float y)
{
	for (unsigned int i = 0; i < wm->len; i++)
	{
		if (widget_has_coords(wm->arr[i].widget,
			wm->arr[i].x, wm->arr[i].y, x, y))
		{
			switch (wm->arr[i].widget->type)
			{
				case WIDGET_BUTTON:
					wm->arr[i].widget->button.clic_callback(wm->arr[i].widget);
				break;
				case WIDGET_SLIDER:
					wm->arr[i].widget->slider.value = (x - wm->arr[i].x) / wm->arr[i].widget->w;
					wm->arr[i].widget->slider.clic_callback(wm->arr[i].widget);
					widget_update_slider(ui_fabric, wm->arr[i].widget,
						wm->arr[i].x, wm->arr[i].y);
				break;
				default:
					assert(0);
				break;
			}
			return 1;
		}
	}
	return 0;
}

int main(int argc, const char** argv)
{
	l_beginning:;

	/* Parse command line arguments. */

	const char* arg_type_number = NULL;
	const char* arg_ipsysd_filepath = NULL;
	int no_universe = 0;

	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			//assert(0);
		}
		else if (strcmp(argv[i], "-n") == 0)
		{
			no_universe = 1;
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

	ui_fabric_t ui_fabric = {0};
	uipt_init(&ui_fabric.ui_line_table, sizeof(ui_line_t), line_drawcall_callback);
	uipt_init(&ui_fabric.ui_triangle_table, sizeof(ui_triangle_t), triangle_drawcall_callback);

	/* Font and text setup. */

	ui_fabric.font.implicit_space_length = 1.0f;
	ui_fabric.font.space_length = 5.0f;

	ui_fabric.font.texture_side = 256;
	ui_fabric.font.texture_data = calloc(ui_fabric.font.texture_side * ui_fabric.font.texture_side, 1);

	unsigned int char_x = 0, char_y = 0;
	#define PAINT(x_, y_, v_) \
		ui_fabric.font.texture_data[(char_x + (x_)) + (char_y + (y_)) * ui_fabric.font.texture_side] = (v_)

	unsigned int line_i;
	#define LINE(...) \
		do \
		{ \
			unsigned char data[] = {__VA_ARGS__}; \
			for (unsigned int i = 0; i < sizeof data; i++) \
			{ \
				PAINT(i, line_i, data[i]); \
			} \
			line_i--; \
		} \
		while (0)

	#define W 255
	#define _ 0

	#define CHAR_BEG() \
		line_i = 7
	#define CHAR_END(c_, w_) \
		do \
		{ \
			_Static_assert(' ' < (c_) && (c_) <= '~', "Invalid character"); \
			ui_fabric.font.char_arr[(c_) - ' '] = (font_char_t){ \
				.rect = { \
					.x = (float)char_x / (float)ui_fabric.font.texture_side, \
					.y = (float)char_y / (float)ui_fabric.font.texture_side, \
					.h = 8.0f / (float)ui_fabric.font.texture_side, \
					.w = (float)(w_) / (float)ui_fabric.font.texture_side \
				}, \
				.w = (w_), \
				.h = 8 \
			}; \
			char_x += (w_); \
		} \
		while (0)

	CHAR_BEG();
		LINE(_,_,W,_,_);
		LINE(_,W,_,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
	CHAR_END('A', 5);

	CHAR_BEG();
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,_);
	CHAR_END('B', 5);

	CHAR_BEG();
		LINE(_,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('C', 5);

	CHAR_BEG();
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,_);
	CHAR_END('D', 5);

	CHAR_BEG();
		LINE(W,W,W,W,W);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,W,W,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,W,W,W,W);
	CHAR_END('E', 5);

	CHAR_BEG();
		LINE(W,W,W,W,W);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,W,W,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
	CHAR_END('F', 5);

	CHAR_BEG();
		LINE(_,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,W,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('G', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
	CHAR_END('H', 5);

	CHAR_BEG();
		LINE(W);
		LINE(W);
		LINE(W);
		LINE(W);
		LINE(W);
		LINE(W);
		LINE(W);
		LINE(W);
	CHAR_END('I', 1);

	CHAR_BEG();
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('J', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,W,_);
		LINE(W,_,W,_,_);
		LINE(W,W,_,_,_);
		LINE(W,_,W,_,_);
		LINE(W,_,W,_,_);
		LINE(W,_,_,W,_);
		LINE(W,_,_,_,W);
	CHAR_END('K', 5);

	CHAR_BEG();
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,W,W,W,W);
	CHAR_END('L', 5);

	CHAR_BEG();
		LINE(W,_,_,_,_,_,W);
		LINE(W,W,_,_,_,W,W);
		LINE(W,_,W,_,W,_,W);
		LINE(W,_,_,W,_,_,W);
		LINE(W,_,_,_,_,_,W);
		LINE(W,_,_,_,_,_,W);
		LINE(W,_,_,_,_,_,W);
		LINE(W,_,_,_,_,_,W);
	CHAR_END('M', 7);

	CHAR_BEG();
		LINE(W,_,_,_,_,W);
		LINE(W,W,_,_,_,W);
		LINE(W,_,W,_,_,W);
		LINE(W,_,W,_,_,W);
		LINE(W,_,_,W,_,W);
		LINE(W,_,_,W,_,W);
		LINE(W,_,_,_,W,W);
		LINE(W,_,_,_,_,W);
	CHAR_END('N', 6);

	CHAR_BEG();
		LINE(_,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('O', 5);

	CHAR_BEG();
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
	CHAR_END('P', 5);

	CHAR_BEG();
		LINE(_,W,W,W,_,_);
		LINE(W,_,_,_,W,_);
		LINE(W,_,_,_,W,_);
		LINE(W,_,_,_,W,_);
		LINE(W,_,_,_,W,_);
		LINE(W,_,_,W,W,_);
		LINE(W,_,_,_,W,_);
		LINE(_,W,W,W,_,W);
	CHAR_END('Q', 6);

	CHAR_BEG();
		LINE(W,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,W,W,W,_);
		LINE(W,_,W,_,_);
		LINE(W,_,_,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
	CHAR_END('R', 5);

	CHAR_BEG();
		LINE(_,W,W,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,_);
		LINE(_,W,W,W,_);
		LINE(_,_,_,_,W);
		LINE(_,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('S', 5);

	CHAR_BEG();
		LINE(W,W,W,W,W);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
	CHAR_END('T', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,W,W,_);
	CHAR_END('U', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,_,W,_);
		LINE(_,W,_,W,_);
		LINE(_,_,W,_,_);
	CHAR_END('V', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(W,_,W,_,W);
		LINE(W,_,W,_,W);
		LINE(_,W,_,W,_);
		LINE(_,W,_,W,_);
	CHAR_END('W', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,_,W,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,W,_,W,_);
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
	CHAR_END('X', 5);

	CHAR_BEG();
		LINE(W,_,_,_,W);
		LINE(W,_,_,_,W);
		LINE(_,W,_,W,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
		LINE(_,_,W,_,_);
	CHAR_END('Y', 5);

	CHAR_BEG();
		LINE(W,W,W,W,W);
		LINE(_,_,_,_,W);
		LINE(_,_,_,W,_);
		LINE(_,_,W,_,_);
		LINE(_,W,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,_,_,_,_);
		LINE(W,W,W,W,W);
	CHAR_END('Z', 5);

	#undef CHAR_BEG
	#undef CHAR_END
	#undef W
	#undef _
	#undef LINE
	#undef PAINT

	glGenTextures(1, &ui_fabric.font.texture_id);
	glBindTexture(GL_TEXTURE_2D, ui_fabric.font.texture_id);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RED, ui_fabric.font.texture_side, ui_fabric.font.texture_side, 0, GL_RED, GL_UNSIGNED_BYTE, ui_fabric.font.texture_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	uipt_init(&ui_fabric.ui_gchar_table, sizeof(gchar_t), gchar_drawcall_callback);

	/* UI setup. */

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

	widget_manager_t wm = {.next_x = 20.0f, .next_y = 800.0f - 40.0f};

	widget_t widget_slider_test_arr[2];
	widget_init_slider(&ui_fabric, &widget_slider_test_arr[0],
		1.0f - g_setting_read_fade_factor / SETTING_FADE_FACTOR_MAX,
		0.0f, 0.0f, 760.0f, 20.0f, "TRACE", callback_slider_1);
	widget_manager_give(&wm, &ui_fabric, &widget_slider_test_arr[0]);
	widget_init_slider(&ui_fabric, &widget_slider_test_arr[1],
		((float)iteration_number_per_frame - 1.0f) / ((float)(MAX_ITER_PER_FRAME) - 1.0f),
		0.0f, 0.0f, 760.0f, 20.0f, "ITERATION NUMBER PER FRAME", callback_slider_2);
	widget_manager_give(&wm, &ui_fabric, &widget_slider_test_arr[1]);
	
	widget_t widget_button_test_arr[1];
	for (unsigned int i = 0; i < 1; i++)
	{
		widget_init_button(&ui_fabric, &widget_button_test_arr[i],
			0.0f, 0.0f, 200.0f, 20.0f, "RANDOMIZE LAWS", callback_test);
		widget_manager_give(&wm, &ui_fabric, &widget_button_test_arr[i]);
	}

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

						if (widget_manager_clic(&wm, &ui_fabric, x, y))
						{
							break;
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

		if (g_callback_slider_1_flag)
		{
			g_callback_slider_1_flag = 0;
			const float value = (1.0f - widget_slider_test_arr[0].slider.value) *
				SETTING_FADE_FACTOR_MAX;
			setting_set_fade_factor(value);
		}
		if (g_callback_slider_2_flag)
		{
			g_callback_slider_2_flag = 0;
			const unsigned int value =
				roundf(widget_slider_test_arr[1].slider.value *
					((float)MAX_ITER_PER_FRAME - 1.0f) + 1.0f);
			iteration_number_per_frame = value;
		}
		if (g_callback_button_test_flag)
		{
			g_callback_button_test_flag = 0;
			randomize_pils(pil_set_table, tn, &rg);
			glBindBuffer(GL_ARRAY_BUFFER, buf_pil_set_id);
			glBufferData(GL_ARRAY_BUFFER,
				tn*tn * sizeof(pil_set_t),
				pil_set_table, GL_STATIC_DRAW);
		}

		t++;

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
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glUseProgram(0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR

			/* Draw the UI fabric. */
			{
				uipt_draw(&ui_fabric.ui_triangle_table, NULL);
				uipt_draw(&ui_fabric.ui_line_table, NULL);
				uipt_draw(&ui_fabric.ui_gchar_table, &ui_fabric.font);
			}
		}

		if (!no_universe)
		{
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
		}

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
