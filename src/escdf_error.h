/* Copyright (C) 2016-2017 Damien Caliste <dcaliste@free.fr>
 *                         Micael Oliveira <micael.oliveira@mpsd.mpg.de>
 *                         Yann Pouillon <devops@materialsevolution.es>
 *
 * This file is part of ESCDF.
 *
 * ESCDF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, version 2.1 of the License, or (at your option) any
 * later version.
 *
 * ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 */

#ifndef LIBESCDF_ERROR_H
#define LIBESCDF_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file escdf_error.h 
 * @brief Error handlers
 */

#include <stdlib.h>
#include <stdio.h>

#include "escdf_common.h"


/**********************************************************************
 * Data structures                                                    *
 **********************************************************************/

/**
 * Global error handling structure
 */
struct escdf_error_type {
    int id; /**< ID of the error */
    char *filename; /**< name of the file where the error appeared */
    int line; /**< line number in the file where the error appeared */
    char *routine; /**< routine where the error appeared */
    struct escdf_error_type *next; /**< next error in the chain */
};
typedef struct escdf_error_type escdf_error_t;

typedef int escdf_errno_t;

/**********************************************************************
 * Routines                                                           *
 **********************************************************************/

/**
 * Add an error to the chain.
 * @param[in] error_id: error code.
 * @param[in] filename: source filename (use NULL if none).
 * @param[in] line: line number in the source file (ignored if filename
 *            is NULL).
 * @param[in] routine: current routine in the source file.
 * @return the error code provided as input (for automation purposes).
 */
escdf_errno_t escdf_error_add(const escdf_errno_t error_id, const char *filename,
                              const int line, const char *routine);

/**
 * Fetch and clear the error chain.
 * @return string pointer describing the chain of errors
 */
char *escdf_error_fetchall(void);

/**
 * Flush and clear the error chain.
 * @param[in] fd: file descriptor for the output.
 * @return error code
 */
void escdf_error_flush(FILE *fd);

/**
 * Clear the error chain.
 */
void escdf_error_free(void);

/**
 * Get the current error status.
 * @param[in] routine: return last error of the specified routine, or the very 
 *            last error if NULL.
 * @return error code
 */
escdf_errno_t escdf_error_get_last(const char *routine);

/**
 * Get the length of the error chain.
 * @return length of the error chain
 */
int escdf_error_len(void);

/**
 * Pop the first available error.
 * @return error structure pointer
 */
escdf_error_t *escdf_error_pop(void);

/**
 * Displays an error message.
 * @param[in] error_id: integer identifying the error.
 * @param[in] filename: source filename (use NULL if none).
 * @param[in] line: line number in the source file (ignored if filename
 *            is NULL).
 * @param[in] routine: current routine in the source file.
 * @return string with error message.
 */
void escdf_error_show(const escdf_errno_t error_id, const char *filename,
                      const int line, const char *routine);

/**
 * Returns a string with error description.
 * @param[in] error_id: integer identifying the error.
 * @return string with error message.
 */
const char *escdf_error_string(const escdf_errno_t error_id);


/**********************************************************************
 * Macros                                                             *
 **********************************************************************/



/**
 * Macro to break loops when there are deferred errors, in order to propagate
 * error conditions from inner loops to outer loops
 */
#define BREAK_ON_DEFERRED_ERROR \
    if ( escdf_error_get_last(__func__) != ESCDF_SUCCESS ) { \
      break; \
    }

/**
 * Deferred error handler macro for function calls, when it is necessary
 * to continue executing the code despite the error
 * @param[in] function_call: the function to be called with all its parameters
 */
#define DEFER_FUNC_ERROR(function_call) \
  escdf_error_add(function_call, __FILE__, __LINE__, __func__);

/**
 * Deferred error handler macro for unsatisfied conditions, when it is
 * necessary to continue executing the code despite the error
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set
 */
#define DEFER_TEST_ERROR(condition, error_id) \
    if ( !(condition) ) {		    \
        escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
    }

/**
 * Macro to break out of a loop when a condition is unsatisfied
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set before breaking out of the loop
 */
#define FULFILL_OR_BREAK(condition, error_id) \
    if (!(condition)) { \
        escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
        break; \
    }

/**
 * Macro to exit the program when a condition is unsatisfied
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set before aborting
 */
#define FULFILL_OR_EXIT(condition, error_id) \
    if ( !(condition) ) {		    \
        escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
        escdf_error_flush(stderr); \
        exit(1); \
    }

/**
 * Macro to return from a routine when a condition is unsatisfied
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set before returning
 */
#define FULFILL_OR_RETURN(condition, error_id) \
    if (!(condition)) {		    \
        return escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
    }

/**
 * Macro to return from a routine when a condition is unsatisfied with additional deallocating
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set before returning
 */
#define FULFILL_OR_RETURN_CLEAN(condition, error_id, ptr) \
    if (!(condition)) {		    \
        if(ptr != NULL) free(ptr); \
        return escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
    }


/**
 * Macro to return a specific value from a routine when a condition is unsatisfied
 * @param[in] condition: condition to check
 * @param[in] error_id: error code to set before returning
 * @param[in] val: returned value
 */
#define FULFILL_OR_RETURN_VAL(condition, error_id, val)     \
    if (!(condition)) {		    \
        escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
        return val; \
    }

/**
 * Error handler macro for deferred errors
 */
#define RETURN_ON_DEFERRED_ERROR \
    if ( escdf_error_get_last(__func__) != ESCDF_SUCCESS ) { \
        return escdf_error_get_last(__func__); \
    }

/**
 * Error handler macro ensuring that errors are properly registered
 * before returning from a function
 * @param[in] error_id: error code
 */
#define RETURN_WITH_ERROR(error_id) \
    return escdf_error_add(error_id, __FILE__, __LINE__, __func__);

/**
 * Break the current loop when a pspio function call fails
 * @param[in] function_call: the function to be called with all its parameters
 */
#define SUCCEED_OR_BREAK(function_call) \
    if ( escdf_error_add(function_call, __FILE__, __LINE__, __func__) != ESCDF_SUCCESS ) { \
        break; \
    }

/**
 * Return when a pspio function call fails
 * @param[in] function_call: the function to be called with all its parameters
 */
#define SUCCEED_OR_RETURN(function_call) \
    if ( escdf_error_add(function_call, __FILE__, __LINE__, __func__) != ESCDF_SUCCESS ) { \
        return escdf_error_get_last(NULL); \
    }

/**
 * Macro to skip routine execution on error
 * @param[in] routine_call: the routine to be called with all its parameters
 */
#define SKIP_CALL_ON_ERROR(routine_call) \
    if ( escdf_error_get_last(__func__) == ESCDF_SUCCESS ) { \
        routine_call; \
    }

/**
 * Macro to skip function execution on error
 * @param[in] function_call: the function to be called with all its parameters
 */
#define SKIP_FUNC_ON_ERROR(function_call) \
    if ( escdf_error_get_last(__func__) == ESCDF_SUCCESS ) { \
        escdf_error_add(function_call, __FILE__, __LINE__, __func__); \
    }

/**
 * Macro to skip a test on error
 * @param[in] function_call: the function to be called with all its parameters
 */
#define SKIP_TEST_ON_ERROR(condition, error_id) \
    if ( (escdf_error_get_last(__func__) == ESCDF_SUCCESS) && !(condition) ) { \
        escdf_error_add(error_id, __FILE__, __LINE__, __func__); \
    }


/**
 * Error handler macro ensuring that errors are properly registered
 * 
 * @param[in] error_id: error code
 */
#define REGISTER_ERROR(error_id) \
    escdf_error_add(error_id, __FILE__, __LINE__, __func__);


#ifdef __cplusplus
}
#endif

#endif
