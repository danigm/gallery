#pragma once

#include <glib.h>

typedef struct {
    GHashTable *buckets; // HashTable(YYYYMM, [Img, Img, ...])
} Gallery;

Gallery *gly_new ();
void     gly_free (Gallery *gly);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(Gallery, gly_free)
