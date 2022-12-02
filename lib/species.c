#include "species.h"
#include <string.h>

bool species_is_valid(const char* sp, char** sp_arr)
{
  while (*sp_arr != NULL) {
    if (strcmp(sp, *sp_arr) == 0) {
      return true;
    }

    sp_arr++;
  }

  return false;
}
