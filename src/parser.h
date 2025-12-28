#pragma once

#include <glib/ghash.h>
#include "gly.h"

int gly_parse_directory (Gallery *gly, const char *dir);
int gly_parse_image     (Gallery *gly, const char *path);
