#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <string.h>
#include <jpeglib.h>

#define WP_ERROR 1 // General error
#define WP_JPG_FILE_TYPE 2
#define WP_PNG_FILE_TYPE 3
#define WP_UNKNOWN_FILE 4
#define WP_DEBUG 9 // TODO switch to -9 for releases

int detect_file_type(const char *filepath);
void print_file_type(int file_type);

png_structp png_ptr;
png_infop info_ptr;
png_structp png_write_ptr;
png_infop info_write_ptr;

// PNG needs 8 bytes to detect
const unsigned char WP_PNG_MAGIC[8] =
{
    0x89, // Special marker (non ASCII)
    'P',
    'N',
    'G',
    0x0D, // End of line
    0x0A, // New line
    0x1A, // Control char
    0x0A  // New line
};
// JPG needs 2 bytes to detect
const unsigned char WP_JPEG_MAGIC[2] =
{
    0xFF, // Start of img marker
    0xD8  // Continued img marker
};

int main(int argc, char *argv[]) {
    if (argc < 3)
    {
        printf("Error: Required args missing.\n");
        printf("Usage: webpix <source> <destination>\n");
        return WP_ERROR;
    }

    char *source = argv[1];
    char *destination = argv[2];

    printf("Source: %s \n", source);
    printf("Destination: %s\n", destination);

    char *current_file = malloc(100 * sizeof(char));


    strcat(source, "/ocean.png");

    if (WP_DEBUG) printf("Current file source: %s\n", source);

    // read the first 8 bytes
    int file_type = detect_file_type(source);
    if (file_type == WP_ERROR)
    {
        printf("Error reading file: %s\n",source);
    }
    else
    {
        if (WP_DEBUG) print_file_type(file_type);
        // handle
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        info_ptr = png_create_info_struct(png_ptr);

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            printf("Error: reading file -> %s\n", source);
            goto clean;
        }

        FILE *file = fopen(source, "rb");
        if (!file)
        {
            perror("Error opening source file");
            goto clean;
        }
        png_init_io(png_ptr, file);
        png_read_info(png_ptr, info_ptr);

        int width = png_get_image_width(png_ptr, info_ptr);
        int height = png_get_image_height(png_ptr, info_ptr);
        png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);

        for (int y = 0; y < height; y++)
        {
            row_pointers[y] = malloc(png_get_rowbytes(png_ptr, info_ptr));
        }

        png_read_image(png_ptr, row_pointers);

        // Compress

        png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        info_write_ptr = png_create_info_struct(png_write_ptr);

        if (setjmp(png_jmpbuf(png_write_ptr)))
        {
            printf("Error: libpng issued an error when allocating write memory\n");
            goto clean;
        }

        png_set_compression_level(png_write_ptr, 9);

        FILE *dest_fp = fopen(destination, "wb");
        if (!dest_fp)
        {
            perror("Error opening destination file");
            goto clean;
        }

        png_init_io(png_write_ptr, dest_fp);
        png_set_IHDR(
            png_write_ptr,
            info_write_ptr,
            width,
            height,
            png_get_bit_depth(png_ptr, info_ptr),
            png_get_color_type(png_ptr, info_ptr),
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );

        png_write_info(png_write_ptr, info_write_ptr);
        png_write_image(png_write_ptr, row_pointers);
        png_write_end(png_write_ptr, NULL);
        fclose(dest_fp);

        for (int y = 0; y < height; y++)
        {
            free(row_pointers[y]);
        }
        free(row_pointers);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    png_destroy_write_struct(&png_write_ptr, &info_write_ptr);

clean:
    if (WP_DEBUG) printf("Cleaning & exiting...\n");
    free(current_file);

    return 0;
}

/**
 * Detects whether the file type is a JPG or PNG file.
 * @param filepath 
 * @return 
 */
int detect_file_type(const char *filepath)
{
    unsigned char buffer[8];
    FILE *file = fopen(filepath, "rb");

    if (!file)
    {
        perror("Failed to open file");
        return WP_ERROR;
    }

    size_t bytes_read = fread(buffer, 1, 8, file);
    fclose(file);

    if (bytes_read < 8)
    {
        return WP_UNKNOWN_FILE;
    }

    if (memcmp(buffer, WP_PNG_MAGIC, 8) == 0)
    {
        return WP_PNG_FILE_TYPE;
    }
    if (buffer[0] == WP_JPEG_MAGIC[0] && buffer[1] == WP_JPEG_MAGIC[1])
    {
        return WP_JPG_FILE_TYPE;
    }
    // TODO check the suffix
    return WP_UNKNOWN_FILE;
}

void print_file_type(int file_type)
{
    switch (file_type)
    {
        case WP_JPG_FILE_TYPE: printf("File type: JPG\n"); break;
        case WP_PNG_FILE_TYPE: printf("File type: PNG\n"); break;
        case WP_UNKNOWN_FILE: printf("File type: Unknown\n"); break;
        default: printf("File type: Unknown\n");
    }
}
