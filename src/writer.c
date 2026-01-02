#include <gio/gio.h>
#include <glib.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlsave.h>

#include "img.h"
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
 * Look for the div with "gallery" id
 */
static xmlNodePtr
get_gallery_block (xmlDocPtr doc)
{
    // Use xpath to get the target "//div[@id=\"gallery\"]"
    const char *str = "//div[@id=\"gallery\"]";
    xmlNodePtr ret = NULL;
    xmlXPathObjectPtr res = NULL;
    xmlXPathContextPtr ctxt = NULL;

    ctxt = xmlXPathNewContext (doc);
    ctxt->node = xmlDocGetRootElement (doc);
    res = xmlXPathEvalExpression (BAD_CAST str, ctxt);

    // Getting the first one, there should be just one
    if (!res || !res->nodesetval || res->nodesetval->nodeNr != 1) {
        fprintf (stderr, "Unable to find div with \"gallery\" id in template\n");
        goto out;
    }

    ret = (xmlNodePtr)res->nodesetval->nodeTab[0];

out:
    xmlXPathFreeObject (res);
    xmlXPathFreeContext (ctxt);

    return ret;
}

/**
 * Build the xmlNode for a image
 * TODO: define the template and just replace values
 * <a href="IMG_PATH" data-pswp-width="IMG_WITH" data-pswp-height="IMG_HEIGHT" target="_blank">
 *     <img src="IMG_PATH" />
 * </a>
 */
static xmlNodePtr
img_build_node (xmlDocPtr doc, Img *img)
{
    xmlNodePtr node = xmlNewDocNode (doc, NULL, BAD_CAST "a", NULL);
    xmlNodePtr node_img = xmlNewDocNode (doc, NULL, BAD_CAST "img", NULL);
    char *value = NULL;
    int width = 0;

    xmlSetProp (node, BAD_CAST "href", BAD_CAST img->path);
    xmlSetProp (node, BAD_CAST "target", BAD_CAST "_blank");

    value = g_strdup_printf ("%d", gly_img_width (img));
    xmlSetProp (node, BAD_CAST "data-pswp-width", BAD_CAST value);
    g_free (value);
    value = g_strdup_printf ("%d", gly_img_height (img));
    xmlSetProp (node, BAD_CAST "data-pswp-height", BAD_CAST value);
    g_free (value);

    // This is the thumbnail
    // TODO: Resize the thumbnail with js or should we add some kind of width attr?
    xmlSetProp (node_img, BAD_CAST "src", BAD_CAST img->path);
    width = gly_img_width (img) / 20;
    value = g_strdup_printf ("%d", width ? width : 200);
    xmlSetProp (node_img, BAD_CAST "width", value);
    g_free (value);
    xmlAddChild (node, node_img);

    return node;
}

/**
 * gly_writer_write:
 * @gly the actual gallery to write to filesystem
 * @path the path to the images folder
 * @tmpl the path to the template folder. It must contain a file name template.html
 * @out the path to the output directory
 */
void
gly_writer_write (Gallery *gly, const char *path, const char *tmpl, const char *out)
{
    xmlDocPtr template = NULL;
    xmlDocPtr index = NULL;

    g_autoptr(GFile) of = g_file_new_for_path (out);
    g_autoptr(GFile) images = g_file_new_for_path (path);
    g_autoptr(GError) error = NULL;
    g_autoptr(GList) keys = NULL;

    g_autofree char *filename = g_strdup_printf ("%s/%s", out, "template.html");

    g_file_make_directory (of, NULL, &error);
    images = g_file_get_child (of, g_file_get_basename (images));
    g_file_make_directory (images, NULL, &error);
    // Copy all files from template folder
    copy_all_files (tmpl, out);
    // Copy images folder in output
    copy_all_files (path, g_file_get_path (images));

    // Loading template file
    template = xmlReadFile (filename, NULL, 0);

    // Iterate over all dates and create a different file for each one with all images.
    keys = g_hash_table_get_keys (gly->buckets);
    while (keys != NULL) {
        int k = *(int*)keys->data;
        xmlDocPtr doc = xmlCopyDoc (template, 1);
        xmlNodePtr block = get_gallery_block (doc);

        GList *imgs = NULL;

        g_autofree char *uri = g_strdup_printf ("%s/%d.html", out, k);

        //  Iterate over all images in this key and add to the block
        imgs = g_hash_table_lookup (gly->buckets, &k);
        xmlNodeAddContent (block, BAD_CAST "\n");
        while (imgs != NULL) {
            xmlNodePtr img_node = img_build_node (doc, (Img*)imgs->data);
            xmlAddChild (block, img_node);
            xmlNodeAddContent (block, BAD_CAST "\n");
            imgs = imgs->next;
        }
        // TODO: Add links to prev/next
        // TODO: Add link to index.html

        // Write the new doc as key.html
        xmlSaveCtxt *ctxt = xmlSaveToFilename(uri, "UTF-8", XML_SAVE_FORMAT | XML_SAVE_XHTML | XML_SAVE_INDENT);
        xmlSaveSetIndentString (ctxt, "\t");

        if (xmlSaveDoc (ctxt, doc) < 0) {
            fprintf(stderr, "failed save to %s\n", uri);
        }
        xmlSaveClose (ctxt);
        xmlFreeDoc (doc);

        keys = keys->next;
    }

    xmlFreeDoc (template);
}
