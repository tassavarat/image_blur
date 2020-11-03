#ifndef BLUR_H
#define BLUR_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

#define NUM_THREADS get_nprocs()

/**
 * struct pixel_s - RGB pixel
 *
 * @r: red component
 * @g: green component
 * @b: blue component
 */
typedef struct pixel_s
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel_t;

/**
 * struct img_s - Image
 *
 * @w:      image width
 * @h:      image height
 * @pixels: array of pixels
 */
typedef struct img_s
{
	size_t w;
	size_t h;
	pixel_t **pixels;
} img_t;

/**
 * struct kernel_s - convolution kernel
 *
 * @size:   size of the matrix (both width and height)
 * @matrix: kernel matrix
 */
typedef struct kernel_s
{
	size_t size;
	float **matrix;
} kernel_t;

/**
 * struct blur_portion_s - Information needed to blur a portion of an image
 *
 * @img:      source image
 * @img_blur: destination image
 * @x:        x position of the portion in the image
 * @y:        y position of the portion in the image
 * @w:        width of the portion
 * @h:        height of the portion
 * @kernel:   convolution kernel to use
 */
typedef struct blur_portion_s
{
	const img_t *img;
	img_t *img_blur;
	size_t x;
	size_t y;
	size_t w;
	size_t h;
	const kernel_t *kernel;
} blur_portion_t;

/**
 * struct tinfo_s - argument for thread_start()
 * @tid:	id returned by pthread_create()
 * @tnum:	application-defined thread number
 * @portion:	pointer to blur_portion_s struct
 * @pixels:	pointer to 2-D array representation of image pixels
 */
typedef struct tinfo_s
{
	pthread_t tid;
	int tnum;
	blur_portion_t *portion;
	pixel_t **pixels;
} tinfo_t;

int blur_img(img_t *img_blur, const img_t *img, const kernel_t *kernel);

#endif /* BLUR_H */
