
#include "universe.h"
#include <stdio.h>

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
	}
	fprintf(dst_file, "}\n");

	fprintf(dst_file, "\tpart_type_table {\n");
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
