
#include "universe.h"
#include <assert.h>
#include <stdio.h>

void emit_indent(FILE* dst, int indent)
{
	for (unsigned int i = 0; i < (unsigned int)indent; i++)
	{
		fprintf(dst, "\t");
	}
}

void emit_leaf_int(FILE* dst, int indent, const char* name, int value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .int %d }\n", name, value);
}

void emit_leaf_float(FILE* dst, int indent, const char* name, float value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .float %f }\n", name, value);
}

void emit_leaf_string(FILE* dst, int indent, const char* name, const char* value)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .string \"%s\" }\n", name, value);
}

void emit_leaf_rgb(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgb %f %f %f }\n", name, r, g, b);
}

void emit_leaf_rgbvec(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgbvec %f %f %f }\n", name, r, g, b);
}

void emit_leaf_pil(FILE* dst, int indent, const char* name, const pil_t* pil)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .pil ", name);
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fprintf(dst, "%f %f ", pil->steps[i].offset, pil->steps[i].slope);
	}
	fprintf(dst, "}\n");
}

void emit_pil_set(FILE* dst, int indent, const char* name, const pil_set_t* pil_set)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_pil(dst, indent+1, "attraction", &pil_set->attraction);
	emit_leaf_pil(dst, indent+1, "angle", &pil_set->angle);
	emit_leaf_pil(dst, indent+1, "speed", &pil_set->speed);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void emit_pil_set_table(FILE* dst, int indent, const char* name, unsigned int type_number, const pil_set_t* pil_set_table)
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

void emit_part_type_color(FILE* dst, int indent, const char* name, const part_type_t* part_type)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_rgb(dst, indent+1, "base", part_type->br, part_type->bg, part_type->bb);
	emit_leaf_rgbvec(dst, indent+1, "speed", part_type->sr, part_type->sg, part_type->sb);
	emit_leaf_rgbvec(dst, indent+1, "pressure", part_type->pr, part_type->pg, part_type->pb);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void emit_part_type_table(FILE* dst, int indent, const char* name, unsigned int type_number, const part_type_t* type_table)
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

void emit_univ_info(FILE* dst, int indent, const char* name, const universe_info_t* info)
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

void emit_version(FILE* dst, int indent, const char* name, int major, int minor, int patch)
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

enum ipsysd_type_t
{
	IPSYSD_INT,
};
typedef enum ipsysd_type_t ipsysd_type_t;

struct ipsysd_t
{
	ipsysd_type_t type;
	union
	{
		int value_int;
	};
};
typedef struct ipsysd_t ipsysd_t;

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
