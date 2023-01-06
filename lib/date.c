#include "date.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isleap(unsigned);
bool date_is_valid_f(unsigned, unsigned, unsigned);

bool isleap(unsigned y)
{
  if ((y % 4 == 0) && (y % 100 != 0) && (y % 400 == 0)) {
    return true;
  } else {
    return false;
  }
}

bool date_is_valid_f(unsigned y, unsigned m, unsigned d)
{
  if (m < 1 || m > 12) {
    return false;
  }

  if (d < 1 || d > 31) {
    return false;
  }

  if (m == 2) {
    if (isleap(y)) {
      if (d <= 29) {
        return true;
      } else {
        return false;
      }
    }
  }

  if (m == 4 || m == 6 || m == 9 || m == 11) {
    if (d <= 30) {
      return true;
    } else {
      return false;
    }
  }

  return true;
}

bool date_is_valid(const char* date)
{
  char* year_s = calloc(5, sizeof(char));
  char* month_s = calloc(3, sizeof(char));
  char* day_s = calloc(3, sizeof(char));
  sscanf(date, "%[^-]-%[^-]-%[^-]", year_s, month_s, day_s);

  if (strlen(year_s) != 4 || strlen(month_s) != 2 || strlen(day_s) != 2) {
    return false;
  }

  unsigned year = atoi(year_s);
  unsigned month = atoi(month_s);
  unsigned day = atoi(day_s);

  return date_is_valid_f(year, month, day);
}
