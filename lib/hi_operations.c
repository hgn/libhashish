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

/* hi_lookup search for a given key and return SUCCESS
 * when found in the hash and FAILURE if not found.
 *
 * @arg hi_handle the hashish handle
 * @arg key the pointer to the key
 * @arg keylen the len of the key in bytes
 * @return SUCCESS if found or FAILURE when not found
 */

int hi_lookup(const hi_handle_t *hi_handle, void *key, uint32_t keylen)
{
	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			return lhi_lookup_list(hi_handle, key, keylen);

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
			return lhi_lookup_array(hi_handle, key, keylen);

		case COLL_ENG_RBTREE:
			return lhi_lookup_rbtree(hi_handle, key, keylen);

		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
	}

	/* catch rule */
	return FAILURE;
}

/**
 * Remove a bucket from the list.
 * This function only do some list handling. It is up to the callee to
 * free this object and do some stat keeping (bucket_size, etc.).
 *
 * @arg hi_bucket_obj the object to remove from bucket list
 */
void lhi_bucket_obj_remove(hi_bucket_obj_t *hi_bucket_obj)
{

}

/**
 * lhi_bucket_remove cleans a bucket completely. No further
 * free() is needed (and imposible ;).
 *
 * @arg hi_handle the hashish handle
 * @arg bucket_index the index to the bucket
 * @return SUCCESS or negative error value and set lhi_errno (see * hi_geterror())
 */
int lhi_bucket_remove(hi_handle_t *hi_handle, unsigned int bucket_index)
{

}

/**
 * hi_get return for a given key the correspond data entry
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @arg keylen the length of the key in bytes
 * @data the pointer-pointer for the returned data
 * @returns FAILURE or SUCCESS on success and set data pointer
 */
int hi_get(const hi_handle_t *hi_handle, void *key, uint32_t keylen, void **data)
{

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			return lhi_get_list(hi_handle, key, keylen, data);

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
		case COLL_ENG_RBTREE:
		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
	}

	/* catch rule */
	return FAILURE;

}

/**
 * hi_remove remove a complete dataset completly from the hash set
 *
 * @arg hi_handle the hashish handle
 * @arg key a pointer to the key
 * @arg keylen the length of the key in bytes
 * @data the pointer-pointer for the returned data
 * @returns FAILURE or SUCCESS on success and set data pointer
 */
int hi_remove(const hi_handle_t *hi_handle, void *key, uint32_t keylen, void **data)
{
	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			return lhi_remove_list(hi_handle, key, keylen, data);

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
		case COLL_ENG_RBTREE:
		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
	}

	/* catch rule */
	return FAILURE;
}

/**
 * hi_insert insert a key/data pair into our hashhandle
 *
 * @arg hi_handle the hashish handle
 * @return SUCCESS or a negativ return values in the case of an error
 */
int hi_insert(const hi_handle_t *hi_handle, void *key, uint32_t keylen, void *data)
{
	int ret; uint32_t bucket;

	lhi_pthread_lock(hi_handle->mutex_lock);

	if (hi_lookup(hi_handle, key, keylen) == SUCCESS) { /* already in hash or error */
		return hi_error(EINVAL, "key already in hashtable");
	}

	switch (hi_handle->coll_eng) {

		case COLL_ENG_LIST:
		case COLL_ENG_LIST_HASH:
		case COLL_ENG_LIST_MTF:
		case COLL_ENG_LIST_MTF_HASH:
			ret = lhi_insert_list((hi_handle_t *)hi_handle, key, keylen, data);
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return ret;

		case COLL_ENG_ARRAY:
		case COLL_ENG_ARRAY_HASH:
		case COLL_ENG_ARRAY_DYN:
		case COLL_ENG_ARRAY_DYN_HASH:
			ret = lhi_insert_array((hi_handle_t *)hi_handle, key, keylen, data);
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return ret;

		case COLL_ENG_RBTREE:
			ret = lhi_insert_rbtree(hi_handle, key, keylen, data);
			lhi_pthread_unlock(hi_handle->mutex_lock);
			return ret;

		default:
			return hi_error(EINVAL,
					"Internal library error - send a bug report! ;-)");
	}

	lhi_pthread_unlock(hi_handle->mutex_lock);

	return FAILURE;
}



/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */