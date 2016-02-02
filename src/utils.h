/*
 Copyright (C) 2016 D. Caliste, F. Corsetti, M. Oliveira, Y. Pouillon, and D. Strubbe

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef LIBESCDF_UTILS_H
#define LIBESCDF_UTILS_H

#include <stdbool.h>

/******************************************************************************
 * Data structures                                                            *
 ******************************************************************************/

/**
 * Boolean type with extra variable to keep track of status (set or unset)
 *
 */
typedef struct {
    bool value;
    bool is_set;
} _bool_set_t;

/**
 * Unsigned integer type with extra variable to keep track of status (set or
 * unset)
 */
typedef struct {
    unsigned int value;
    bool is_set;
} _uint_set_t;

/**
 * Integer type with extra variable to keep track of status (set or unset)
 */
typedef struct {
    int value;
    bool is_set;
} _int_set_t;

/**
 * Double type with extra variable to keep track of status (set or unset)
 */
typedef struct {
    double value;
    bool is_set;
} _double_set_t;


/******************************************************************************
 * Setters                                                                    *
 ******************************************************************************/

_bool_set_t _bool_set(const bool value);

_uint_set_t _uint_set(const unsigned int value);

_int_set_t _int_set(const int value);

_double_set_t _double_set(const double value);

#endif