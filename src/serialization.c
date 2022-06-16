
#include "serialization.h"
#include "universe.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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
	char* ruleset_name)
{
	out_node->type = IPSYSD_STRING;
	out_node->value_string = ruleset_name;
}

void ipsysdify_univ_info(ipsysd_node_t* out_node,
	universe_info_t const* info)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "particle_number" },
		.value = {
			.type = IPSYSD_INT,
			.value_int = info->part_number
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "particle_type_number" },
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
		.key = { .name = "pil_step_length" },
		.value = {
			.type = IPSYSD_FLOAT,
			.value_float = info->pil_step_dist
		}
	};
}

void ipsysdify_part_type(ipsysd_node_t* out_node,
	part_type_t const* type)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "color_base" },
		.value = {
			.type = IPSYSD_RGB,
			.value_rgb = {.r = type->br, .g = type->bg, .b = type->bb }
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "color_speed" },
		.value = {
			.type = IPSYSD_RGBVEC,
			.value_rgbvec = {.r = type->sr, .g = type->sg, .b = type->sb }
		}
	};
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "color_pressure" },
		.value = {
			.type = IPSYSD_RGBVEC,
			.value_rgbvec = {.r = type->pr, .g = type->pg, .b = type->pb }
		}
	};
}

void ipsysdify_part_type_table(ipsysd_node_t* out_node,
	unsigned int type_number, part_type_t const* type_table)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;

	unsigned int index_array[1];
	for (unsigned int i = 0; i < type_number; i++)
	{
		DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
		da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "particle_type" } };
		da->arr[da->len-1].key.index_number = 1;
		da->arr[da->len-1].key.index_array = malloc(sizeof index_array);
		index_array[0] = i;
		memcpy(da->arr[da->len-1].key.index_array, index_array, sizeof index_array);
		ipsysdify_part_type(&da->arr[da->len-1].value, &type_table[i]);
	}
}

void ipsysdify_pil_set(ipsysd_node_t* out_node,
	pil_set_t const* pil_set)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pil_attraction" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->attraction, sizeof(pil_t));
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pil_angle" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->angle, sizeof(pil_t));
	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){
		.key = { .name = "pil_speed" },
		.value = { .type = IPSYSD_PIL },
	};
	da->arr[da->len-1].value.value_pil = malloc(sizeof(pil_t));
	memcpy(da->arr[da->len-1].value.value_pil, &pil_set->speed, sizeof(pil_t));
}

void ipsysdify_pil_set_table(ipsysd_node_t* out_node,
	unsigned int type_number, pil_set_t const* pil_set_table)
{
	out_node->type = IPSYSD_NODE_DA;
	ipsysd_node_da_t* da = &out_node->value_node_da;

	unsigned int index_array[2];
	for (unsigned int i = 0; i < type_number; i++)
	for (unsigned int j = 0; j < type_number; j++)
	{
		DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
		da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "pil_set" } };
		da->arr[da->len-1].key.index_number = 2;
		da->arr[da->len-1].key.index_array = malloc(sizeof index_array);
		index_array[0] = i;
		index_array[1] = j;
		memcpy(da->arr[da->len-1].key.index_array, index_array, sizeof index_array);
		ipsysdify_pil_set(&da->arr[da->len-1].value, &pil_set_table[i * type_number + j]);
	}
}

void ipsysdify_universe_rules(ipsysd_node_t* out_node,
	universe_info_t const* info, pil_set_t const* pil_set_table, part_type_t const* type_table)
{
	out_node->type = IPSYSD_NODE_DA;

	ipsysd_node_da_t* da;
	da = &out_node->value_node_da;
	*da = (ipsysd_node_da_t){0};

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "ipsys_version" } };
	ipsysdify_version(&da->arr[da->len-1].value, 0, 1, 1);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "name" } };
	ipsysdify_ruleset_name(&da->arr[da->len-1].value, "uwu");

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "information" } };
	ipsysdify_univ_info(&da->arr[da->len-1].value, info);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "particle_type_table" } };
	ipsysdify_part_type_table(&da->arr[da->len-1].value, info->type_number, type_table);

	DA_LENGTHEN(da->len += 1, da->cap, da->arr, ipsysd_kv_t);
	da->arr[da->len-1] = (ipsysd_kv_t){ .key = { .name = "pil_set_table" } };
	ipsysdify_pil_set_table(&da->arr[da->len-1].value, info->type_number, pil_set_table);
}

static void emit_ipsysd_node(FILE *dst, int indent,
	ipsysd_key_t const* key, ipsysd_node_t const* node);

void serialize_universe_rules(char const* dst_filepath,
	universe_info_t const* info, pil_set_t const* pil_set_table, part_type_t const* type_table)
{
	ipsysd_node_t node;
	ipsysdify_universe_rules(&node, info, pil_set_table, type_table);
	FILE* dst = fopen(dst_filepath, "w");
	emit_ipsysd_node(dst, 0, NULL, &node);
	fclose(dst);
}

/* TODO: Make this parser safe and robust! And find better variable names! */

#define PARSE_IPSYSD_ALLOC(src_, out_, while_cond_) \
	do \
	{ \
		long const beginning = ftell(src_); \
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
	long const c_pos = ftell(src);
	int const c = fgetc(src);
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
			long const beginning = ftell(src);
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
			long const pos = ftell(src);
			int const c = fgetc(src);
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

static void parse_ipsysd_file(char const* src_filepath, ipsysd_node_t* out_node)
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

static void emit_key(FILE* dst, const ipsysd_key_t* key)
{
	if (key == NULL)
	{
		assert(0);
	}
	fprintf(dst, "%s", key->name);
	for (unsigned int i = 0; i < key->index_number; i++)
	{
		fprintf(dst, ".%d", key->index_array[i]);
	}
}

static void emit_void(FILE* dst)
{
	fprintf(dst, ".void");
}

static void emit_int(FILE* dst, int value)
{
	fprintf(dst, ".int %d ", value);
}

static void emit_float(FILE* dst, float value)
{
	fprintf(dst, ".float "FLOAT_FMT" ", value);
}

static void emit_string(FILE* dst, char const* value)
{
	fprintf(dst, ".string \"%s\" ", value);
}

static void emit_rgb(FILE* dst, float r, float g, float b)
{
	fprintf(dst, ".rgb "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" ", r, g, b);
}

static void emit_rgbvec(FILE* dst, float r, float g, float b)
{
	fprintf(dst, ".rgbvec "FLOAT_FMT" "FLOAT_FMT" "FLOAT_FMT" ", r, g, b);
}

static void emit_pil(FILE* dst, pil_t const* pil)
{
	fprintf(dst, ".pil ");
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fprintf(dst, ""FLOAT_FMT" "FLOAT_FMT" ", pil->steps[i].offset, pil->steps[i].slope);
	}
}

static void emit_ipsysd_node(FILE* dst, int indent,
	ipsysd_key_t const* key, ipsysd_node_t const* node)
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
			emit_void(dst);
		break;
		case IPSYSD_INT:
			emit_int(dst, node->value_int);
		break;
		case IPSYSD_FLOAT:
			emit_float(dst, node->value_float);
		break;
		case IPSYSD_STRING:
			emit_string(dst, node->value_string);
		break;
		case IPSYSD_RGB:
			emit_rgb(dst,
				node->value_rgb.r,
				node->value_rgb.g,
				node->value_rgb.b);
		break;
		case IPSYSD_RGBVEC:
			emit_rgbvec(dst,
				node->value_rgbvec.r,
				node->value_rgbvec.g,
				node->value_rgbvec.b);
		break;
		case IPSYSD_PIL:
			emit_pil(dst, node->value_pil);
		break;
		case IPSYSD_NODE_DA:
			for (unsigned int i = 0; i < node->value_node_da.len; i++)
			{
				emit_ipsysd_node(dst, indent,
					&node->value_node_da.arr[i].key,
					&node->value_node_da.arr[i].value);
			}
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

void deserialize_ipsysd_file(char const* src_filepath,
	universe_info_t* out_info, pil_set_t** out_pil_set_table, part_type_t** out_type_table)
{
	ipsysd_node_t node;
	parse_ipsysd_file(src_filepath, &node);
	emit_ipsysd_node(stdout, 0, NULL, &node);

	printf("\n");
	int type_number = -1;
	assert(node.type == IPSYSD_NODE_DA);
	ipsysd_node_da_t* root_da = &node.value_node_da;
	for (unsigned int i = 0; i < root_da->len; i++)
	{
		if (strcmp(root_da->arr[i].key.name, "information") == 0)
		{
			assert(root_da->arr[i].value.type == IPSYSD_NODE_DA);
			ipsysd_node_da_t* info_da = &root_da->arr[i].value.value_node_da;
			for (unsigned int j = 0; j < info_da->len; j++)
			{
				if (strcmp(info_da->arr[j].key.name, "particle_type_number") == 0)
				{
					assert(info_da->arr[j].value.type == IPSYSD_INT);
					out_info->type_number = info_da->arr[j].value.value_int;
					printf("type_number: %d\n", out_info->type_number);
					if (type_number == -1)
					{
						type_number = out_info->type_number;
					}
					else
					{
						assert(type_number == (int)out_info->type_number);
					}
				}
				else if (strcmp(info_da->arr[j].key.name, "particle_number") == 0)
				{
					assert(info_da->arr[j].value.type == IPSYSD_INT);
					out_info->part_number = info_da->arr[j].value.value_int;
					printf("part_number: %d\n", out_info->part_number);
					assert(out_info->part_number == (256 * 6));
				}
				else if (strcmp(info_da->arr[j].key.name, "pil_step_number") == 0)
				{
					assert(info_da->arr[j].value.type == IPSYSD_INT);
					out_info->pil_step_number = info_da->arr[j].value.value_int;
					printf("pil_step_number: %d\n", out_info->pil_step_number);
					assert(out_info->pil_step_number == PIL_STEP_NUMBER);
				}
				else if (strcmp(info_da->arr[j].key.name, "pil_step_length") == 0)
				{
					assert(info_da->arr[j].value.type == IPSYSD_FLOAT);
					out_info->pil_step_dist = info_da->arr[j].value.value_float;
					printf("pil_step_dist: %f\n", out_info->pil_step_dist);
				}
			}
		}
		else if (strcmp(root_da->arr[i].key.name, "particle_type_table") == 0)
		{
			assert(root_da->arr[i].value.type == IPSYSD_NODE_DA);
			ipsysd_node_da_t* ptt_da = &root_da->arr[i].value.value_node_da;
			if (type_number == -1)
			{
				type_number = ptt_da->len;
			}
			else
			{
				assert(type_number == (int)ptt_da->len);
			}
			*out_type_table = calloc(type_number, sizeof(part_type_t));
			printf("part_type_table length: %d\n", type_number);
			for (unsigned int j = 0; j < ptt_da->len; j++)
			{
				if (strcmp(ptt_da->arr[j].key.name, "particle_type") == 0)
				{
					assert(ptt_da->arr[j].key.index_number == 1);
					int pt_i = ptt_da->arr[j].key.index_array[0];
					assert(ptt_da->arr[j].value.type == IPSYSD_NODE_DA);
					ipsysd_node_da_t* pt_da = &ptt_da->arr[j].value.value_node_da;
					for (unsigned int k = 0; k < pt_da->len; k++)
					{
						if (strcmp(pt_da->arr[k].key.name, "color_base") == 0)
						{
							assert(pt_da->arr[k].value.type == IPSYSD_RGB);
							(*out_type_table)[pt_i].br = pt_da->arr[k].value.value_rgb.r;
							(*out_type_table)[pt_i].bg = pt_da->arr[k].value.value_rgb.g;
							(*out_type_table)[pt_i].bb = pt_da->arr[k].value.value_rgb.b;
							printf("base: %f %f %f\n",
								(*out_type_table)[pt_i].br,
								(*out_type_table)[pt_i].bg,
								(*out_type_table)[pt_i].bb);
						}
						else if (strcmp(pt_da->arr[k].key.name, "color_speed") == 0)
						{
							assert(pt_da->arr[k].value.type == IPSYSD_RGBVEC);
							(*out_type_table)[pt_i].sr = pt_da->arr[k].value.value_rgbvec.r;
							(*out_type_table)[pt_i].sg = pt_da->arr[k].value.value_rgbvec.g;
							(*out_type_table)[pt_i].sb = pt_da->arr[k].value.value_rgbvec.b;
							printf("speed: %f %f %f\n",
								(*out_type_table)[pt_i].sr,
								(*out_type_table)[pt_i].sg,
								(*out_type_table)[pt_i].sb);
						}
						else if (strcmp(pt_da->arr[k].key.name, "color_pressure") == 0)
						{
							assert(pt_da->arr[k].value.type == IPSYSD_RGBVEC);
							(*out_type_table)[pt_i].pr = pt_da->arr[k].value.value_rgbvec.r;
							(*out_type_table)[pt_i].pg = pt_da->arr[k].value.value_rgbvec.g;
							(*out_type_table)[pt_i].pb = pt_da->arr[k].value.value_rgbvec.b;
							printf("pressure: %f %f %f\n",
								(*out_type_table)[pt_i].pr,
								(*out_type_table)[pt_i].pg,
								(*out_type_table)[pt_i].pb);
						}
					}
				}
			}
		}
		else if (strcmp(root_da->arr[i].key.name, "pil_set_table") == 0)
		{
			assert(root_da->arr[i].value.type == IPSYSD_NODE_DA);
			ipsysd_node_da_t* pst_da = &root_da->arr[i].value.value_node_da;
			if (type_number == -1)
			{
				assert(0);
			}
			else
			{
				assert(type_number * type_number == (int)pst_da->len);
			}
			*out_pil_set_table = calloc(type_number * type_number, sizeof(pil_set_t));
			printf("pil_set_table length: %d\n", type_number * type_number);
			for (unsigned int j = 0; j < pst_da->len; j++)
			{
				if (strcmp(pst_da->arr[j].key.name, "pil_set") == 0)
				{
					assert(pst_da->arr[j].key.index_number == 2);
					int pt_i = pst_da->arr[j].key.index_array[0];
					int pt_j = pst_da->arr[j].key.index_array[1];
					assert(pst_da->arr[j].value.type == IPSYSD_NODE_DA);
					ipsysd_node_da_t* ps_da = &pst_da->arr[j].value.value_node_da;
					for (unsigned int k = 0; k < ps_da->len; k++)
					{
						if (strcmp(ps_da->arr[k].key.name, "pil_attraction") == 0)
						{
							assert(ps_da->arr[k].value.type == IPSYSD_PIL);
							memcpy(&((*out_pil_set_table)[pt_i * type_number + pt_j].attraction),
								ps_da->arr[k].value.value_pil, sizeof(pil_t));
							printf("attraction: got it\n");
						}
						else if (strcmp(ps_da->arr[k].key.name, "pil_angle") == 0)
						{
							assert(ps_da->arr[k].value.type == IPSYSD_PIL);
							memcpy(&((*out_pil_set_table)[pt_i * type_number + pt_j].angle),
								ps_da->arr[k].value.value_pil, sizeof(pil_t));
							printf("angle: got it\n");
						}
						else if (strcmp(ps_da->arr[k].key.name, "pil_speed") == 0)
						{
							assert(ps_da->arr[k].value.type == IPSYSD_PIL);
							memcpy(&((*out_pil_set_table)[pt_i * type_number + pt_j].speed),
								ps_da->arr[k].value.value_pil, sizeof(pil_t));
							printf("speed: got it\n");
						}
					}
				}
			}
		}
	}
}
