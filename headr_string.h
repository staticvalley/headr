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

int headr_string_trim_left(char* src, char* trim_array) {
	// check if null
	if(!src) return -1;

	// iter through string until character not in trim_array
	size_t iter = 0;
	size_t src_len = strlen(src);
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

int headr_string_trim_right(char* src, char*trim_array) {
	// check if null
	if(!src) return -1;

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

int headr_string_trim(char* src, char* trim_array) {
	// check if null
	if(!src) return -1;

	headr_string_trim_left(src, trim_array);
	headr_string_trim_right(src, trim_array);

	return 1;
}

char* headr_string_slice(const char* src, size_t start, size_t end) {
	size_t src_len = strlen(src);

	// check for valid indices
	if(start > src_len || start < 0) return NULL;
	if(end > src_len || end < 0) return NULL;
	if(end < start) return NULL;
	
	char* slice = (char*)malloc(end - start + 1);
	if(slice == NULL) { return NULL; }

	memcpy(slice, src + start, end - start);
	slice[end - start] = '\0';
	return slice;
}

//char* headr_string_join(


#endif

