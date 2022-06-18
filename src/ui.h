
#ifndef IPSYS_HEADER_UI__
#define IPSYS_HEADER_UI__

#include "opengl.h"

struct __attribute__((packed)) ui_vertex_t
{
	GLfloat x, y;
	GLfloat r, g, b;
};
typedef struct ui_vertex_t ui_vertex_t;

struct rect_t
{
	float x, y, w, h;
};
typedef struct rect_t rect_t;

/* FONT SECTION */

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

void font_init(font_t* font);

/* Graphical character, one that is displayed on screen. */
struct gchar_t
{
	rect_t rect_ui;
	rect_t rect_font;
};
typedef struct gchar_t gchar_t;

void gchar_set(gchar_t* gchar, font_t const* font, char c, float x, float y);

/* UIPT SECTION */

/* Designate a UI primitive block. */
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

void uipt_init(uipt_t* uipt, unsigned int sizeof_prim, drawcall_callback_t drawcall_callback);
unsigned int uipt_alloc_prim_block(uipt_t* uipt, unsigned int len);
void* uipt_get_prim_block(uipt_t* uipt, unsigned int block_index);
unsigned int uipt_get_block_len(uipt_t* uipt, unsigned int block_index);
void uipt_draw(uipt_t* uipt, void* callback_data);
void uipt_needs_sync(uipt_t* uipt, unsigned int block_index);

void line_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* unused);
void triangle_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* unused);
void gchar_drawcall_callback(GLuint opengl_buffer_id, unsigned int prim_count, void* data);

struct ui_fabric_t
{
	uipt_t ui_line_table;
	uipt_t ui_triangle_table;
	font_t font;
	uipt_t ui_gchar_table;
};
typedef struct ui_fabric_t ui_fabric_t;

/* GSTRING SECTION */

struct gstring_t
{
	const char* string;
	unsigned int gchar_block_index;
	float w, h;
};
typedef struct gstring_t gstring_t;

void init_gstring(gstring_t* gstring, gchar_t* gchar_block,
	char const* string, font_t const* font, float x, float y);
gstring_t alloc_gstring(uipt_t* ui_gchar_table,
	char const* string, font_t const* font, float x, float y);
void gstring_predict_dimensions(char const* string, font_t const* font,
	float* out_w, float* out_h);
void gstring_get_dimensions(uipt_t* ui_gchar_table, unsigned int gchar_block_index,
	float* out_w, float* out_h);

/* WIDGET SECTION */

enum widget_type_t
{
	WIDGET_BUTTON,
	WIDGET_SLIDER,
};
typedef enum widget_type_t widget_type_t;

typedef struct widget_t widget_t;

struct widget_button_t
{
	unsigned int line_block_index;
	unsigned int triangle_block_index;
	gstring_t gstring;
	void (*clic_callback)(widget_t* widget);
};
typedef struct widget_button_t widget_button_t;

void widget_init_button(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h, char* text, void (*clic_callback)(widget_t* widget));
void widget_reposition_button(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h);

struct widget_slider_t
{
	unsigned int line_block_index;
	unsigned int triangle_block_index;
	gstring_t gstring;
	void (*clic_callback)(widget_t* widget);
	float value;
};
typedef struct widget_slider_t widget_slider_t;

void widget_init_slider(ui_fabric_t* ui_fabric, widget_t* widget, float value,
	float x, float y, float w, float h, char* text, void (*clic_callback)(widget_t* widget));
void widget_reposition_slider(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h);
void widget_update_slider(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y);

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

int widget_has_coords(widget_t* widget, float widget_x, float widget_y,
	float coords_x, float coords_y);
void widget_reposition(ui_fabric_t* ui_fabric, widget_t* widget,
	float x, float y, float w, float h);

/* WIDGET MANAGER SECTION */

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

void widget_manager_give(widget_manager_t* wm, ui_fabric_t* ui_fabric, widget_t* widget);
int widget_manager_clic(widget_manager_t* wm, ui_fabric_t* ui_fabric, float x, float y);

#endif /* IPSYS_HEADER_UI__ */
