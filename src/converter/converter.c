#include "converter_lib.h"
#include "negative.c"

int main(int argc, char **argv) {
    enum ERROR type_of_error = NO_ERROR;
    int mine_count = 0;
    int theirs_count = 0;
    if (argc != 4) {
        fprintf(stdout, "Use %s --mine (or --theirs) <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }
    if (strncmp(argv[1], "--mine", MY_IMPLEMENTATION_LENGTH) == 0) {
        mine_count += 1;
    } else if (strncmp(argv[1], "--theirs", THEIRS_IMPLEMENTATION_LENGTH) == 0) {
        theirs_count += 1;
    }
    if (mine_count != 1 && theirs_count != 1) {
        fprintf(stdout, "Use %s --mine (or --theirs) <input file>.bmp <output file>.bmp\n", argv[0]);
        return -1;
    }
    if (mine_count == 1) {
        if (mine_implementation(argv[2], argv[3], &type_of_error)) {
            if (type_of_error == FORMAT) {
                fprintf(stdout, "Wrong format! The converter only supports BMP format.");
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
                        "Error color table! According to the metadata, the number of colors value is 0, but 8 bits per pixel images always have a color table.");
                return -2;
            } else if (type_of_error == TABLE_COLOR_ERROR2) {
                fprintf(stdout,
                        "Error color table! According to the metadata, the number of colors value over 256, but 8 bits per pixel images always use no more 256 colors in the table.");
                return -2;
            } else if (type_of_error == TABLE_COLOR_ERROR3) {
                fprintf(stdout,
                        "Error color table! According to the metadata, the number of colors value is not 0, but 24 bits per pixel images never have a color table.");
                return -2;
            } else if (type_of_error == TABLE_COLOR_ERROR4) {
                fprintf(stdout,
                        "Error color table! According to the metadata, the number of important colors value over 256, but 8 bits per pixel images always use no more 256 of important colors in the table.");
                return -2;
            } else {
                return -1;
            }
        }
    } else if (theirs_count == 1) {
        if (theirs_implementation(argv) != 0) {
            return -3;
        }
    }
    return 0;
}
