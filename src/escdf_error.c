/*
 Copyright (C) 2011-2012 J. Alberdi, M. Oliveira, Y. Pouillon, and M. Verstraete

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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "escdf_error.h"

#if defined HAVE_CONFIG_H
#include "config.h"
#endif

/* Store successive errors in a chain */
static escdf_error_t *ESCDF_error_chain = NULL;

escdf_errno_t escdf_error_add(const escdf_errno_t error_id, const char *filename,
                              const int line, const char *routine)
{
    size_t s;
    escdf_error_t *last_err;

    /* Notes:
       * this routine cannot call any error macro, in order to avoid
         infinite loops;
       * errors in this routine must be fatal, in order to avoid
         infinite loops;
       * ESCDF_SUCCESS must always be ignored;
       * this routine returns the submitted error ID for automation
         purposes.
    */

    if ( error_id == ESCDF_SUCCESS )
        return error_id;

    if ( ESCDF_error_chain == NULL ) {
        ESCDF_error_chain = malloc (sizeof(escdf_error_t));
        if ( ESCDF_error_chain == NULL ) {
        escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
        exit(1);
        }

        ESCDF_error_chain->id = error_id;
        ESCDF_error_chain->line = line;
        ESCDF_error_chain->next = NULL;
        s = strlen(filename);
        ESCDF_error_chain->filename = (char *) malloc((s+1)*sizeof(char));
        if ( ESCDF_error_chain->filename == NULL ) {
        escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
        exit(1);
        }
        memcpy(ESCDF_error_chain->filename, filename, s);
        ESCDF_error_chain->filename[s] = '\0';
        s = strlen(routine);
        ESCDF_error_chain->routine = (char *) malloc((s+1)*sizeof(char));
        if ( ESCDF_error_chain->routine == NULL ) {
            escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
            exit(1);
        }
        memcpy(ESCDF_error_chain->routine, routine, s);
        ESCDF_error_chain->routine[s] = '\0';
    } else {
        last_err = ESCDF_error_chain;
        while ( last_err->next != NULL ) {
        last_err = last_err->next;
        }

        last_err->next = (escdf_error_t *) malloc (sizeof(escdf_error_t));
        if ( last_err->next == NULL ) {
        escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
        exit(1);
        }
        last_err = last_err->next;
        last_err->id = error_id;
        last_err->line = line;
        last_err->next = NULL;
        s = strlen(filename);
        last_err->filename = (char *) malloc((s+1)*sizeof(char));
        if ( last_err->filename == NULL ) {
        escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
        exit(1);
        }
        memcpy(last_err->filename, filename, s);
        last_err->filename[s] = '\0';
        s = strlen(routine);
        last_err->routine = (char *) malloc((s+1)*sizeof(char));
        if ( last_err->routine == NULL ) {
        escdf_error_show(ESCDF_ENOMEM, __FILE__, __LINE__, __func__);
        exit(1);
        }
        memcpy(last_err->routine, routine, s);
        last_err->routine[s] = '\0';
    }

    return error_id;
}

void escdf_error_fetchall(char **err_str) 
{
    char buf[8];
    char *tmp_str;
    int err_len;
    escdf_error_t *cursor = ESCDF_error_chain;

    *err_str = NULL;

    if ( cursor != NULL ) {
        *err_str  = (char *) malloc (20*sizeof(char));
        assert(*err_str != NULL);
        sprintf(*err_str, "%s\n", "libescdf: ERROR:");
    }

    while ( cursor != NULL ) {
        assert(cursor->filename != NULL);
        assert(cursor->routine != NULL);

        err_len  = 19;
        err_len += strlen(escdf_error_string(cursor->id));
        err_len += strlen(cursor->filename);
        err_len += strlen(cursor->routine);
        sprintf(buf, "%d", cursor->line);
        err_len += strlen(buf);

        tmp_str  = (char *) malloc ((err_len+1)*sizeof(char));
        assert(tmp_str != NULL);
        sprintf(tmp_str, "  * in %s(%s):%d:\n      %s\n", cursor->filename,
                cursor->routine, cursor->line, escdf_error_string(cursor->id));
        *err_str = realloc(*err_str, strlen(*err_str)+err_len+1);
        if ( *err_str == NULL ) {
        fprintf(stderr, "libescdf: FATAL:\n      could not build error message"
                ".\n");
        exit(1);
        }
        strcat(*err_str, tmp_str);
        free(tmp_str);

        cursor = cursor->next;
    }

    escdf_error_free();
}

void escdf_error_flush(FILE *fd)
{
    char *err_str = NULL;

    assert(fd != NULL);

    escdf_error_fetchall(&err_str);
    if ( err_str != NULL ) {
        fprintf(fd, "%s", err_str);
        fflush(fd);
        free(err_str);
    }
}

void escdf_error_free(void)
{
    escdf_error_t *first_err;

    while ( ESCDF_error_chain != NULL ) {
        first_err = ESCDF_error_chain;
        ESCDF_error_chain = ESCDF_error_chain->next;
        free(first_err->filename);
        free(first_err->routine);
        free(first_err);
    }
}

escdf_errno_t escdf_error_get_last(const char *routine)
{
    escdf_errno_t eid = ESCDF_SUCCESS;
    escdf_error_t *cursor = ESCDF_error_chain;

    while ( cursor != NULL ) {
        if ( routine == NULL ) {
            eid = cursor->id;
        } else {
            if ( strcmp(cursor->routine, routine) == 0 ) {
                eid = cursor->id;
            }
        }
        cursor = cursor->next;
    }

    return eid;
}

int escdf_error_len(void)
{
    int n = 0;
    escdf_error_t *cursor = ESCDF_error_chain;

    while ( cursor != NULL ) {
        n++;
        cursor = cursor->next;
    }

    return n;
}

escdf_error_t *escdf_error_pop(void)
{
    escdf_error_t *first_error = NULL;

    if ( ESCDF_error_chain != NULL ) {
        first_error = ESCDF_error_chain;
        ESCDF_error_chain = ESCDF_error_chain->next;
    }

    return first_error;
}

void escdf_error_show(const escdf_errno_t error_id, const char *filename,
                      const int line, const char *routine)
{
    fprintf(stderr, "libescdf: ERROR:\n");
    if ( (filename != NULL) && (routine != NULL) ) {
        fprintf(stderr, "  * in %s(%s):%d:\n", filename, routine, line);
    }
    fprintf(stderr, "      %s\n", escdf_error_string(error_id));
}

const char *escdf_error_string(const escdf_errno_t error_id)
{
    switch (error_id) {
    case ESCDF_SUCCESS:
        return "success (ESCDF_SUCCESS)" ;
    case ESCDF_ERROR:
        return "error (ESCDF_ERROR)" ;
    case ESCDF_EFILE_CORRUPT:
        return "file corrupted (ESCDF_EFILE_CORRUPT)";
    case ESCDF_EFILE_FORMAT:
        return "unknown file format (ESCDF_EFILE_FORMAT)";
    case ESCDF_EIO:
        return "input/output error (ESCDF_EIO)" ;
    case ESCDF_ENOFILE:
        return "file does not exist (ESCDF_ENOFILE)" ;
    case ESCDF_ENOMEM:
        return "malloc failed (ESCDF_ENOMEM)";
    case ESCDF_ENOSUPPORT:
        return "unsupported option in file (ESCDF_ENOSUPPORT)";
    case ESCDF_EVALUE:
        return "value error: bad value found (ESCDF_EVALUE)";
    default:
        return "unknown error code";
    }
}
