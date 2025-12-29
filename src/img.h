#pragma once

#include <glib.h>
#include <libexif/exif-data.h>

typedef struct {
    char *path;         // Image path
    GDateTime *date;    // The full date in exif data
    int key;            // YYYYMM
    ExifData *exif;     // All exif data
} Img;

Img *gly_img_new (const char *path);
void gly_img_free (Img *img);

const char *gly_img_datetime (Img *img);
int         gly_img_cmp (Img *a, Img *b);
