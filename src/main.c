#include <stdio.h>
#include "gly.h"
#include "parser.h"
#include "writer.h"


int
main(int argc, char **argv)
{
    int ret = 0;
    Gallery *gly = NULL;

    if (argc != 2) {
        printf ("Usage: %s path\n", argv[0]);
        return -1;
    }

    gly = gly_new ();
    ret = gly_parse_directory (gly, argv[1]);

    // TODO: template folder as argument
    gly_writer_write (gly, "html", "output");

    return ret;
}
