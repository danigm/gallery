#include <glib.h>
#include "img.h"

Img *
gly_img_new (const char *path)
{
    Img *img = g_malloc0 (sizeof (Img));
    img->path = g_strdup (path);
    img->exif = exif_data_new_from_file (path);
    return img;
}

void
gly_img_free (Img *img)
{
    g_free (img->path);
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
    // TODO: parse time and create a GDateTime
    return (const char*)entry->data;
}
