#include <glib.h>

#include "gly.h"
#include "img.h"

static void
destroy_list (GList *list)
{
    g_list_free_full (list, (GDestroyNotify)gly_img_free);
}

/**
 * gly_new:
 * Returns: (transfer full): The new #Gallery struct
 * Creates a new Gallery and initialize to default
 */
Gallery *
gly_new ()
{
    Gallery *gly = g_malloc0 (sizeof (Gallery));
    gly->buckets = g_hash_table_new_full (g_int_hash,
                                          g_int_equal,
                                          NULL,
                                          (GDestroyNotify)destroy_list);
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
    g_hash_table_unref (gly->buckets);
    g_free (gly);
}
