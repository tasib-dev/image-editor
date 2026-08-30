#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"


#pragma pack(push, 1)

typedef struct
{
    uint16_t type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixel_offset;
} BMPFileHeader;


typedef struct
{
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t important_colors;
} BMPInfoHeader;

#pragma pack(pop)


Image *load_bmp(const char *filename)
{
    FILE *fp;
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    Image *image;
    int width;
    int height;
    int padding;

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        return NULL;
    }


    /* Read file header */
    if (fread(&file_header, sizeof(BMPFileHeader), 1, fp) != 1)
    {
        fclose(fp);
        return NULL;
    }


    /* Check BMP signature */
    if (file_header.type != 0x4D42)
    {
        fclose(fp);
        return NULL;
    }


    /* Read info header */
    if (fread(&info_header, sizeof(BMPInfoHeader), 1, fp) != 1)
    {
        fclose(fp);
        return NULL;
    }


    /* We only support the required BMP format */
    if (info_header.header_size < 40)
    {
        fclose(fp);
        return NULL;
    }

    if (info_header.planes != 1)
    {
        fclose(fp);
        return NULL;
    }

    if (info_header.bits_per_pixel != 24)
    {
        fclose(fp);
        return NULL;
    }

    if (info_header.compression != 0)
    {
        fclose(fp);
        return NULL;
    }

    if (info_header.width <= 0 || info_header.height == 0)
    {
        fclose(fp);
        return NULL;
    }


    width = info_header.width;

    /*
     * A positive BMP height means bottom-up.
     * A negative height means top-down.
     */
    if (info_header.height < 0)
    {
        height = -info_header.height;
    }
    else
    {
        height = info_header.height;
    }


    image = create_image(width, height);

    if (image == NULL)
    {
        fclose(fp);
        return NULL;
    }


    /*
     * Each BMP row must be a multiple of 4 bytes.
     *
     * 24-bit BMP uses 3 bytes per pixel.
     */
    padding = (4 - (width * 3) % 4) % 4;


    if (fseek(fp, file_header.pixel_offset, SEEK_SET) != 0)
    {
        destroy_image(image);
        fclose(fp);
        return NULL;
    }


    for (int file_y = 0; file_y < height; file_y++)
    {
        int image_y;

        /*
         * Positive height = bottom-up.
         * Negative height = top-down.
         */
        if (info_header.height > 0)
        {
            image_y = height - 1 - file_y;
        }
        else
        {
            image_y = file_y;
        }


        for (int x = 0; x < width; x++)
        {
            uint8_t blue;
            uint8_t green;
            uint8_t red;

            if (fread(&blue, sizeof(uint8_t), 1, fp) != 1 ||
                fread(&green, sizeof(uint8_t), 1, fp) != 1 ||
                fread(&red, sizeof(uint8_t), 1, fp) != 1)
            {
                destroy_image(image);
                fclose(fp);
                return NULL;
            }

            image->data[image_y * width + x].red = red;
            image->data[image_y * width + x].green = green;
            image->data[image_y * width + x].blue = blue;
        }


        /* Skip row padding */
        if (fseek(fp, padding, SEEK_CUR) != 0)
        {
            destroy_image(image);
            fclose(fp);
            return NULL;
        }
    }


    fclose(fp);

    return image;
}


int save_bmp(const char *filename, const Image *image)
{
    FILE *fp;
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    int padding;
    uint8_t zero = 0;

    if (image == NULL || image->data == NULL)
    {
        return 0;
    }


    fp = fopen(filename, "wb");

    if (fp == NULL)
    {
        return 0;
    }


    padding = (4 - (image->width * 3) % 4) % 4;


    /* File header */
    file_header.type = 0x4D42;
    file_header.pixel_offset =
        sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    file_header.reserved1 = 0;
    file_header.reserved2 = 0;

    file_header.file_size =
        file_header.pixel_offset +
        (image->width * 3 + padding) * image->height;


    /* Info header */
    info_header.header_size = sizeof(BMPInfoHeader);
    info_header.width = image->width;

    /*
     * Positive height means bottom-up BMP.
     */
    info_header.height = image->height;

    info_header.planes = 1;
    info_header.bits_per_pixel = 24;
    info_header.compression = 0;

    info_header.image_size =
        (image->width * 3 + padding) * image->height;

    info_header.x_pixels_per_meter = 0;
    info_header.y_pixels_per_meter = 0;
    info_header.colors_used = 0;
    info_header.important_colors = 0;


    /* Write headers */
    if (fwrite(&file_header, sizeof(BMPFileHeader), 1, fp) != 1)
    {
        fclose(fp);
        return 0;
    }

    if (fwrite(&info_header, sizeof(BMPInfoHeader), 1, fp) != 1)
    {
        fclose(fp);
        return 0;
    }


    /*
     * BMP stores rows from bottom to top.
     */
    for (int y = image->height - 1; y >= 0; y--)
    {
        for (int x = 0; x < image->width; x++)
        {
            Pixel *pixel =
                &image->data[y * image->width + x];

            /*
             * BMP order is BGR.
             */
            if (fwrite(&pixel->blue, sizeof(uint8_t), 1, fp) != 1 ||
                fwrite(&pixel->green, sizeof(uint8_t), 1, fp) != 1 ||
                fwrite(&pixel->red, sizeof(uint8_t), 1, fp) != 1)
            {
                fclose(fp);
                return 0;
            }
        }


        /* Write padding */
        for (int i = 0; i < padding; i++)
        {
            if (fwrite(&zero, sizeof(uint8_t), 1, fp) != 1)
            {
                fclose(fp);
                return 0;
            }
        }
    }


    fclose(fp);

    return 1;
}