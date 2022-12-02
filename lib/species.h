/**
 * @file
 * @date 2022-11-30 21:27:33 PM +0800
 */
#ifndef SPECIES_H
#define SPECIES_H

#include <stdbool.h>

/**
 * Check if the specified species is listed in an array stored the valied
 * species
 *
 * @param sp A species name to be checked
 * @param sp_arr A species array used to check
 *
 * @return true if sp is in sp_arr, false otherwise
 */
bool species_is_valid(const char* sp, char** sp_arr);

#endif
