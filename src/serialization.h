
/* Serialization of various ipsys data and state parts.
 * Implementations are in "serialization.c". */

/* TODO: Comment. */

#ifndef IPSYS_HEADER_SERIALIZATION__
#define IPSYS_HEADER_SERIALIZATION__

#include "universe.h"

enum ipsysd_type_t
{
	IPSYSD_VOID,
	IPSYSD_INT,
	IPSYSD_FLOAT,
	IPSYSD_STRING,
	IPSYSD_RGB,
	IPSYSD_RGBVEC,
	IPSYSD_PIL,
	IPSYSD_NODE_DA,
};
typedef enum ipsysd_type_t ipsysd_type_t;

typedef struct ipsysd_kv_t ipsysd_kv_t;

struct ipsysd_node_da_t
{
	unsigned int len;
	unsigned int cap;
	ipsysd_kv_t* arr;
};
typedef struct ipsysd_node_da_t ipsysd_node_da_t;

struct ipsysd_node_t
{
	ipsysd_type_t type;
	union
	{
		int value_int;
		float value_float;
		char* value_string;
		struct { float r, g, b; } value_rgb;
		struct { float r, g, b; } value_rgbvec;
		pil_t* value_pil;
		ipsysd_node_da_t value_node_da;
	};
};
typedef struct ipsysd_node_t ipsysd_node_t;

struct ipsysd_key_t
{
	char* name;
	unsigned int index_number;
	unsigned int* index_array;
};
typedef struct ipsysd_key_t ipsysd_key_t;

struct ipsysd_kv_t
{
	ipsysd_key_t key;
	ipsysd_node_t value;
};

void serialize_universe_rules(char const* dst_filepath,
	universe_info_t const* info, pil_set_t const* pil_set_table, part_type_t const* type_table);

void deserialize_ipsysd_file(char const* src_filepath,
	universe_info_t* out_info, pil_set_t** out_pil_set_table, part_type_t** out_type_table);

#endif /* IPSYS_HEADER_SERIALIZATION__ */
