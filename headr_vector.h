/***********************************************************
 *
 * file: headr_vector.h
 * description: basic generic vector implementation using macros
 * author: jonathan kocevar (@staticvalley)
 *
 ***********************************************************/

#ifndef HEADR_VECTOR
#define HEADR_VECTOR

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define HEADR_VECTOR_MIN_CAP 256

/**_
 * @brief generates a vector container type for a given type
 * @param type_name the name to be set in the vector containers typedef, ie. headr_<type_name>_vec_t
 * @param base_type the given type to make a vector type for
 */
#define headr_vector_generate_type(type_name, base_type)\
typedef struct {\
	base_type* data;\
	size_t length;\
	size_t capacity;\
	size_t _type_size;\
} headr_##type_name##_vec_t;\

/**
 * @brief internally initializes a vector struct
 * @param v pointer to vector struct
 * @param t the base type in the vector variable
 * @note this is just a convienient wrapper for _headr_internal_vector_init
 */
#define headr_vector_init(v, t) do {\
	_headr_internal_vector_init(v, sizeof(t));\
} while (0)\


/**
 * @brief internal function that initializes a vector struct
 * @param v pointer to vector struct
 * @param s size of base type in bytes
 */
#define _headr_internal_vector_init(v, s) do {\
	memset((v), 0, sizeof(*(v)));\
	(v)->_type_size = s;\
} while (0)\

/**
 * @brief frees and zeroes a vector struct
 * @param v pointer to vector struct
 */
#define headr_vector_free(v) do {\
	if((v)->data != NULL) { free((v)->data); }\
	size_t headr_internal_vector_type_size = (v)->_type_size;\
	_headr_internal_vector_init(v, headr_internal_vector_type_size);\
} while(0)\

/**
 * @brief appends an element to a vector
 * @param v pointer to vector struct
 * @param e element to appent to vector
 */
#define headr_vector_append(v, e) do {\
	if((v)->length >= (v)->capacity) {\
		if((v)->capacity == 0) {(v)->capacity = HEADR_VECTOR_MIN_CAP;}\
		else {(v)->capacity *= 2;}\
		_headr_internal_vector_resize(v);\
	}\
	(v)->data[(v)->length] = e;\
	(v)->length++;\
} while (0)\

/**
 * @brief reserves excess space in a vectors capacity
 * @param v pointer to vector struct
 * @param n amount of space to reserve
 */
#define headr_vector_reserve(v, n) do {\
	size_t headr_internal_vector_reserve_diff = (v)->capacity - (v)->length;\
	if(headr_internal_vector_reserve_diff < n) {\
		(v)->capacity += n - headr_internal_vector_reserve_diff;\
		_headr_internal_vector_resize(v);\
	}\
} while(0)\

/**
 * @brief internal function that reallocates memory based on a vectors current capacity
 * @param v pointer to vector struct
 */
#define _headr_internal_vector_resize(v) do {\
	void* headr_internal_vector_data_ptr = realloc((v)->data, (v)->_type_size * (v)->capacity);\
	if(headr_internal_vector_data_ptr == NULL) {\
		headr_vector_free(v);\
		fprintf(stderr, "headr_vector.h: line %d: error: realloc failed\n", __LINE__);\
		exit(EXIT_FAILURE);\
	}\
	(v)->data = headr_internal_vector_data_ptr;\
} while(0)\

/**
 * @brief removes element at index in vector
 * @param v pointer to vector struct
 * @param i index to element to remove
 */
#define headr_vector_remove(v, i) do {\
	if(i < ((v)->length)) { (v)->length--; }\
	for(int j = i; j < (v)->length; j++) { (v)->data[j] = (v)->data[j+1]; } \
} while(0)\

/**
 * @brief concatenates elements of one vector to another
 * @param vd pointer to the destination vector struct to concat to
 * @param vs pointer to the source vector struct to copy from
 */
#define headr_vector_concat(vd, vs) do {\
	headr_vector_reserve(vd, (vs)->length);\
	memcpy(((vd)->data + (vd)->length), (vs)->data, (vs)->length * (vs)->_type_size);\
	(vd)->length += (vs)->length;\
} while(0)\

/**
 * @brief copies the data from one vector to another
 * @param vd pointer to the destination vector struct to copy to
 * @param vs pointer to the source vector struct to copy from
 * @note this operation is destructive, and will not free any data in vd before copying
 */
#define headr_vector_copy(vd, vs) do {\
	_headr_internal_vector_init(vd, (vs)->_type_size);\
	headr_vector_concat(vd, vs);\
} while(0)\

/**
 * automatically generate common vector types
 */ 
headr_vector_generate_type(int, int)
headr_vector_generate_type(char, char)
headr_vector_generate_type(float, float)
headr_vector_generate_type(double, double)
headr_vector_generate_type(str, char*)
headr_vector_generate_type(void, void*)

#endif

