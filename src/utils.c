
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

#if 0
void* xmalloc(size_t size)
{
	void const* ptr = malloc(size);
	if (ptr == NULL)
	{
		fprintf(stderr, "Allocation error: "
			"xmalloc failed to allocate %lu bytes\n", size);
	}
	return ptr;
}

void* xcalloc(size_t number, size_t size)
{
	void const* ptr = calloc(number, size);
	if (ptr == NULL)
	{
		fprintf(stderr, "Allocation error: "
			"xcalloc failed to allocate %lu bytes\n", size);
	}
	return ptr;
}

void* xrealloc(void* ptr, size_t size)
{
	void const* newptr = realloc(ptr, size);
	if (newptr == NULL)
	{
		fprintf(stderr, "Allocation error: "
			"xrealloc failed to reallocate %lu bytes\n", size);
		return NULL;
	}
	return newptr;
}
#endif

char* read_file(char const* filepath)
{
	FILE* file = fopen(filepath, "r");
	if (file == NULL)
	{
		fprintf(stderr, "File error: failed to open \"%s\"\n", filepath);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	long const length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = malloc((length+1) * sizeof(char));
	size_t const length_read = fread(buffer, sizeof(char), length, file);
	fclose(file);
	if (length_read != (unsigned int)length)
	{
		fprintf(stderr, "File error: failed to properly read \"%s\"\n",
			filepath);
		free(buffer);
		return NULL;
	}
	buffer[length] = '\0';
	return buffer;
}

unsigned int umax(unsigned int a, unsigned int b)
{
	return a > b ? a : b;
}
