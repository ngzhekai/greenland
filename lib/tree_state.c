#include "tree_state.h"

const char* trstat_to_string(tree_state st)
{
  switch (st) {
    case DEAD:
      return "DEAD";

    case ALIVE:
      return "ALIVE";

    case SICK:
      return "SICK";

    case TREAMENT:
      return "TREAMENT";

    default:
      return (char*) 0;
  }
}

bool trstat_is_valid(tree_state st)
{
  if (!(trstat_to_string(st))) {
    return false;
  }

  return true;
}