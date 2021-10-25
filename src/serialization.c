
#include "universe.h"
#include <stdio.h>

void emit_indent(FILE* dst, int indent)
{
	for (unsigned int i = 0; i < indent; i++)
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
	fprintf(dst, "%s { .string \"%s\" }\n", name value);
}

void emit_leaf_rgb(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgb %f %f %f }\n", name r, g, b);
}

void emit_leaf_rgbvec(FILE* dst, int indent, const char* name, float r, float g, float b)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .rgbvec %f %f %f }\n", name r, g, b);
}

void emit_leaf_pil(FILE* dst, int indent, const char* name, pil_t* pil)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s { .pil ", name);
	for (unsigned int i = 0; i < PIL_STEP_NUMBER; i++)
	{
		fprintf(dst, "%f %f ", pil->steps[i].offset, pil->steps[i].slope);
	}
	fprintf(dst, "}\n");
}

void emit_pil_set(FILE* dst, int indent, const char* name, pil_set_t* pil_set)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	emit_leaf_pil(dst, indent+1, "attraction", &pil_set->attraction);
	emit_leaf_pil(dst, indent+1, "angle", &pil_set->angle);
	emit_leaf_pil(dst, indent+1, "speed", &pil_set->speed);
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void emit_pil_set_table(FILE* dst, int indent, const char* name, unsigned int type_number, pil_set_t* pil_set_table)
{
	emit_indent(dst, indent);
	fprintf(dst, "%s {\n", name);
	for (unsigned int i = 0; i < type_number; i++)
	for (unsigned int j = 0; j < type_number; j++)
	{
		// TODO
	}
	emit_indent(dst, indent);
	fprintf(dst, "}\n");
}

void serialize_universe_rules(const char* dst_filepath,
	universe_info_t* info, pil_set_t* pil_set_table, part_type_t* type_table)
{
	FILE* dst_file = fopen(dst_filepath, "w");

	fprintf(dst_file,
		"ipsys_version {\n"
		"\tmajor { .int %d }\n"
		"\tminor { .int %d }\n"
		"\tpatch { .int %d }\n"
		"}\n",
		0, 1, 1);

	fprintf(dst_file,
		"ruleset_name { .string \"%s\" }\n",
		"uwu");

	fprintf(dst_file,
		"univ_info {\n"
		"\tpart_number { .int %d }\n"
		"\ttype_number { .int %d }\n"
		// "\tchange_type_law_number { .int %d }\n"
		"\tpil_step_number { .int %d }\n"
		"\tpil_step_dist { .float %f }\n"
		"}\n",
		info->part_number,
		info->type_number,
		// info->change_type_law_number,
		info->pil_step_number,
		info->pil_step_dist);

	fprintf(dst_file, "pil_set_table {\n");

	for (unsigned int i = 0; i < info->type_number; i++)
	for (unsigned int j = 0; j < info->type_number; j++)
	{
		unsigned int ij = i * info->type_number + j;

		fprintf(dst_file, "\tpil_set.%d.%d {\n", i, j);
		
		fprintf(dst_file, "\t\tattraction { .pil ");
		for (unsigned int k = 0; k < info->pil_step_number; k++)
		{
			fprintf(dst_file, "%f %f ",
				pil_set_table[ij].attraction.steps[k].offset,
				pil_set_table[ij].attraction.steps[k].slope);
		}
		fprintf(dst_file, "}\n");

		fprintf(dst_file, "\t\tangle { .pil ");
		for (unsigned int k = 0; k < info->pil_step_number; k++)
		{
			fprintf(dst_file, "%f %f ",
				pil_set_table[ij].angle.steps[k].offset,
				pil_set_table[ij].angle.steps[k].slope);
		}
		fprintf(dst_file, "}\n");

		fprintf(dst_file, "\t\tspeed { .pil ");
		for (unsigned int k = 0; k < info->pil_step_number; k++)
		{
			fprintf(dst_file, "%f %f ",
				pil_set_table[ij].speed.steps[k].offset,
				pil_set_table[ij].speed.steps[k].slope);
		}
		fprintf(dst_file, "}\n");

		fprintf(dst_file, "\t}\n");
	}
	fprintf(dst_file, "}\n");

	fprintf(dst_file, "part_type_table {\n");
	for (unsigned int i = 0; i < info->type_number; i++)
	{
		fprintf(dst_file, "\tpart_type.%d {\n", i);
		
		fprintf(dst_file, "\t\tcolor {\n");
		fprintf(dst_file, "\t\t\tbase { .rgb %f %f %f }\n",
			type_table[i].br, type_table[i].bg, type_table[i].bb);
		fprintf(dst_file, "\t\t\tspeed { .rgbvec %f %f %f }\n",
			type_table[i].sr, type_table[i].sg, type_table[i].sb);
		fprintf(dst_file, "\t\t\tpressure { .rgbvec %f %f %f }\n",
			type_table[i].pr, type_table[i].pg, type_table[i].pb);
		fprintf(dst_file, "\t\t}\n");
		
		fprintf(dst_file, "\t}\n");
	}
	fprintf(dst_file, "}\n");

	fclose(dst_file);
}

void deserialize_universe_rules(const char* src_filepath,
	universe_info_t* info, pil_set_t** pil_set_table, part_type_t** type_table)
{
	FILE* src_file = fopen(src_filepath, "w");

	// TODO

	fclose(src_file);
}
