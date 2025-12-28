#pragma once

#include <libexif/exif-data.h>

typedef struct {
    char *path;
    ExifData *exif;
} Img;

Img *gly_img_new (const char *path);
void gly_img_free (Img *img);

const char *gly_img_datetime (Img *img);
