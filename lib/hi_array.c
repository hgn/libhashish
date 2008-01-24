/*
** $Id$
**
** Copyright (C) 2006 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "privlibhashish.h"
#include "libhashish.h"
#include "list.h"

#include "threads.h"

/* lhi_lookup_array search for a given key and return SUCCESS
 * when found in the hash and FAILURE if not found.
 *
 * @arg hi_handle the hashish handle
 * @arg key the pointer to the key
 * @arg keylen the len of the key in bytes
 * @return SUCCESS if found or FAILURE when not found
 */
int lhi_lookup_array(const hi_handle_t *hi_handle,
		void *key, uint32_t keylen)
{
	uint32_t i, bucket;
	bucket =  hi_handle->hash_func(key, keylen) % hi_handle->table_size;

	switch (hi_handle->coll_eng) {

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_DYN:
			/* FIXME: the hash variant must still be implemented */
		case COLL_ENG_ARRAY_DYN_HASH:
		case COLL_ENG_ARRAY_HASH:
			for (i = 0; i < hi_handle->eng_array.bucket_array_slot_size[bucket]; i++) {
				if (hi_handle->key_cmp(key,
							hi_handle->eng_array.bucket_array[bucket][i].key)) {
					return SUCCESS;
				}
			}
			break;

		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
	}


	return FAILURE;
}


/* lhi_insert_array insert a key/data pair into our hashhandle
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int lhi_insert_array(hi_handle_t *hi_handle, void *key,
		uint32_t keylen, void *data)
{
	uint32_t bucket;

	bucket = hi_handle->hash_func(key, keylen) % hi_handle->table_size;
	if (hi_handle->eng_array.bucket_array_slot_size[bucket] >=
			hi_handle->eng_array.bucket_array_slot_max[bucket]) {

		/* bucket size exhausted -> double it */
		hi_handle->eng_array.bucket_array_slot_max[bucket] =
			hi_handle->eng_array.bucket_array_slot_max[bucket] << 1;

		hi_handle->eng_array.bucket_array[bucket] = realloc(hi_handle->eng_array.bucket_array[bucket],
				sizeof(hi_bucket_a_obj_t) * hi_handle->eng_array.bucket_array_slot_max[bucket]);
		if (hi_handle->eng_array.bucket_array[bucket] == NULL) {
			return hi_errno(errno);
		}

	}

	/* add key/data add next free slot */
	hi_handle->eng_array.bucket_array[bucket][hi_handle->eng_array.bucket_array_slot_size[bucket]].key = key;
	hi_handle->eng_array.bucket_array[bucket][hi_handle->eng_array.bucket_array_slot_size[bucket]].data = data;

	hi_handle->eng_array.bucket_array_slot_size[bucket]++;
	hi_handle->no_objects++;

	return SUCCESS;
}

int lhi_fini_array(hi_handle_t *hi_handle)
{
	uint32_t i;

	lhi_pthread_lock(&hi_handle->mutex_lock);
	for (i = 0; i < hi_handle->table_size; i++) {
		free(hi_handle->eng_array.bucket_array[i]);
	}
	free(hi_handle->eng_array.bucket_array);
	free(hi_handle->eng_array.bucket_array_slot_size);
	free(hi_handle->eng_array.bucket_array_slot_max);
	free(hi_handle->bucket_size);
	lhi_pthread_unlock(&hi_handle->mutex_lock);

	return SUCCESS;
}

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */