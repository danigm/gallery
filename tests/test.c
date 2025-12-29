#include <glib.h>
#include <locale.h>
#include <stdio.h>

#include "../src/parser.h"
#include "../src/img.h"


static void
gly_parser_test1 ()
{
    Gallery *gly = gly_new ();
    int ret = gly_parse_directory (gly, "testfiles");
    g_assert (ret == 0);

    int key = 202506;
    GList *keys = g_hash_table_get_keys (gly->buckets);
    g_assert (keys != NULL);
    key = *(int*)keys->data;

    GList *imgs = g_list_first (g_hash_table_lookup (gly->buckets, &key));
    g_assert (imgs != NULL);
    g_assert (g_list_length (imgs) > 0);

    Img *prev = NULL;
    while (imgs) {
        Img *img = (Img*)imgs->data;
        if (prev) {
            g_assert (gly_img_cmp (prev, img) <= 0);
        }
        printf ("%s - %s\n", img->path, g_date_time_format (img->date, "%Y/%m/%d - %H:%M"));
        prev = img;
        imgs = imgs->next;
    }

    gly_free (gly);
}

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);

  // Define the tests.
  g_test_add_func ("/gly-parser/test1", gly_parser_test1);

  return g_test_run ();
}
