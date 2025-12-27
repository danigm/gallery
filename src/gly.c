#include <glib.h>
#include "gly.h"

/**
 * gly_new:
 * Returns: (transfer full): The new #Gallery struct
 * Creates a new Gallery and initialize to default
 */
Gallery *
gly_new ()
{
    Gallery *gly = g_malloc0 (sizeof (Gallery));
    return gly;
}

/**
 * gly_free:
 * @gly: The #Gallery struct
 *
 * Free all the allocated memory and struct
 */
void
gly_free (Gallery *gly)
{
    g_free (gly);
}
