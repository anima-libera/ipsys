
#include "ui.h"
#include "utils.h"
#include "shaders.h"
#include <assert.h>
#include <math.h>

void font_init(font_t* font)
{
	font->implicit_space_length = 1.0f;
	font->space_length = 5.0f;

	font->texture_side = 256;
	font->texture_data = calloc(font->texture_side * font->texture_side, 1);

	unsigned int char_x = 0, char_y = 0;
	#define PAINT(x_, y_, v_) \
		font->texture_data[(char_x + (x_)) + (char_y + (y_)) * font->texture_side] = (v_)

	unsigned int line_i;
	#define LINE(...) \
		do \
		{ \
			unsigned char const data[] = {__VA_ARGS__}; \
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
			font->char_arr[(c_) - ' '] = (font_char_t){ \
				.rect = { \
					.x = (float)char_x / (float)font->texture_side, \
					.y = (float)char_y / (float)font->texture_side, \
					.h = 8.0f / (float)font->texture_side, \
					.w = (float)(w_) / (float)font->texture_side \
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

	glGenTextures(1, &font->texture_id);
	glBindTexture(GL_TEXTURE_2D, font->texture_id);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RED, font->texture_side, font->texture_side,
		0, GL_RED, GL_UNSIGNED_BYTE, font->texture_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void gchar_set(gchar_t* gchar, font_t const* font, char c, float x, float y)
{
	assert(c > ' ');
	font_char_t const* fc = &font->char_arr[c - ' '];
	assert(fc->w > 0 || fc->h > 0);
	gchar->rect_ui.x = x;
	gchar->rect_ui.y = y;
	gchar->rect_ui.w = (float)fc->w;
	gchar->rect_ui.h = (float)fc->h;
	gchar->rect_font = fc->rect;
}

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
	unsigned int const inf = uipt->block_da_arr[block_index].index;
	unsigned int const sup = inf + uipt->block_da_arr[block_index].len - 1;

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
	font_t const* font = data;

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

void init_gstring(gstring_t* gstring, gchar_t* gchar_block,
	char const* string, font_t const* font, float x, float y)
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
			font_char_t const* font_char = &font->char_arr[string[string_i] - ' '];
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

gstring_t alloc_gstring(uipt_t* ui_gchar_table,
	char const* string, font_t const* font, float x, float y)
{
	unsigned int gchar_count = 0;
	for (unsigned int string_i = 0; string[string_i] != '\0'; string_i++)
	{
		if (string[string_i] != ' ')
		{
			gchar_count++;
		}
	}

	unsigned int const gchar_block_index = uipt_alloc_prim_block(ui_gchar_table, gchar_count);
	gchar_t* gchar_block = uipt_get_prim_block(ui_gchar_table, gchar_block_index);
	gstring_t gstring;
	gstring.gchar_block_index = gchar_block_index;
	init_gstring(&gstring, gchar_block, string, font, x, y);
	return gstring;
}

void gstring_predict_dimensions(char const* string, font_t const* font, float* out_w, float* out_h)
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
			font_char_t const* font_char = &font->char_arr[string[string_i] - ' '];
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

void gstring_get_dimensions(uipt_t* ui_gchar_table, unsigned int gchar_block_index,
	float* out_w, float* out_h)
{
	gchar_t* gchar_block = uipt_get_prim_block(ui_gchar_table, gchar_block_index);
	unsigned int const len = uipt_get_block_len(ui_gchar_table, gchar_block_index);

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

static void set_lines_as_rect(ui_line_t* line_block, rect_t rect)
{
	float const xx = rect.x + 0.5f;
	float const yy = rect.y + 0.5f;
	float const w = rect.w;
	float const h = rect.h;

	line_block[0].a.x = xx;
	line_block[0].a.y = yy;
	line_block[0].b.x = xx + w;
	line_block[0].b.y = yy;
	line_block[1].a.x = xx + w;
	line_block[1].a.y = yy;
	line_block[1].b.x = xx + w;
	line_block[1].b.y = yy + h;
	line_block[2].a.x = xx + w;
	line_block[2].a.y = yy + h;
	line_block[2].b.x = xx;
	line_block[2].b.y = yy + h;
	line_block[3].a.x = xx;
	line_block[3].a.y = yy + h;
	line_block[3].b.x = xx;
	line_block[3].b.y = yy;
}

static void set_lines_color(ui_line_t* line_block, unsigned int number, float r, float g, float b)
{
	for (unsigned int i = 0; i < number; i++)
	{
		line_block[i].a.r = r;
		line_block[i].a.g = g;
		line_block[i].a.b = b;
		line_block[i].b.r = r;
		line_block[i].b.g = g;
		line_block[i].b.b = b;
	}
}

static void set_triangles_as_rect(ui_triangle_t* triangle_block, rect_t rect)
{
	float const xx = rect.x + 0.5f;
	float const yy = rect.y + 0.5f;
	float const w = rect.w;
	float const h = rect.h;

	triangle_block[0].a.x = xx;
	triangle_block[0].a.y = yy;
	triangle_block[0].b.x = xx;
	triangle_block[0].b.y = yy + h;
	triangle_block[0].c.x = xx + w; 
	triangle_block[0].c.y = yy + h;
	triangle_block[1].a.x = xx;
	triangle_block[1].a.y = yy;
	triangle_block[1].b.x = xx + w; 
	triangle_block[1].b.y = yy + h;
	triangle_block[1].c.x = xx + w; 
	triangle_block[1].c.y = yy;
}

static void set_triangles_color(ui_triangle_t* triangle_block, unsigned int number,
	float r, float g, float b)
{
	for (unsigned int i = 0; i < number; i++)
	{
		triangle_block[i].a.r = r;
		triangle_block[i].a.g = g;
		triangle_block[i].a.b = b;
		triangle_block[i].b.r = r;
		triangle_block[i].b.g = g;
		triangle_block[i].b.b = b;
		triangle_block[i].c.r = r;
		triangle_block[i].c.g = g;
		triangle_block[i].c.b = b;
	}
}

void widget_init_button(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h, char* text, void (*clic_callback)(widget_t* widget))
{
	widget->w = w;
	widget->h = h;

	widget->type = WIDGET_BUTTON;

	widget->button.clic_callback = clic_callback;

	rect_t const rect = {.x = x, .y = y, .w = w, .h = h};

	widget->button.line_block_index = uipt_alloc_prim_block(&ui_fabric->ui_line_table, 4);
	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->button.line_block_index);
	set_lines_color(line_block, 4, 1.0f, 1.0f, 1.0f);
	set_lines_as_rect(line_block, rect);

	widget->button.triangle_block_index = uipt_alloc_prim_block(&ui_fabric->ui_triangle_table, 2);
	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->button.triangle_block_index);
	set_triangles_color(triangle_block, 2, 0.0f, 0.3f, 0.2f);
	set_triangles_as_rect(triangle_block, rect);

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

	rect_t const rect = {.x = x, .y = y, .w = w, .h = h};

	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->button.line_block_index);
	set_lines_as_rect(line_block, rect);
	uipt_needs_sync(&ui_fabric->ui_line_table, widget->button.line_block_index);

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->button.triangle_block_index);
	set_triangles_as_rect(triangle_block, rect);
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

	float const v = widget->slider.value;
	rect_t const rect = {.x = x, .y = y, .w = w, .h = h};
	rect_t const rect_filling = {.x = x, .y = y, .w = floorf(w * v) + 0.5f, .h = h};

	widget->slider.line_block_index = uipt_alloc_prim_block(&ui_fabric->ui_line_table, 4);
	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->slider.line_block_index);
	set_lines_color(line_block, 4, 1.0f, 1.0f, 1.0f);
	set_lines_as_rect(line_block, rect);

	widget->slider.triangle_block_index = uipt_alloc_prim_block(&ui_fabric->ui_triangle_table, 4);
	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	set_triangles_color(&triangle_block[0], 2, 0.0f, 0.3f, 0.2f);
	set_triangles_as_rect(&triangle_block[0], rect);
	set_triangles_color(&triangle_block[2], 2, 0.1f, 0.8f, 0.0f);
	set_triangles_as_rect(&triangle_block[2], rect_filling);

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

	float const v = widget->slider.value;
	rect_t const rect = {.x = x, .y = y, .w = w, .h = h};
	rect_t const rect_filling = {.x = x, .y = y, .w = floorf(w * v) + 0.5f, .h = h};

	ui_line_t* line_block = uipt_get_prim_block(&ui_fabric->ui_line_table,
		widget->slider.line_block_index);
	set_lines_as_rect(line_block, rect);
	uipt_needs_sync(&ui_fabric->ui_line_table, widget->slider.line_block_index);

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	set_triangles_as_rect(&triangle_block[0], rect);
	set_triangles_as_rect(&triangle_block[2], rect_filling);
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
	float const w = widget->w;
	float const h = widget->h;
	float const v = widget->slider.value;
	rect_t const rect_filling = {.x = x, .y = y, .w = floorf(w * v) + 0.5f, .h = h};

	ui_triangle_t* triangle_block = uipt_get_prim_block(&ui_fabric->ui_triangle_table,
		widget->slider.triangle_block_index);
	set_triangles_as_rect(&triangle_block[2], rect_filling);
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
