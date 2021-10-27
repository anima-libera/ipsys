
#include "serialization.h"
#include "universe.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* See https://stackoverflow.com/a/21162120
 * and the value of FLT_DECIMAL_DIG defined in <float.h>. */
#define FLOAT_FMT "%.9g"

static void emit_indent(FILE* dst, int indent)
{
	for (unsigned int i = 0; i < (unsigned int)indent; i++)
	{
		fprintf(dst, "\t");
	}
}

#if 0

static void emit_leaf_int(FILE* dst, int indent, const char* name, int value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .int %d }\n", name, value);
}

static void emit_leaf_float(FILE* dst, int indent, const char* name, float value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .float "FLOAT_FMT" }\n", name, value);
}

static void emit_leaf_string(FILE* dst, int indent, const char* name, const char* value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .string \"%s\" }\n", name, value);
}

static void emit_leaf_rgb(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgb "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" }\n", name, r, g, b);
}

static void emit_leaf_rgbvec(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgbvec "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" }\n", name, r, g, b);
}

static void emit_leaf_pil(FILE* dst, int indent, const char* name, const pil_t* pil)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .pil ", name);
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fprintf(dst, ""FLOAT_FMT" "FLOAT_FMT" ", pil->steps[i].offset, pil->steps[i].slope);
	}
	fprintf(dst, "}\n");
}

static void emit_pil_set(FILE* dst, int indent, const char* name, const pil_set_t* pil_set)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_pil(dst, indent+1, "attraction", &pil_set->attraction);
	emit_leaf_pil(dst, indent+1, "angle", &pil_set->angle);
	emit_leaf_pil(dst, indent+1, "speed", &pil_set->speed);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

static void emit_pil_set_table(FILE* dst, int indent, const char* name, unsigned int type_number, const pil_set_t* pil_set_table)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	char buffer[40];
	for (unsigned int i = 0; i < type_number; i++)
	for (unsigned int j = 0; j < type_number; j++)
	{
		int required = snprintf(buffer, sizeof buffer, "pil_set.%d.%d", i, j);
		if ((unsigned int)required >= sizeof buffer)
		{
			assert(0);
			/* TODO: Reallocate instead of failing. */
		}
		emit_pil_set(dst, indent+1, buffer, &pil_set_table[i * type_number + j]);
	}
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

static void emit_part_type_color(FILE* dst, int indent, const char* name, const part_type_t* part_type)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_rgb(dst, indent+1, "base", part_type->br, part_type->bg, part_type->bb);
	emit_leaf_rgbvec(dst, indent+1, "speed", part_type->sr, part_type->sg, part_type->sb);
	emit_leaf_rgbvec(dst, indent+1, "pressure", part_type->pr, part_type->pg, part_type->pb);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

static void emit_part_type_table(FILE* dst, int indent, const char* name, unsigned int type_number, const part_type_t* type_table)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	char buffer[40];
	for (unsigned int i = 0; i < type_number; i++)
	{
		int required = snprintf(buffer, sizeof buffer, "pil_set.%d", i);
		if ((unsigned int)required >= sizeof buffer)
		{
			assert(0);
			/* TODO: Reallocate instead of failing. */
		}
		emit_part_type_color(dst, indent+1, buffer, &type_table[i]);
	}
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

static void emit_univ_info(FILE* dst, int indent, const char* name, const universe_info_t* info)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_int(dst, indent+1, "part_number", info->part_number);
	emit_leaf_int(dst, indent+1, "type_number", info->type_number);
	//emit_leaf_int(dst, indent+1, "change_type_law_number", info->change_type_law_number);
	emit_leaf_int(dst, indent+1, "pil_step_number", info->pil_step_number);
	emit_leaf_float(dst, indent+1, "pil_step_dist", info->pil_step_dist);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

static void emit_version(FILE* dst, int indent, const char* name, int major, int minor, int patch)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_int(dst, indent+1, "major", major);
	emit_leaf_int(dst, indent+1, "minor", minor);
	emit_leaf_int(dst, indent+1, "patch", patch);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void serialize_universe_rules(const char* dst_filepath,
	const universe_info_t* info, const pil_set_t* pil_set_table, const part_type_t* type_table)
{
	FILE* dst = fopen(dst_filepath, "w");
	int indent = 0;

	emit_version(dst, indent, "ipsys_version", 0, 1, 1);
	emit_leaf_string(dst, indent, "ruleset_name", "uwu");
	emit_univ_info(dst, indent, "univ_info", info);
	emit_part_type_table(dst, indent, "part_type_table", info->type_number, type_table);
	emit_pil_set_table(dst, indent, "pil_set_table", info->type_number, pil_set_table);
	
	fclose(dst);
}

#endif

void ipsysdify_version(ipsysd_node_t* out_node,
	unsigned int major, unsigned int minor, unsigned int patch)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "major" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = major
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "minor" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = minor
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "patch" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = patch
		}
	};
}

void ipsysdify_ruleset_name(ipsysd_node_t* out_node,
	const char* ruleset_name)
{
	out_node->type = IPSYSD_STRING;
	out_node->value_string = ruleset_name;
}

void ipsysdify_univ_info(ipsysd_node_t* out_node,
	const universe_info_t* info)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "part_number" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = info->part_number
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "type_number" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = info->type_number
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pil_step_number" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = info->pil_step_number
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pil_step_dist" },
		.value = {
			.type = IPSYSD_FLOAT,
			.value_float = info->pil_step_dist
		}
	};
}

void ipsysdify_part_type(ipsysd_node_t* out_node,
	const part_type_t* type)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "base" },
		.value = {
			.type = IPSYSD_RGB,
			.value_rgb = {.r = type->br, .g = type->bg, .b = type->bb }
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "speed" },
		.value = {
			.type = IPSYSD_RGBVEC,
			.value_rgbvec = {.r = type->sr, .g = type->sg, .b = type->sb }
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pressure" },
		.value = {
			.type = IPSYSD_RGBVEC,
			.value_rgbvec = {.r = type->pr, .g = type->pg, .b = type->pb }
		}
	};
}

void ipsysdify_part_type_table(ipsysd_node_t* out_node,
	unsigned int type_number, const part_type_t* type_table)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;

	unsigned int index_array[1];
	for (unsigned int i = 0; i < type_number; i++)
	{
		DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
		da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "part_type" } };
		da->arr[da->len-1].key.index_number = 1;
		da->arr[da->len-1].key.index_array = malloc(sizeof index_array);
		index_array[0] = i;
		memcpy(da->arr[da->len-1].key.index_array, index_array, sizeof index_array);
		ipsysdify_part_type(&da->arr[da->len-1].value, &type_table[i]);
	}
}

void ipsysdify_pil_set(ipsysd_node_t* out_node,
	const pil_set_t* pil_set)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "attraction" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->attraction, sizeof(pil_t));
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "angle" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->angle, sizeof(pil_t));
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "speed" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->speed, sizeof(pil_t));
}

void ipsysdify_pil_set_table(ipsysd_node_t* out_node,
	unsigned int type_number, const pil_set_t* pil_set_table)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;

	unsigned int index_array[2];
	for (unsigned int i = 0; i < type_number; i++)
	for (unsigned int j = 0; j < type_number; j++)
	{
		DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
		da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "part_type" } };
		da->arr[da->len-1].key.index_number = 2;
		da->arr[da->len-1].key.index_array = malloc(sizeof index_array);
		index_array[0] = i;
		index_array[1] = j;
		memcpy(da->arr[da->len-1].key.index_array, index_array, sizeof index_array);
		ipsysdify_pil_set(&da->arr[da->len-1].value, &pil_set_table[i * type_number + j]);
	}
}

void ipsysdify_universe_rules(ipsysd_node_t* out_node,
	const universe_info_t* info, const pil_set_t* pil_set_table, const part_type_t* type_table)
{
	out_node->type = IPSYSD_NODE_DA;

	ipsysd_node_da_t* da;
	da = &out_node->value_node_da;
	*da = (ipsysd_node_da_t){0};

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "ipsys_version" } };
	ipsysdify_version(&da->arr[da->len-1].value, 0, 1, 1);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "ruleset_name" } };
	ipsysdify_ruleset_name(&da->arr[da->len-1].value, "uwu");

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "univ_info" } };
	ipsysdify_univ_info(&da->arr[da->len-1].value, info);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "part_type_table" } };
	ipsysdify_part_type_table(&da->arr[da->len-1].value, info->type_number, type_table);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "pil_set_table" } };
	ipsysdify_pil_set_table(&da->arr[da->len-1].value, info->type_number, pil_set_table);
}

static void emit_ipsysd_node(FILE *dst, int indent, const ipsysd_key_t* key, const ipsysd_node_t* node);

#if 1
void serialize_universe_rules(const char* dst_filepath,
	const universe_info_t* info, const pil_set_t* pil_set_table, const part_type_t* type_table)
{
	#if 0
	emit_version(dst, indent, "ipsys_version", 0, 1, 1);
	emit_leaf_string(dst, indent, "ruleset_name", "uwu");
	emit_univ_info(dst, indent, "univ_info", info);
	emit_part_type_table(dst, indent, "part_type_table", info->type_number, type_table);
	emit_pil_set_table(dst, indent, "pil_set_table", info->type_number, pil_set_table);
	#endif

	ipsysd_node_t node;
	ipsysdify_universe_rules(&node, info, pil_set_table, type_table);
	FILE* dst = fopen(dst_filepath, "w");
	emit_ipsysd_node(dst, 0, NULL, &node);
	fclose(dst);
}
#endif

/* TODO: Make this parser safe and robust! And find better variable names! */

#define PARSE_IPSYSD_ALLOC(src_, out_, while_cond_) \
	do \
	{ \
		long beginning = ftell(src_); \
		int length = 0; \
		int c; \
		while (c = fgetc(src_), (while_cond_)) \
		{ \
			length++; \
		} \
		(out_) = malloc(length + 1); \
		fseek(src_, beginning, SEEK_SET); \
		fgets((out_), length + 1, src_); \
		(out_)[length] = '\0'; \
	} \
	while (0)

static void parse_ipsysd_name(FILE* src, char** out_name)
{
	fscanf(src, " ");
	PARSE_IPSYSD_ALLOC(src, *out_name, islower(c) || c == '_');
}

static void parse_ipsysd_string(FILE* src, char** out_string_content)
{
	fscanf(src, " ");
	if (fgetc(src) != '\"')
	{
		assert(0);
	}
	PARSE_IPSYSD_ALLOC(src, *out_string_content, c != '\"');
	if (fgetc(src) != '\"')
	{
		assert(0);
	}
}

static void parse_ipsysd_node(FILE* src, ipsysd_node_t* out_node);

static void parse_ipsysd_kv(FILE* src, ipsysd_kv_t* out_kv)
{
	fscanf(src, " ");
	parse_ipsysd_name(src, &out_kv->key.name);
	unsigned int len = 0;
	unsigned int cap = 0;
	unsigned int* arr = NULL;
	unsigned int value;
	while (fscanf(src, " .%u", &value) != 0)
	{
		DA_LENGTHEN(len += 1, cap, arr, unsigned int);
		arr[len-1] = value;
	}
	out_kv->key.index_number = len;
	out_kv->key.index_array = arr;
	fscanf(src, " ");
	if (fgetc(src) != '{')
	{
		assert(0);
	}
	parse_ipsysd_node(src, &out_kv->value);
	fscanf(src, " ");
	if (fgetc(src) != '}')
	{
		assert(0);
	}
}

static void parse_ipsysd_node(FILE* src, ipsysd_node_t* out_node)
{
	fscanf(src, " ");
	long c_pos = ftell(src);
	int c = fgetc(src);
	if (c == '.')
	{
		char* type;
		parse_ipsysd_name(src, &type);
		if (strcmp(type, "int") == 0)
		{
			out_node->type = IPSYSD_INT;
			fscanf(src, " %d", &out_node->value_int);
			return;
		}
		else if (strcmp(type, "float") == 0)
		{
			out_node->type = IPSYSD_FLOAT;
			fscanf(src, " %f", &out_node->value_float);
			return;
		}
		else if (strcmp(type, "string") == 0)
		{
			out_node->type = IPSYSD_STRING;
			parse_ipsysd_string(src, &out_node->value_string);
			return;
		}
		else if (strcmp(type, "rgb") == 0)
		{
			out_node->type = IPSYSD_RGB;
			fscanf(src, " %f %f %f",
				&out_node->value_rgb.r,
				&out_node->value_rgb.g,
				&out_node->value_rgb.b);
			return;
		}
		else if (strcmp(type, "rgbvec") == 0)
		{
			out_node->type = IPSYSD_RGBVEC;
			fscanf(src, " %f %f %f",
				&out_node->value_rgbvec.r,
				&out_node->value_rgbvec.g,
				&out_node->value_rgbvec.b);
			return;
		}
		else if (strcmp(type, "pil") == 0)
		{
			out_node->type = IPSYSD_PIL;
			long beginning = ftell(src);
			float dummy;
			unsigned int step_count = 0;
			while (fscanf(src, " %f %f", &dummy, &dummy) != 0)
			{
				step_count++;
			}
			assert(step_count == 90);
			out_node->value_pil = malloc(sizeof(pil_t));
			fseek(src, beginning, SEEK_SET);
			for (unsigned int i = 0; i < step_count; i++)
			{
				fscanf(src, " %f %f",
					&out_node->value_pil->steps[i].offset,
					&out_node->value_pil->steps[i].slope);
			}
			return;
		}
		else
		{
			assert(0);
		}
	}
	else if (islower(c) || c == '_')
	{
		out_node->type = IPSYSD_NODE_DA;
		ipsysd_node_da_t* da = &out_node->value_node_da;
		*da = (ipsysd_node_da_t){0};
		fseek(src, c_pos, SEEK_SET);
		while (1)
		{
			fscanf(src, " ");
			long pos = ftell(src);
			int c = fgetc(src);
			fseek(src, pos, SEEK_SET);
			if (!(islower(c) || c == '_'))
			{
				break;
			}
			DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
			parse_ipsysd_kv(src, &da->arr[da->len-1]);
		}
		return;
	}
	else
	{
		assert(0);
	}
}

static void parse_ipsysd_file(const char* src_filepath, ipsysd_node_t* out_node)
{
	FILE* src = fopen(src_filepath, "r");
	if (src == NULL)
	{
		fprintf(stderr, "Cannot open %s\n", src_filepath);
		return;
	}

	parse_ipsysd_node(src, out_node);

	fclose(src);
}

static void emit_key(FILE* dst, const ipsysd_key_t* key)
{
	if (key == NULL)
	{
		fprintf(dst, "here");
		return;
	}
	fprintf(dst, "%s", key->name);
	for (unsigned int i = 0; i < key->index_number; i++)
	{
		fprintf(dst, ".%d", key->index_array[i]);
	}
}

static void emit_void(FILE* dst, int indent, const ipsysd_key_t* key)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".void");
}

static void emit_int(FILE* dst, int indent, const ipsysd_key_t* key, int value)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".int %d ", value);
}

static void emit_float(FILE* dst, int indent, const ipsysd_key_t* key, float value)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".float "FLOAT_FMT" ", value);
}

static void emit_string(FILE* dst, int indent, const ipsysd_key_t* key, const char* value)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".string \"%s\" ", value);
}

static void emit_rgb(FILE* dst, int indent, const ipsysd_key_t* key, float r, float g, float b)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".rgb "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" ", r, g, b);
}

static void emit_rgbvec(FILE* dst, int indent, const ipsysd_key_t* key, float r, float g, float b)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".rgbvec "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" ", r, g, b);
}

static void emit_pil(FILE* dst, int indent, const ipsysd_key_t* key, const pil_t* pil)
{
	//emit_indent(dst, indent);
	//emit_key(dst, key);
	fprintf(dst, ".pil ");
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fprintf(dst, ""FLOAT_FMT" "FLOAT_FMT" ", pil->steps[i].offset, pil->steps[i].slope);
	}
	//fprintf(dst, "}\n");
}

static void emit_ipsysd_node(FILE *dst, int indent, const ipsysd_key_t* key, const ipsysd_node_t* node)
{
	if (key != NULL)
	{
		emit_indent(dst, indent++);
		emit_key(dst, key);
		fprintf(dst, " {%c", node->type == IPSYSD_NODE_DA ? '\n' : ' ');
	}
	switch (node->type)
	{
		case IPSYSD_VOID:
			emit_void(dst, indent, key);
		break;
		case IPSYSD_INT:
			emit_int(dst, indent, key, node->value_int);
		break;
		case IPSYSD_FLOAT:
			emit_float(dst, indent, key, node->value_float);
		break;
		case IPSYSD_STRING:
			emit_string(dst, indent, key, node->value_string);
		break;
		case IPSYSD_RGB:
			emit_rgb(dst, indent, key,
				node->value_rgb.r,
				node->value_rgb.g,
				node->value_rgb.b);
		break;
		case IPSYSD_RGBVEC:
			emit_rgbvec(dst, indent, key,
				node->value_rgbvec.r,
				node->value_rgbvec.g,
				node->value_rgbvec.b);
		break;
		case IPSYSD_PIL:
			emit_pil(dst, indent, key, node->value_pil);
		break;
		case IPSYSD_NODE_DA:
			//emit_indent(dst, indent);
			//emit_key(dst, key);
			//fprintf(dst, " {\n");
			for (unsigned int i = 0; i < node->value_node_da.len; i++)
			{
				emit_ipsysd_node(dst, indent,//+1,
					&node->value_node_da.arr[i].key,
					&node->value_node_da.arr[i].value);
			}
			//emit_indent(dst, indent);
			//fprintf(dst, "}\n");
		break;
		default:
			assert(0);
		break;
	}
	if (key != NULL)
	{
		if (node->type == IPSYSD_NODE_DA)
		{
			emit_indent(dst, --indent);
			fprintf(dst, "}\n");
		}
		else
		{
			fprintf(dst, "}\n");
		}
	}
}

void deserialize_ipsysd_file(const char* src_filepath,
	universe_info_t* out_info, pil_set_t** out_pil_set_table, part_type_t** out_type_table)
{
	ipsysd_node_t node;
	parse_ipsysd_file(src_filepath, &node);
	emit_ipsysd_node(stdout, 0, NULL, &node);

	printf("\n");
	assert(node.type == IPSYSD_NODE_DA);
	ipsysd_node_da_t* root_da = &node.value_node_da;
	for (unsigned int i = 0; i < root_da->len; i++)
	{
		if (strcmp(root_da->arr[i].key.name, "univ_info") == 0)
		{
			assert(root_da->arr[i].value.type == IPSYSD_NODE_DA);
			ipsysd_node_da_t* info_da = &root_da->arr[i].value.value_node_da;
			for (unsigned int j = 0; j < info_da->len; j++)
			{
				if (strcmp(info_da->arr[j].key.name, "type_number") == 0)
				{
					assert(info_da->arr[j].value.type == IPSYSD_INT);
					printf("type number: %d\n", info_da->arr[j].value.value_int);
				}
			}
		}
	}
}

#if 0

void parse_expect_leaf_int(FILE* src, int* out_value)
{
	fscanf(src, " { .int %d }", out_value);
}

void parse_expect_leaf_float(FILE* src, float* out_value)
{
	fscanf(src, " { .float %f }", out_value);
}

void parse_expect_leaf_string(FILE* src, char** out_value)
{
	fscanf(src, " { .string \"");
	int length = 0;
	long beginning = ftell(src);
	int c;
	while (c = fgetc(src), c != "\"" || c != EOF)
	{
		length++;
	}
	assert(c != EOF);
	*out_value = malloc(length + 1);
	fseek(src, beginning, SEEK_SET);
	fgets(*out_value, length, src);
	*out_value[length] = '\0';
	fscanf(src, "\" }");
}

void parse_expect_leaf_rgb(FILE* src, float* out_r, float* out_g, float* out_b)
{
	fscanf(src, " { .rgb %f %f %f }", out_r, out_g, out_b);
}

void parse_expect_leaf_rgbvec(FILE* src, float* out_r, float* out_g, float* out_b)
{
	fscanf(src, " { .rgbvec %f %f %f }", out_r, out_g, out_b);
}

void parse_expect_leaf_pil(FILE* src, pil_t* out_pil)
{
	fsacnf(src, " { .pil");
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fsacnf(src, " %f %f", &out_pil->steps[i].offset, &out_pil->steps[i].slope);
	}
	fsacnf(src, " }");
}

void parse_expect_pil_set(FILE* src, pil_set_t* out_pil_set)
{
	fscanf(src, " {");
	fscanf(src, " attraction");
	parse_expect_leaf_pil(src, &out_pil_set->attraction);
	fscanf(src, " angle");
	parse_expect_leaf_pil(src, &out_pil_set->angle);
	fscanf(src, " speed");
	parse_expect_leaf_pil(src, &out_pil_set->speed);
	fprintf(src, " }");
}

void parse_expect_pil_set_table(FILE* src, unsigned int type_number, pil_set_t* out_pil_set_table)
{
	fscanf(src, " {");
	char buffer[40];
	for (unsigned int i = 0; i < type_number; i++)
	for (unsigned int j = 0; j < type_number; j++)
	{
		int required = snprintf(buffer, sizeof buffer, "pil_set.%d.%d", i, j);
		if ((unsigned int)required >= sizeof buffer)
		{
			assert(0);
			/* TODO: Reallocate instead of failing. */
		}
		emit_pil_set(dst, indent+1, buffer, &pil_set_table[i * type_number + j]);
	}
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void deserialize_universe_rules(const char* src_filepath,
	universe_info_t* info, pil_set_t** pil_set_table, part_type_t** type_table)
{
	FILE* src = fopen(src_filepath, "r");

	// TODO

	fclose(src);
}

#endif
