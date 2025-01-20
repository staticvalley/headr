/***********************************************************
 *
 * file: headr_string.h
 * description: helper functions for string handling
 * author: jonathan kocevar (@staticvalley)
 *
 ***********************************************************/

#ifndef HEADR_STRING
#define HEADR_STRING

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef HEADR_USE_SHORTNAMES
#define headr_string_trim_left string_trim_left
#define headr_string_trim_right string_trim_right
#define headr_string_trim string_trim
#define headr_string_slice string_slice
#endif

/**
 * @brief trims the left side of a string given an array of characters to check against
 *
 * @param src the string to trim
 * @param trim_array a string of characters to include in the trim (order is irrelevant)
 *
 * @return 1 on success, or -1 if errors occur
 */
int headr_string_trim_left(char* src, char* trim_array) {
	// check if null
	if(!src) return -1;
	if(!trim_array) return -1;

	// iter through string until character not in trim_array
	size_t iter = 0;
	const size_t src_len = strlen(src);
	while (src[iter] != '\0') {
		if(strchr(trim_array, src[iter]) == NULL) break;
		iter++;
	}

	// nothing to trim
	if(iter == 0) return 1;

	memmove(src, &(src[iter]), (src_len - iter));
	src[src_len - iter] = '\0';
	return 1;
}

/**
 * @brief trims the right side of a string given an array of characters to check against
 *
 * @param src the string to trim
 * @param trim_array a string of characters to include in the trim (order is irrelevant)
 *
 * @return 1 on success, or -1 if errors occur
 */
int headr_string_trim_right(char* src, char*trim_array) {
	// check if null
	if(!src) return -1;
	if(!trim_array) return -1;

	// iter through string until character not in trim_array
	size_t iter = strlen(src);
	while (iter > 0) {
		if(strchr(trim_array, src[iter]) == NULL) break;
		iter--;
	}

	// nothing to trim
	if(iter == strlen(src)) return 1;

	memmove(src, src, iter + 1);
	src[iter + 1] = '\0';
	return 1;
}

/**
 * @brief trims both sides of a string given an array of characters to check against
 *
 * @param src the string to trim
 * @param trim_array a string of characters to include in the trim (order is irrelevant)
 *
 * @return 1 on success, or -1 if errors occur
 */
int headr_string_trim(char* src, char* trim_array) {
	// check if null
	if(!src) return -1;
	if(!trim_array) return -1;

	headr_string_trim_left(src, trim_array);
	headr_string_trim_right(src, trim_array);

	return 1;
}

/**
 * @brief creates a new string slice on the heap based on given indices
 *
 * @param src the string to slice
 * @param start the leftmost index in the sliced string
 * @param end the rightmost index in the sliced string
 *
 * @return a slice of the source string on the heap, or NULL if errors occur
 */
char* headr_string_slice(const char* src, size_t start, size_t end) {
	// check if null
	if(!src) return NULL;

	const size_t src_len = strlen(src);

	// check for valid indices
	if(start > src_len || start < 0) return NULL;
	if(end > src_len || end < 0) return NULL;
	if(end < start) return NULL;
	
	char* slice = (char*)malloc(end - start + 1);
	if(!slice) return NULL;

	memcpy(slice, src + start, end - start);
	slice[end - start] = '\0';
	return slice;
}

/**
 * @brief constructs a list of strings on the heap based on a delimiter
 *
 * @param src the string to be split
 * @param delmimiter the character to split the string by
 * @param string_list_size an overwritten variable with the size of the output list of strings
 *
 * @return a list of strings on the heap teriminated by a NULL pointer, or NULL if errors occur
 */
char** headr_string_split(const char* src, char delimiter, size_t* string_list_size) {
	// check if null
	if(!src) return NULL;
	
	// get number of tokens
	const size_t src_len = strlen(src);
	size_t token_count = 0;
	for(int i = 0; i < src_len; i++)
		if(src[i] == delimiter) 
			token_count++;

	char** token_list = (char**)malloc(sizeof(char*) * (token_count + 1));
	if(!token_list) return NULL;

	size_t last_delimiter_idx = 0;
	*string_list_size = 0;
	for(int i = 0; i < src_len; i++) {
		if(src[i] == delimiter) {
			char* token = headr_string_slice(src, last_delimiter_idx, i);
			// free entire list
			if(!token) {
				for (int j = 0; j < *string_list_size; j++) {
					free(token_list[j]);
				}
				free(token_list);
				*string_list_size = 0;
				return NULL;
			}
			token_list[*string_list_size] = token;
			last_delimiter_idx = i + 1;
			(*string_list_size)++;
		}
	}
	token_list[*string_list_size] = NULL;
	return token_list;
}


#endif

