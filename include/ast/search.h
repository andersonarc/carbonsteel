/**
 * @file search.h
 * @author mikhail-j (github.com/mikhail-j)
 * @author andersonarc (e.andersonarc@gmail.com)
 * @author *see the comment below*
 * @version 0.1
 * @date 2021-09-18
 * 
 * @copyright This file is licensed under the GNU GPL license
 *              and its original authors are listed below.
 *              It was copied from github.com/mikhail-j/libc_hsearch_r
 * 
 *  Re-entrant hash table search functions,
 *  embedded in the code for cross-platform compatibility
 */

/* Re-entrant hash table search function definitions.
 *
 * Copyright (C) 1993-2019 Free Software Foundation, Inc.
 * Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1993.
 * Copyright (C) 2019 Qijia (Michael) Jin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <search.h>

#ifndef CARBONSTEEL_AST_SEARCH_H
#define CARBONSTEEL_AST_SEARCH_H

#ifndef _ENTRY
struct _ENTRY;
#endif

struct hsearch_data {
	struct _ENTRY *table;
	unsigned int size;
	unsigned int filled;
};

int hcreate_r (size_t nel, struct hsearch_data *htab);
void hdestroy_r (struct hsearch_data *htab);
int hsearch_r (ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *htab);

#endif /* CARBONSTEEL_AST_SEARCH_H */

#ifdef __cplusplus
}
#endif