#include <glib.h>
#include <locale.h>

#include "../src/parser.h"


static void
gly_parser_test1 ()
{
    int ret = gly_parse_directory ("testfiles");
    g_assert (ret == 0);
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
