#include "blur.h"

/**
 * get_sums - initialise sums for rgb and kernel
 * @r_sum:	pointer to sum of red values
 * @g_sum:	pointer to sum of green values
 * @b_sum:	pointer to sum of blue values
 * @k_sum:	pointer to sum of kernel values
 * @portion:	pointer to structure with information needed to blur
 * @pixels:	double array of pixels
 * @x:		x position for pixels array of pixel to blur
 * @y:		y position for pixels array of pixel to blur
 */
void get_sums(float *r_sum, float *g_sum, float *b_sum, float *k_sum,
		const blur_portion_t *portion, const pixel_t **pixels, const size_t x,
		const size_t y)
{
	ssize_t grid_x, grid_y, grid_stop_x, grid_stop_y;
	size_t half_kernel, k_x, k_y;

	half_kernel = portion->kernel->size / 2;
	grid_x = x - half_kernel, grid_y = y - half_kernel;
	grid_stop_y = grid_y + portion->kernel->size;
	grid_stop_x = grid_x + portion->kernel->size;
	for (grid_y = y - half_kernel, k_y = *r_sum = *g_sum = *b_sum = *k_sum = 0;
			grid_y < grid_stop_y; ++grid_y, ++k_y) {
		for (grid_x = x - half_kernel, k_x = 0; grid_x < grid_stop_x; ++grid_x, ++k_x) {
			if (grid_x > -1 && grid_y > -1 && grid_x < (ssize_t) portion->img->w &&
					grid_y < (ssize_t) portion->img->h) {
				*r_sum += portion->kernel->matrix[k_y][k_x] *
					pixels[grid_y][grid_x].r;
				*g_sum += portion->kernel->matrix[k_y][k_x] *
					pixels[grid_y][grid_x].g;
				*b_sum += portion->kernel->matrix[k_y][k_x] *
					pixels[grid_y][grid_x].b;
				*k_sum += portion->kernel->matrix[k_y][k_x];
			}
		}
	}
}

/**
 * blur_pixel - blur individual pixel
 * @portion:	pointer to structure with information needed to blur
 * @pixels:	double array of pixels
 * @x:		x position for pixels array of pixel to blur
 * @y:		y position for pixels array of pixel to blur
 */
void blur_pixel(const blur_portion_t *portion, const pixel_t **pixels, const size_t x,
		const size_t y)
{
	float r_avg, g_avg, b_avg, k_sum;

	get_sums(&r_avg, &g_avg, &b_avg, &k_sum, portion, pixels, x, y);
	r_avg /= k_sum, g_avg /= k_sum, b_avg /= k_sum;
	portion->img_blur->pixels[y][x].r = r_avg;
	portion->img_blur->pixels[y][x].g = g_avg;
	portion->img_blur->pixels[y][x].b = b_avg;
}

/**
 * blur_portion_thread - blur portion of an image using Gaussian blur
 * @tinfo: pointer to structure with pointers to portion and pixels
 *
 * Each thread will blur its portion of the image. The constant r will shift threads to any remaining
 * x-coordinate dimensions when the image's horizontal pixels / NUM_THREADS does not produce a whole
 * number.
 */
void blur_portion_thread(tinfo_t *tinfo)
{
	size_t i, j;
	const size_t r = NUM_THREADS * tinfo->portion->w;
	blur_portion_t *portion = tinfo->portion;

	if (!portion)
		return;
	for (i = portion->x; i < portion->w + portion->x; ++i)
		for (j = portion->y; j < portion->h + portion->y; ++j) {
			blur_pixel(portion, (const pixel_t **)tinfo->pixels, i, j);
			if (i + r < portion->img->w)
				blur_pixel(portion, (const pixel_t **)tinfo->pixels, i + r, j);
		}
}

/**
 * thread_start - entry point for thread
 * @arg: pointer to struct containing thread and image blur information
 *
 * Return: NULL
 */
void *thread_start(void *arg)
{
	blur_portion_thread((tinfo_t *)arg);
	pthread_exit(NULL);
}

/**
 * init - initialise dynamic memory
 * @tinfo:	double pointer to argument for thread_start()
 * @portion:	double pointer to structure with information needed to blur
 * @img:	pointer to source image
 * @img_blur:	pointer to destination image
 * @kernel:	pointer to convolution kernel
 *
 * Return: 0 on success or 1 on malloc fail
 */
int init(tinfo_t **tinfo, blur_portion_t **portion, img_t const *img, img_t *img_blur,
		kernel_t const *kernel)
{
	int i;
	size_t offset;

	*tinfo = malloc(NUM_THREADS * sizeof(**tinfo));
	if (!*tinfo)
		return 1;
	*portion = malloc(NUM_THREADS * sizeof(**portion));
	if (!*portion)
		return 1;
	for (i = 0; i < NUM_THREADS; ++i) {
		(*portion)[i].img = img, (*portion)[i].img_blur = img_blur;
		(*portion)[i].kernel = kernel;
		if ((*portion)[i].img->w <= (size_t) NUM_THREADS &&
			(size_t) i < (*portion)[i].img->w) {
			(*portion)[i].x = i;
			(*portion)[i].y = 0;
			(*portion)[i].w = 1;
			(*portion)[i].h = (*portion)[i].img->h;
		} else {
			offset = (*portion)[i].img->w / (NUM_THREADS);
			(*portion)[i].x = offset * i;
			(*portion)[i].y = 0;
			(*portion)[i].w = offset;
			(*portion)[i].h = (*portion)[i].img->h;
		}
	}
	return 0;
}

/**
 * blur_image - blur entire image using Gaussian blur
 * @img_blur:	pointer to destination image
 * @img:	pointer to source image
 * @kernel:	pointer to convolution kernel
 *
 * Return: 0 on success, 1 on failure
 */
int blur_img(img_t *img_blur, const img_t *img, const kernel_t *kernel)
{
	tinfo_t *tinfo;
	blur_portion_t *portion;
	int i, s;

	if (!img_blur || !img || !kernel)
		return 1;
	if (init(&tinfo, &portion, img, img_blur, kernel))
		return 1;
	for (i = 0; i < NUM_THREADS; ++i) {
		tinfo[i].tnum = i;
		tinfo[i].portion = &portion[i];
		tinfo[i].pixels = img->pixels;
		s = pthread_create(&tinfo[i].tid, NULL, &thread_start, tinfo + i);
		if (s != 0)
			return 1;
	}
	for (i = 0; i < NUM_THREADS; ++i) {
		s = pthread_join(tinfo[i].tid, NULL);
		if (s != 0)
			return 0;
	}
	free(portion), free(tinfo);
	return 0;
}
