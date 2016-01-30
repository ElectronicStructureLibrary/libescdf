
#ifndef LIBESCDF_UTILS_H
#define LIBESCDF_UTILS_H

#include <stdbool.h>

/**********************************************************************
 * Data structures                                                    *
 **********************************************************************/

/**
* Boolean type with extra variable to keep track of status (set or unset)
*/
typedef struct {
    bool value;
    bool is_set;
} _bool_set_t;

/**
* Unsigned integer type with extra variable to keep track of status (set or unset)
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

#endif