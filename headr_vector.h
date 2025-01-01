#ifndef HEADR_VECTOR
#define HEADR_VECTOR

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define HEADR_VECTOR_MIN_CAP 256

// generates vector type for given type
// resulting name will follow: headr_<type>_vec_t
#define headr_vector_generate_type(type)\
typedef struct {\
	type* data;\
	size_t length;\
	size_t capacity;\
} headr_##type##_vec_t;\

#define headr_vector_init(v) do {\
       memset((v), 0, sizeof(*(v)));\
} while (0)\

#define headr_vector_free(v) do {\
	if((v)->data != NULL) {\
		free((v)->data);\
	}\
	memset((v), 0, sizeof(*(v)));\
} while(0)\

#define headr_vector_append(v, s) do {\
	if((v)->length == (v)->capacity) {\
		if((v)->capacity == 0) {(v)->capacity = HEADR_VECTOR_MIN_CAP;}\
		else {(v)->capacity *= 2;}\
		(v)->data = realloc((v)->data, sizeof((v)->data) * (v)->capacity);\
	}\
	(v)->data[(v)->length] = s;\
	(v)->length++;\
} while (0)\

// generate standard types
headr_vector_generate_type(int)
headr_vector_generate_type(float)
headr_vector_generate_type(char)
headr_vector_generate_type(double)

#endif

