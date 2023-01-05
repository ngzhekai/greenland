#include "tree_state.h"

const char* trstat_to_string(tree_state st)
{
  switch (st) {
    case DEAD:
      return "DEAD";

    case PLANTED:
      return "PLANTED";

    case SICK:
      return "SICK";

    case TREAMENT:
      return "TREAMENT";

    default:
      return "UNKNOWN STATE";
  }
}

bool trstat_is_valid(tree_state st)
{
  if (st >= 0 && st < 4) {
    return true;
  }

  return false;
}
