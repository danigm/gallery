#include <gio/gio.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "writer.h"

static void
copy_all_files (const char *src, const char *dst)
{
    g_autoptr(GFile) sp = g_file_new_for_path (src);
    g_autoptr(GFile) dp = g_file_new_for_path (dst);
    g_autoptr(GFileEnumerator) files = NULL;
    g_autoptr(GError) error = NULL;

    files = g_file_enumerate_children (sp, "", G_FILE_QUERY_INFO_NONE, NULL, &error);

    while (TRUE) {
        g_autofree char *file_path = NULL;
        g_autofree char *src_path = NULL;
        GFileInfo *info = NULL;

        if (!g_file_enumerator_iterate (files, &info, NULL, NULL, &error))
            break;
        if (!info)
            break;

        file_path = g_strdup_printf ("%s/%s", dst, g_file_info_get_name (info));
        src_path = g_strdup_printf ("%s/%s", src, g_file_info_get_name (info));

        if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY) {
            g_autoptr(GFile) dir = g_file_new_for_path (file_path);
            if (!g_file_make_directory (dir, NULL, &error)) {
                g_error_free (error);
                error = NULL;
            }
            copy_all_files (src_path, file_path);
            continue;
        }
        if (g_file_info_get_file_type (info) != G_FILE_TYPE_REGULAR)
            continue;

        {
            g_autoptr(GFile) s = g_file_new_for_path (src_path);
            g_autoptr(GFile) d = g_file_new_for_path (file_path);
            if (!g_file_copy (s, d, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
                fprintf (stderr, "Unable to copy file: %s\n", error->message);
            }
        }
    }
}

/**
 * gly_writer_write:
 * @gly the actual gallery to write to filesystem
 * @tmpl the path to the template folder. It must contain a file name template.html
 * @out the path to the output directory
 */
void
gly_writer_write (Gallery *gly, const char *tmpl, const char *out)
{
    g_autoptr(GFile) of = g_file_new_for_path (out);
    g_autoptr(GError) error = NULL;
    g_autofree char *filename = g_strdup_printf ("%s/%s", out, "template.html");

    g_file_make_directory (of, NULL, &error);
    // Copy all files from template folder
    copy_all_files (tmpl, out);

    // Loading template file
    xmlDocPtr doc = xmlReadFile (filename, NULL, 0);
    // Use xpath to get the target "//div[@id=\"gallery\"]"
    // Iterate over all dates and create a different file for each one with all images.
}
