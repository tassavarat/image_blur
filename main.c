#include "blur.h"

/**
 * write_img - write an image into a PBM file
 * @img: pointer to image structure to write into file
 * @file: path to file to write
 *
 * Return: 0 on success, 1 on failure
 */
int write_img(const img_t *img, const char *file)
{
	FILE *fp;
	size_t i, j;

	fp = fopen(file, "w");
	if (!fp)
		return 1;
	fprintf(fp, "P6\n %lu %lu 255\n", img->w, img->h);
	for (i = 0; i < img->h; ++i)
		for (j = 0; j < img->w; ++j)
			fprintf(fp, "%c%c%c", img->pixels[i][j].r, img->pixels[i][j].g,
				img->pixels[i][j].b);
	fclose(fp);
	return 0;
}

/**
 * load_kernel - load convolution kernel from file
 * @kernel: pointer to kernel structure to initialise
 * @file: path to kernel file
 *
 * Return: 0 on success, 1 on failure
 */
int load_kernel(kernel_t *kernel, const char *file)
{
	FILE *fp;
	size_t i, j;

	fp = fopen(file, "r");
	if (!fp)
		return 1;
	fscanf(fp, "%lu\n", &kernel->size);
	kernel->matrix = malloc(kernel->size * sizeof(*kernel->matrix));
	if (!kernel->matrix)
		return 1;
	for (i = 0; i < kernel->size; i++)
	{
		kernel->matrix[i] = malloc(kernel->size * sizeof(**kernel->matrix));
		for (j = 0; j < kernel->size; j++)
			fscanf(fp, "%f", &kernel->matrix[i][j]);
	}
	fclose(fp);
	return 0;
}

/**
 * cpy_img - an image structure
 * @dest: pointer to image structure to initialise
 * @src: pointer to image structure to copy
 *
 * Return: 0 on success, 1 on failure
 */
int cpy_img(img_t *dest, img_t *src)
{
	size_t i, j;

	dest->w = src->w, dest->h = src->h;
	dest->pixels = malloc(dest->h * sizeof(*dest->pixels));
	if (!dest->pixels)
		return 1;
	for (i = 0; i < dest->h; ++i) {
		dest->pixels[i] = malloc(dest->w * sizeof(**dest->pixels));
		if (!dest->pixels[i])
			return 1;
		for (j = 0; j < dest->w; ++j)
			dest->pixels[i][j] = src->pixels[i][j];
	}
	return 0;
}

/**
 * load_img - load an image from a PBM file
 * @img: pointer to image structure to initialise
 * @file: path to image file
 *
 * Return: 0 on success, 1 on failure
 */
int load_img(img_t *img, const char *file)
{
	FILE *fp;
	size_t i, j;

	fp = fopen(file, "r");
	if (!fp)
		return 1;
	fscanf(fp, "P6\n %lu %lu 255\n", &img->w, &img->h);
	img->pixels = malloc(img->h * sizeof(*img->pixels));
	if (!img->pixels)
		return 1;
	for (i = 0; i < img->h; ++i) {
		img->pixels[i] = malloc(img->w * sizeof(**img->pixels));
		if (!img->pixels[i])
			return 1;
		for (j = 0; j < img->w; ++j)
			fscanf(fp, "%c%c%c", &img->pixels[i][j].r, &img->pixels[i][j].g,
				&img->pixels[i][j].b);
	}
	fclose(fp);
	return 0;
}

/**
 * main - entry point
 * @argv: number of command-line arguments
 * @argv: command-line arguments containing image and kernel locations
 *
 * Return: 0 on success, 1 on failure
 */
int main(int argc, char *argv[])
{
	img_t img, img_blur;
	kernel_t kernel;
	size_t i;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s image.ppm kernel.knl\n", *argv);
		return 1;
	}
	if (load_img(&img, argv[1]) || cpy_img(&img_blur, &img) || load_kernel(&kernel, argv[2]) ||
			blur_img(&img_blur, &img, &kernel) || write_img(&img_blur, "output.pbm"))
		return 1;
	for (i = 0; i < img.h; ++i)
		free(img.pixels[i]), free(img_blur.pixels[i]);
	free(img.pixels), free(img_blur.pixels);
	for (i = 0; i < kernel.size; ++i)
		free(kernel.matrix[i]);
	free(kernel.matrix);
	return 0;
}
