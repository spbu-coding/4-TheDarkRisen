#include "comparer_lib.h"

long long int file_size(FILE *file) {
    long long int size_file = 0;
    fseek(file, 0, SEEK_END);
    size_file = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size_file;
}

IMAGE_BITMAP_INFORMATION *decoding_image_bitmap_info(FILE *file, enum ERROR *type_of_error) {
    long long int size_file = file_size(file);
    IMAGE_BITMAP_INFORMATION *image_bitmap_info = malloc(sizeof(IMAGE_BITMAP_INFORMATION));
    if (!image_bitmap_info) {
        fprintf(stdout, "Can't allocate memory for results string in all experiment\n");
        return NULL;
    }
    image_bitmap_info->format = 0;
    image_bitmap_info->size_file = 0;
    image_bitmap_info->reserved1 = 0;
    image_bitmap_info->reserved2 = 0;
    image_bitmap_info->pixel_array_off_set = 0;
    image_bitmap_info->version_header = 0;
    image_bitmap_info->width = 0;
    image_bitmap_info->height = 0;
    image_bitmap_info->planes = 0;
    image_bitmap_info->bits_per_pixel = 0;
    image_bitmap_info->compression = 0;
    image_bitmap_info->size_pixels_file = 0;
    fread(&image_bitmap_info->format, sizeof(image_bitmap_info->format), 1, file);
    if (image_bitmap_info->format != 0x4D42) {
        *type_of_error = FORMAT;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->size_file, sizeof(image_bitmap_info->size_file), 1, file);
    if (size_file != image_bitmap_info->size_file) {
        *type_of_error = SIZE;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->reserved1, sizeof(image_bitmap_info->reserved1), 1, file);
    fread(&image_bitmap_info->reserved2, sizeof(image_bitmap_info->reserved2), 1, file);
    if (image_bitmap_info->reserved1 != 0 || image_bitmap_info->reserved2 != 0) {
        *type_of_error = RESERVED;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->pixel_array_off_set, sizeof(image_bitmap_info->pixel_array_off_set), 1,
          file);
    fread(&image_bitmap_info->version_header, sizeof(image_bitmap_info->version_header), 1, file);
    if (image_bitmap_info->version_header != 40) {
        *type_of_error = VERSION;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->width, sizeof(image_bitmap_info->width), 1, file);
    if (image_bitmap_info->width <= 0) {
        *type_of_error = WIDTH;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->height, sizeof(image_bitmap_info->height), 1, file);
    if (image_bitmap_info->height == 0) {
        *type_of_error = HEIGHT;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->planes, sizeof(image_bitmap_info->planes), 1, file);
    if (image_bitmap_info->planes != 1) {
        *type_of_error = PLANES;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->bits_per_pixel, sizeof(image_bitmap_info->bits_per_pixel), 1, file);
    if ((image_bitmap_info->bits_per_pixel != 8) && (image_bitmap_info->bits_per_pixel != 24)) {
        *type_of_error = BITS_PER_PIXEL;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->compression, sizeof(image_bitmap_info->compression), 1, file);
    if (image_bitmap_info->compression != 0) {
        *type_of_error = COMPRESSION;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->size_pixels_file, sizeof(image_bitmap_info->size_pixels_file), 1, file);
    if (image_bitmap_info->size_pixels_file != 0) {
        int size_only_pixels = image_bitmap_info->size_file - image_bitmap_info->pixel_array_off_set;
        if (image_bitmap_info->size_pixels_file != size_only_pixels) {
            *type_of_error = PIXELS;
            free(image_bitmap_info);
            return NULL;
        }
    }
    fread(&image_bitmap_info->hor_res_image, sizeof(image_bitmap_info->hor_res_image), 1, file);
    fread(&image_bitmap_info->ver_res_image, sizeof(image_bitmap_info->ver_res_image), 1, file);
    fread(&image_bitmap_info->number_of_colors, sizeof(image_bitmap_info->number_of_colors), 1, file);
    if (image_bitmap_info->number_of_colors == 0 && image_bitmap_info->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR1;
        free(image_bitmap_info);
        return NULL;
    }
    if (image_bitmap_info->number_of_colors > 256 && image_bitmap_info->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR2;
        free(image_bitmap_info);
        return NULL;
    }
    if (image_bitmap_info->number_of_colors != 0 && image_bitmap_info->bits_per_pixel == 24) {
        *type_of_error = TABLE_COLOR_ERROR3;
        free(image_bitmap_info);
        return NULL;
    }
    fread(&image_bitmap_info->number_of_important_colors,
          sizeof(image_bitmap_info->number_of_important_colors), 1, file);
    if (image_bitmap_info->number_of_important_colors > 256 && image_bitmap_info->bits_per_pixel == 8) {
        *type_of_error = TABLE_COLOR_ERROR4;
        free(image_bitmap_info);
        return NULL;
    }

    return image_bitmap_info;
}

int **decoding_image_pixels(FILE *file, int width, int height, int bits_per_pixel) {
    int bytes_per_pixel = bits_per_pixel / 8;
    height = abs(height);
    int **array_image_pixels = malloc(sizeof(int *) * height);
    if (!array_image_pixels) {
        fprintf(stdout, "Can't allocate memory for results string in all experiment\n");
        return 0;
    }
    for (long long int i = 0; i < height; i++) {
        array_image_pixels[i] = malloc(sizeof(int) * width);
        if (!array_image_pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(array_image_pixels[i]);
            }
            free(array_image_pixels);
            return 0;
        }
    }
    for (long long int i = 0; i < height; i++) {
        for (long long int j = 0; j < width; j++) {
            array_image_pixels[i][j] = 0;
            fread(&array_image_pixels[i][j], bytes_per_pixel, 1, file);
        }
    }
    return array_image_pixels;
}

int *decoding_image_color_table(FILE *file, int number_of_colors) {
    int size_table_colors = number_of_colors * BYTES_COLOR_TABLE;
    int *array_table_colors = malloc(sizeof(int) * size_table_colors);
    if (!array_table_colors) {
        fprintf(stdout, "Can't allocate memory for results string in all experiment\n");
        return 0;
    }
    for (long long int i = 0; i < number_of_colors; i++) {
        array_table_colors[i] = 0;
        fread(&array_table_colors[i], BYTES_COLOR_TABLE, 1, file);
    }
    return array_table_colors;
}

IMAGE *decoding_image(FILE *file, enum ERROR *type_of_error) {
    IMAGE *image = calloc(1, sizeof(IMAGE));
    if (!image) {
        fprintf(stdout, "Can't allocate memory for results string in all experiment\n");
        return NULL;
    }
    image->meta_data_header = decoding_image_bitmap_info(file, type_of_error);
    if (!image->meta_data_header) {
        free(image);
        return NULL;
    }
    if (image->meta_data_header->number_of_colors != 0 && image->meta_data_header->bits_per_pixel == 8) {
        image->color_table = decoding_image_color_table(file, image->meta_data_header->number_of_colors);
        if (!image->color_table) {
            free(image);
            return NULL;
        }
    }
    image->array_of_pixels = decoding_image_pixels(file, image->meta_data_header->width,
                                                   image->meta_data_header->height,
                                                   image->meta_data_header->bits_per_pixel);

    if (!image->array_of_pixels) {
        free(image);
        return NULL;
    }
    return image;
}

void free_image(IMAGE *image) {
    for (long long int i = 0; i < abs(image->meta_data_header->height); i++) {
        free(image->array_of_pixels[i]);
    }
    free(image->array_of_pixels);
    free(image->meta_data_header);
    free(image->color_table);
    free(image);
    image = NULL;
}

void comparer_pixels(IMAGE *image, IMAGE *image2) {
    int count = 0;
    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0) {
        for (long long int i = 0; i < image->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[i][j] != image2->array_of_pixels[i][j]) {
                    if (image->meta_data_header->height > 0 && image2->meta_data_header->height > 0) {
                        fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    }
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height < 0 && image2->meta_data_header->height < 0) {
        for (long long int i = 0; i < abs(image->meta_data_header->height); i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height > 0 && image2->meta_data_header->height < 0) {
        for (long long int i = 0; i < image->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[i][j] !=
                                   image2->array_of_pixels[abs(image2->meta_data_header->height) - i - 1][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    } else if (image->meta_data_header->height < 0 && image2->meta_data_header->height > 0) {
        for (long long int i = 0; i < image2->meta_data_header->height; i++) {
            for (long long int j = 0; j < image->meta_data_header->width; j++) {
                if (count < 100 && image->array_of_pixels[abs(image->meta_data_header->height) - i - 1][j] !=
                                   image2->array_of_pixels[i][j]) {
                    fprintf(stderr, "X coordinate: %lld Y coordinate: %lld \n", j, i);
                    count++;
                }
            }
        }
    }
}

int comparer_table(IMAGE *image, IMAGE *image2, enum ERROR *type_of_error) {
    for (long long int i = 0; i < image->meta_data_header->number_of_colors; i++) {
        if (image->color_table[i] != image2->color_table[i]) {
            *type_of_error = TABLE_COLOR_ERROR5;
            return -1;
        }
    }
    return 0;
}

int do_comparer(char *name_file, char *name_file2, enum ERROR *type_of_error) {
    FILE *file;
    FILE *file2;
    if ((file = fopen(name_file, "rb")) == NULL) {
        fprintf(stdout, "Can't open file! No file with name %s exists. ", name_file);
        return -1;
    }
    IMAGE *image = decoding_image(file, type_of_error);
    if (!image) {
        fprintf(stdout, "Error for a file named %s! ", name_file);
        fclose(file);
        return -1;
    }
    if ((file2 = fopen(name_file2, "rb")) == NULL) {
        fprintf(stdout, "Can't open file! No file with name %s exists! ", name_file2);
        return -1;
    }
    IMAGE *image2 = decoding_image(file2, type_of_error);
    if (!image2) {
        fprintf(stdout, "Error for a file named %s! ", name_file2);
        fclose(file2);
        return -1;
    }
    if ((image->meta_data_header->width != image2->meta_data_header->width) ||
        (abs(image->meta_data_header->height) != abs(image2->meta_data_header->height))) {
        fprintf(stdout, "Images have different values of width or height! ");
        free_image(image);
        free_image(image2);
        fclose(file);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->size_file != image2->meta_data_header->size_file)) {
        fprintf(stdout, "Images have different values of sizes! ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->size_pixels_file != image2->meta_data_header->size_pixels_file)) {
        fprintf(stdout, "Images have different values of pixels data size! ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->bits_per_pixel != image2->meta_data_header->bits_per_pixel)) {
        fprintf(stdout, "Images have different values of bits_per_pixel! ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else if ((image->meta_data_header->number_of_colors != image2->meta_data_header->number_of_colors)) {
        fprintf(stdout, "Images have different number of colors in the table! ");
        free_image(image);
        fclose(file);
        free_image(image2);
        fclose(file2);
        return -1;
    } else {
        if (!comparer_table(image, image2, type_of_error)) {
            comparer_pixels(image, image2);
        } else {
            free_image(image);
            fclose(file);
            free_image(image2);
            fclose(file2);
            return -1;
        }
    }
    free_image(image);
    fclose(file);
    free_image(image2);
    fclose(file2);
    return 0;
}