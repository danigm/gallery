#include <gio/gio.h>
#include <stdio.h>

#include "parser.h"

/**
 * gly_parse_directory:
 * @dir: a directory path
 * Returns: 0 if all okay, <0 if error
 *
 * Iterates over all the @dir tree, get each image information and stores in
 * the images gallery
 *
 * TODO:
 * - use GError to store error information
 * - pass Gallery struct as first argument to store all information
 */
int
gly_parse_directory(const char *dir)
{
    g_autoptr(GFileEnumerator) files = NULL;
    g_autoptr(GError) error = NULL;
    g_autoptr(GFile) path = g_file_new_for_path (dir);

    if (!g_file_query_exists (path, NULL))
        return -1;

    files = g_file_enumerate_children (path, "", G_FILE_QUERY_INFO_NONE, NULL, &error);
    while (TRUE) {
        GFileInfo *info = NULL;
        if (!g_file_enumerator_iterate (files, &info, NULL, NULL, &error))
            break;
        if (!info)
            break;

        if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY) {
            g_autofree char *newdir = g_strdup_printf ("%s/%s", dir, g_file_info_get_name (info));
            gly_parse_directory (newdir);
            continue;
        }

        if (g_file_info_get_file_type (info) != G_FILE_TYPE_REGULAR)
            continue;

        printf ("%s/%s\n", dir, g_file_info_get_name (info));
    }

    return 0;
}
