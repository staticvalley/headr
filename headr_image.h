/***********************************************************
 *
 * file: headr_image.h
 * description: general image writers
 * author: jonathan kocevar (@staticvalley)
 *
 ***********************************************************/

#ifndef HEADR_IMAGE
#define HEADR_IMAGE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HEADR_USE_SHORTNAMES
#define headr_image_init image_init
#define headr_image_write image_write
#define headr_image_convert_format image_convert_format
#endif

/**
 * @brief UNIX file-system permissions constant
 * @note change value to change image permissions upon creation
 */
#define HEADR_IMAGE_WRITABLE_PERMISSIONS 0664

/**
 * @brief supported image format types
 */
enum headr_image_format {
	PPM,
	PGM,
	PBM,
};

/**
 * @brief general image object struct
 */
typedef struct {
	uint8_t* image_buffer;
	size_t width;
	size_t height;
	// metadata
	enum headr_image_format format;
	size_t _image_buffer_len;
} headr_image;

/**
 * @brief generates an image object
 * @param format image format enum, (ie. PPM, BMP, PNG)
 * @param byte_buffer byte array containing image contents
 * @param byte_buffer_len length of byte array containing image contents
 * @param width width of image
 * @param height height of image
 */
headr_image headr_image_init(enum headr_image_format format, uint8_t* byte_buffer, size_t byte_buffer_len, size_t width, size_t height) {
	headr_image new_img = {
		byte_buffer,
		width,
		height,
		format,
		byte_buffer_len,
	};
	return new_img;
}

/**
 * @brief internally writes an image object to the ppm format
 * @param writable image object to write
 * @param filename name of file to write to
 */
int _headr_internal_image_write_ppm(const headr_image writable, const char* filename) {
	
	// check that buffer is in ppm format
	if((writable.width * writable.height * 3) > writable._image_buffer_len) {
		fprintf(stderr, "headr_image.h: line %d: error: writable buffer must be correct shape to write to ppm\n", __LINE__);\
		return -1;
	}

	// open writable file
	int img_fd = open(filename, O_WRONLY | O_CREAT, HEADR_IMAGE_WRITABLE_PERMISSIONS);
	if(img_fd == -1) {
		fprintf(stderr, "headr_image.h: line %d: error: could not create file \"%s\"\n", __LINE__, filename);\
		return -1;
	}

	// write header
	dprintf(img_fd, "P6\n%zu %zu\n255\n", writable.width, writable.height);

	// dump byte buffer
	if(write(img_fd, writable.image_buffer, sizeof(uint8_t) * writable.width * writable.height * 3) == -1) {
		close(img_fd);
		return -1;
	}
	close(img_fd); 

	return 1;
}

/**
 * @brief internally writes an image object to the pgm format
 * @param writable image object to write
 * @param filename name of file to write to
 */
int _headr_internal_image_write_pgm(const headr_image writable, const char* filename) {
	
	// check that buffer is in pgm format
	if((writable.width * writable.height) > writable._image_buffer_len) {
		fprintf(stderr, "headr_image.h: line %d: error: writable buffer must be correct shape to write to pgm\n", __LINE__);\
		return -1;
	}

	// open writable file
	int img_fd = open(filename, O_WRONLY | O_CREAT, HEADR_IMAGE_WRITABLE_PERMISSIONS);
	if(img_fd == -1) {
		fprintf(stderr, "headr_image.h: line %d: error: could not create file \"%s\"\n", __LINE__, filename);\
		return -1;
	}

	// write header
	dprintf(img_fd, "P5\n%zu %zu\n255\n", writable.width, writable.height);

	// dump byte buffer
	if(write(img_fd, writable.image_buffer, sizeof(uint8_t) * writable.width * writable.height) == -1) {
		close(img_fd);
		return -1;
	}
	close(img_fd); 

	return 1;
}

/**
 * @brief internally writes an image object to the pbm format
 * @param writable image object to write
 * @param filename name of file to write to
 * @note performs byte packing,buffers are assumed to be unpacked
 */
int _headr_internal_image_write_pbm(const headr_image writable, const char* filename) {

	// open writable file
	int img_fd = open(filename, O_WRONLY | O_CREAT, HEADR_IMAGE_WRITABLE_PERMISSIONS);
	if(img_fd == -1) {
		fprintf(stderr, "headr_image.h: line %d: error: could not create file \"%s\"\n", __LINE__, filename);\
		return -1;
	}

	// write header
	dprintf(img_fd, "P4\n%zu %zu\n", writable.width, writable.height);

	/*
	 * TODO: look into if an addictional fixed writing buffer would be worth it
	 *	ie. 4096 byte fixed array to push rows to before writing to file
	 *	(Im too lazy for this right now and writing width sized buffers dont sound too bad)
	 */
	size_t row_byte_size = (writable.width / 8) + 1;
	uint8_t *row_buffer = (uint8_t*)malloc(sizeof(uint8_t) * row_byte_size);
	if(row_buffer == NULL) {
		fprintf(stderr, "headr_image.h: line %d: error: malloc failed\n", __LINE__);\
		return -1;
	}

	// pack bytes and write to iamge using internal buffer
	for(size_t i = 0; i < writable.height; i++) {
		memset(row_buffer, 0, sizeof(uint8_t) * row_byte_size);
		for(size_t j = 0; j < writable.width; j ++) {
			size_t byte_position = j / 8;
			size_t packing_position = 7 - (j % 8); // big endian (MSB = most right)
			/*
			 * write all bits that have a LSB of 1 as black
			 * otherwise, ignore (leave white)
			 */
			if(writable.image_buffer[(i*writable.width) + j] & 1)
				row_buffer[byte_position] |= (1 << packing_position);
		}
		write(img_fd, row_buffer, row_byte_size * sizeof(uint8_t));
	}

	close(img_fd); 

	return 1;
}

/**
 * @brief changes format metadata on image object
 * @param writable image object to be converted
 * @param new_format format to convert image object to
 */
int headr_image_convert_format(headr_image* img, enum headr_image_format new_format) {
	img->format = new_format;
	return 1;
}

/**
 * @brief writes an image object to the format specified in image object metadata
 * @param writable image object to write
 * @param filename name of file to write to
 * @note essentially a wrapper for all internal writer types
 */
int headr_image_write(const headr_image writable, const char* filename) {
	switch (writable.format) {
		case PPM:
			return _headr_internal_image_write_ppm(writable, filename);
		case PGM:
			return _headr_internal_image_write_pgm(writable, filename);
		case PBM:
			return _headr_internal_image_write_pbm(writable, filename);
	}
}

#endif

