#include <glib.h>
#include <locale.h>

#include "../src/parser.h"


static void
gly_parser_test1 ()
{
    Gallery *gly = gly_new ();
    int ret = gly_parse_directory (gly, "testfiles");
    g_assert (ret == 0);

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
