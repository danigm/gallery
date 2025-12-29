#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include "img.h"

/**
 * Converts a string (YYYY:MM:DD HH:MM:SS) to GDateTime
 */
static GDateTime *
parse_date (const char *date)
{
    int d[6] = {0};

    int i = 0;
    char *next = (char *)date;
    while (i < 6) {
        d[i++] = strtol (next, &next, 10);
        if (next == date)
            break;
        next++;
    }

    return g_date_time_new_local (d[0], d[1], d[2], d[3], d[4], d[5]);
}


Img *
gly_img_new (const char *path)
{
    Img *img = g_malloc0 (sizeof (Img));
    img->path = g_strdup (path);
    img->exif = exif_data_new_from_file (path);
    img->date = NULL;
    img->key = 0;
    if (gly_img_datetime (img) != NULL) {
        int year = 0;
        int month = 0;
        img->date = parse_date (gly_img_datetime (img));
        g_date_time_get_ymd (img->date, &year, &month, NULL);
        img->key = year * 100 + month;
    } else {
        g_warning ("No date found in exif data: %s\n", path);
    }
    return img;
}

void
gly_img_free (Img *img)
{
    g_free (img->path);
    if (img->date)
        g_date_time_unref (img->date);
    if (img->exif)
        exif_data_unref (img->exif);

    g_free (img);
}

/**
 * gly_img_datetime:
 * @img: The #Img
 *
 * Returns: (transfer none): The datetime of the @ima
 */
const char *
gly_img_datetime (Img *img)
{
    ExifEntry *entry = NULL;

    if (!img->exif)
        return NULL;

    entry = exif_data_get_entry (img->exif, EXIF_TAG_DATE_TIME);
    return (const char*)entry->data;
}

/**
 * gly_img_cmp:
 * @a The first image
 * @b The second image
 * Returns: negative if a<b, 0 if a==b, positive if a>b
 *
 * Compares images for sorting, it compares based on the image date
 */
int
gly_img_cmp (Img *a, Img *b)
{
    return g_date_time_compare (a->date, b->date);
}
