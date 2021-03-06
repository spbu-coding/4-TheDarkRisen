#include "comparer_lib.h"

int main(int argc, char **argv) {
    enum ERROR type_of_error = NO_ERROR;
    if (argc != 3) {
        fprintf(stdout, "Use %s <file1_name>.bmp <file2_name>.bmp\n", argv[0]);
        return -1;
    }
    if (do_comparer(argv[1], argv[2], &type_of_error)) {
        if (type_of_error == FORMAT) {
            fprintf(stdout, "Wrong format! The converter ONLY supports BMP format.");
            return -1;
        } else if (type_of_error == SIZE) {
            fprintf(stdout, "Wrong size file! The file size in the metadata does not match the actual.");
            return -2;
        } else if (type_of_error == RESERVED) {
            fprintf(stdout, "Error reserved! Reserved bytes must contain 0.");
            return -2;
        } else if (type_of_error == VERSION) {
            fprintf(stdout, "Wrong version! The converter only supports BMP third versions.");
            return -2;
        } else if (type_of_error == WIDTH) {
            fprintf(stdout, "Error width! Width is always a positive number.");
            return -2;
        } else if (type_of_error == HEIGHT) {
            fprintf(stdout, "Error height! Height cannot be 0.");
            return -2;
        } else if (type_of_error == PLANES) {
            fprintf(stdout, "Error planes! The number of planes must be 1.");
            return -2;
        } else if (type_of_error == BITS_PER_PIXEL) {
            fprintf(stdout,
                    "Error bits per pixel error! The converter only supports 8 and 24 bits per pixel images.");
            return -2;
        } else if (type_of_error == COMPRESSION) {
            fprintf(stdout, "Error compression! Only uncompressed images are supported");
            return -2;
        } else if (type_of_error == PIXELS) {
            fprintf(stdout,
                    "Error size pixels! The size of the pixel data recorded in the metadata does not match the actual size.");
            return -2;
        } else if (type_of_error == TABLE_COLOR_ERROR1) {
            fprintf(stdout,
                    "Error color table! According to the metadata, the number of color value is 0, but 8 bits per pixel images always have a color table.");
            return -2;
        } else if (type_of_error == TABLE_COLOR_ERROR2) {
            fprintf(stdout,
                    "Error color table! According to the metadata, the number of color value over 256, but 8 bits per pixel images always use no more 256 colors in the table.");
            return -2;
        } else if (type_of_error == TABLE_COLOR_ERROR3) {
            fprintf(stdout,
                    "Error color table! According to the metadata, the number of color value is not 0, but 24 bits per pixel images never have a color table.");
            return -2;
        } else if (type_of_error == TABLE_COLOR_ERROR4) {
            fprintf(stdout,
                    "Error color table! According to the metadata, the number of important colors value over 256, but 8 bits per pixel images always use no more 256 of important colors in the table.");
            return -2;
        } else if (type_of_error == TABLE_COLOR_ERROR5) {
            fprintf(stdout,
                    "Wrong color table! Color tables do not match.");
            return -2;
        } else {
            return -1;
        }
    }
    return 0;
}

