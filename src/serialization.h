
/* Serialization of various ipsys data and state parts.
 * Implementations are in "serialization.c". */

/* TODO: Comment. */

#ifndef IPSYS_HEADER_SERIALIZATION__
#define IPSYS_HEADER_SERIALIZATION__

#include "universe.h"

void serialize_universe_rules(const char* dst_filepath,
	universe_info_t* info, pil_set_t* pil_set_table, part_type_t* type_table);

#endif /* IPSYS_HEADER_SERIALIZATION__ */
