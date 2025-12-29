#include <stdio.h>
#include <gio/gio.h>
#include <libexif/exif-data.h>

#include "parser.h"
#include "img.h"

/**
 * gly_parse_image:
 * @path: the image path
 * Returns: 0 if all okay, <0 if error
 *
 * Get the image metadata
 *
 * TODO:
 * - Handle error status with GError
 * - Return the ExifData
 */
int
gly_parse_image(Gallery *gly, const char *path)
{
    Img *img = gly_img_new (path);
    GList *imgs = NULL;

    if (img->exif == NULL) {
        gly_img_free (img);
        return -1;
    }

    imgs = g_list_first (g_hash_table_lookup (gly->buckets, &img->key));
    if (!imgs) {
        imgs = g_list_insert_sorted (NULL, img, (GCompareFunc)gly_img_cmp);
        g_hash_table_insert (gly->buckets, &img->key, imgs);
    } else {
        imgs = g_list_insert_sorted (imgs, img, (GCompareFunc)gly_img_cmp);
    }

    return 0;
}

/**
 * gly_parse_directory:
 * @gly: The #Gallery struct
 * @dir: a directory path
 * Returns: 0 if all okay, <0 if error
 *
 * Iterates over all the @dir tree, get each image information and stores in
 * the images gallery
 *
 * TODO:
 * - use GError to store error information
 */
int
gly_parse_directory(Gallery *gly, const char *dir)
{
    g_autoptr(GFileEnumerator) files = NULL;
    g_autoptr(GError) error = NULL;
    g_autoptr(GFile) path = g_file_new_for_path (dir);

    if (!g_file_query_exists (path, NULL))
        return -1;

    files = g_file_enumerate_children (path, "", G_FILE_QUERY_INFO_NONE, NULL, &error);
    while (TRUE) {
        g_autofree char *file_path = NULL;
        GFileInfo *info = NULL;
        if (!g_file_enumerator_iterate (files, &info, NULL, NULL, &error))
            break;
        if (!info)
            break;

        file_path = g_strdup_printf ("%s/%s", dir, g_file_info_get_name (info));

        if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY) {
            gly_parse_directory (gly, file_path);
            continue;
        }
        if (g_file_info_get_file_type (info) != G_FILE_TYPE_REGULAR)
            continue;

        gly_parse_image (gly, file_path);
    }

    return 0;
}
