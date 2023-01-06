/** @file */
#ifndef DATE_H
#define DATE_H

#include <stdbool.h>

/**
 * Check whether the date is valid
 *
 * @param[in] date A string represents data in the format of YYYY-MM-DD
 *
 * @return true if the date is valid, otherwise false
 */
bool date_is_valid(const char* date);

#endif
