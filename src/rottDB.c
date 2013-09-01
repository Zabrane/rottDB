/* Copyright (c) 2013, Mert Bora Alper <boraalper4 at gmail dot com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rottDB.h"

int rottDB_init (rottDB *db) {
	db->pairs = malloc (1 * sizeof(db->pairs));
	if (db->pairs == NULL) {
		return rDB_NO_MEMORY;
	}

	// Setting default values
	db->pairs[0].hash = 0;
	db->pairs[0].key = NULL;
	db->pairs[0].value = NULL;

	db->length = 1;

	return rDB_SUCCESS;
}

void rottDB_shut (rottDB *db) {
	size_t i;

	for (i=0; i < db->length; ++i) {
		db->pairs[i].hash = 0;

		rDB_free (db->pairs[i].key);
		rDB_free (db->pairs[i].value);
	}
	rDB_free (db->pairs);

	db->length = 0;
}

int rottDB_set (rottDB *db, const char *key, const char *value) {
	size_t i, null_pos = db->length;
	const unsigned long hash = rDB_hash(key);

/* =========================== Search if key exist ========================= */
	for (i=0; i < db->length; ++i) {
		/* If you find an empty area, store it's position at null_pos. If key
		 * not exist, use this position to set pair.
		 */
		if (db->pairs[i].key == NULL) { // Found a null position
			null_pos = i; // save it

			continue;
		}

		/* Found a match
		 */
		if (rDB_isEqual(key, hash, db->pairs[i]))
		{
			char *tptr = NULL;

			tptr = malloc (rDB_size(value) * sizeof(char));
			if (tptr == NULL) // Check if malloc failed
				return rDB_NO_MEMORY;

			rDB_free (db->pairs[i].value);
			db->pairs[i].value = tptr;
			strcpy (db->pairs[i].value, value);

			return rDB_UPDATED;
		}
	}

/* ============================== Key not exist ============================= */
/* -------------------------- There is a NULL cell -------------------------- */
	if (null_pos != db->length) {
		db->pairs[null_pos].key = malloc (rDB_size(key) * sizeof(char));
		if (db->pairs[null_pos].key == NULL) { // Check if malloc failed
			return rDB_NO_MEMORY;
		}

		db->pairs[null_pos].value = malloc (rDB_size(value) * sizeof(char));
		if (db->pairs[null_pos].value == NULL) { // Check if malloc failed
			rDB_free (db->pairs[null_pos].key); // free allocated memory

			return rDB_NO_MEMORY;
		}

		// copy key and value
		strcpy (db->pairs[null_pos].key, key);
		strcpy (db->pairs[null_pos].value, value);

		db->pairs[null_pos].hash = rDB_hash(key); // calculate and set hash

		return rDB_ADDED;
	}

/* ------------------------- There isn't a NULL cell ------------------------ */
	else {
		rottDBpair *tmp_pairs = NULL;

		tmp_pairs  = realloc (db->pairs, (db->length + 1) * sizeof(rottDBpair));
		if (tmp_pairs == NULL) // Check if realloc failed
			return rDB_NO_MEMORY;
		else
			db->pairs = tmp_pairs;

		// Setting default values
		db->pairs[db->length].hash = 0;
		db->pairs[db->length].key = NULL;
		db->pairs[db->length].value = NULL;

		++(db->length);

		db->pairs[db->length -1].key = malloc (rDB_size(key) *  sizeof(char));
		if (db->pairs[db->length -1].key == NULL) // Check if malloc failed
			return rDB_NO_MEMORY;

		db->pairs[db->length -1].value = malloc (rDB_size(value) * sizeof(char));
		if (db->pairs[db->length -1].value == NULL) { // Check if malloc failed
			rDB_free (db->pairs[db->length -1].key); // free allocated memory

			return rDB_NO_MEMORY;
		}

		// copy key and value
		strcpy (db->pairs[db->length -1].key, key);
		strcpy (db->pairs[db->length -1].value, value);

		db->pairs[db->length -1].hash = rDB_hash(key); // calculate and set hash

		return rDB_ADDED;
	}
}

char *rottDB_get (const rottDB *db, const char *key) {
	size_t i;
	const unsigned long hash = rDB_hash(key);
	char *tmp = NULL;

	for (i=0; i < db->length; ++i) {
		if (rDB_isEqual(key, hash, db->pairs[i])) {
			tmp = malloc (rDB_size(db->pairs[i].value) * sizeof(char));
			if (tmp == NULL) { // Check if malloc failed
				return NULL;
			}

			strcpy (tmp, db->pairs[i].value); // copy value
			return tmp;
		}
	}

	return NULL;
}

int rottDB_del (rottDB *db, const char *key) {
	size_t i;
	const unsigned long hash = rDB_hash(key);

	for (i=0; i < db->length; ++i) {
		if (rDB_isEqual(key, hash, db->pairs[i])) {
			// delete key and value
			rDB_free (db->pairs[i].key);
			rDB_free (db->pairs[i].value);

			db->pairs[i].hash = 0; // set hash to zero

			return rDB_SUCCESS;
		}
	}

	return rDB_KEY_NOT_EXIST;
}

void rottDB_delall (rottDB *db) {
	size_t i;

	for (i=0; i < db->length; ++i) {
		// delete key and value
		rDB_free (db->pairs[i].key);
		rDB_free (db->pairs[i].value);

		db->pairs[i].hash = 0; // set hash to zero
	}
}

int rottDB_copy (const rottDB *source, const char *key, rottDB *destination, int overwrite) {
	int val;
	size_t i;
	const unsigned long hash = rDB_hash(key);

	if (overwrite == 0) { // Do NOT overwrite
		val = rottDB_exist (destination, key);
		if (val != rDB_KEY_NOT_EXIST) // Key exist
			return rDB_KEY_EXIST;
	}

	for (i=0; i < source->length; ++i) { // Search for pair
		if (rDB_isEqual(key, hash, source->pairs[i])) { // Found
			val = rottDB_set (destination, key, source->pairs[i].value); // Copy
			return val;
		}
	}

	return rDB_KEY_NOT_EXIST;
}

int rottDB_copyall (const rottDB *source, rottDB *destination, int overwrite) {
	int val;
	size_t i;
	rottDB tmp;

	if (overwrite == 0) { // Do NOT overwrite
		for (i=0; i < source->length; ++i) { // Check all keys for overwrite
			if (source->pairs[i].key == NULL) // NULL Check
				continue;

			val = rottDB_exist (destination, source->pairs[i].key);
			if (val != rDB_KEY_NOT_EXIST) // Key exist
				return rDB_KEY_EXIST;
		}
	}

	val = rottDB_init(&tmp);
	if (val < 0) // Check if rottDB_init failed
		return val;
	
	/* Clone destination to tmp */
	for (i=0; i < destination->length; ++i) {
		if (destination->pairs[i].key == NULL) // NULL Check
			continue;

		val = rottDB_set (&tmp, destination->pairs[i].key, destination->pairs[i].value);
		if (val < 0) { // Check if rottDB_set failed
			rottDB_shut (&tmp); // Shut temporary database
			return val;
		}
	}

	/* Copy all pairs from source to the temporary database */
	for (i=0; i < source->length; ++i) {
		if (source->pairs[i].key == NULL) // NULL Check
			continue;

		val = rottDB_set (&tmp, source->pairs[i].key, source->pairs[i].value);
		if (val < 0) { // Check if rottDB_set failed
			rottDB_shut (&tmp); // Shut temporary database
			return val;
		}
	}

	rottDB_shut (destination); // Shut destination

	/* destination = tmp */
	destination->length = tmp.length;
	destination->pairs = tmp.pairs;

	return rDB_SUCCESS;
}

int rottDB_move (rottDB *source, const char *key, rottDB *destination, int overwrite) {
	int val;

	/* rottDB_copy will check for overwrite, no need to do it here */
	val = rottDB_copy (source, key, destination, overwrite);
	if (val < 0) // If an error occured, return immidiately
		return val;

	rottDB_del (source, key);
	return val; // Return val even if there isn't an error, val is the return
				// code of rottDB_set call in rottDB_copy. If it isn't below
				// zero(which mean no error), then it is rDB_ADDED or
				// rDB_UPDATED.
}

int rottDB_moveall (rottDB *source, rottDB *destination, int overwrite) {
	int val;

	val = rottDB_copyall (source, destination, overwrite);
	if (val < 0)
		return val;

	rottDB_delall (source);

	return rDB_SUCCESS;
}

int rottDB_save (const rottDB *db, FILE *fp) {
	size_t i, keyl, vall;
	size_t sign = rDB_SIGN;
	
	fwrite (&sign, sizeof(size_t), 1, fp); // write signature

	for (i=0; i < db->length; ++i) {
		if (db->pairs[i].key == NULL)
			continue;

		keyl = rDB_size (db->pairs[i].key);
		vall = rDB_size (db->pairs[i].value);

		fwrite (&keyl, sizeof(size_t), 1, fp);
		fwrite (db->pairs[i].key, sizeof(char), keyl, fp);

		fwrite (&vall, sizeof(size_t), 1, fp);
		fwrite (db->pairs[i].value, sizeof(char), vall, fp);

		if (ferror(fp))
			return rDB_ERROR;
	}

	return rDB_SUCCESS;
}

int rottDB_load (rottDB *db, FILE *fp) {
	int ret;
	size_t i, keyl, vall;
	size_t sign;
	char *key = NULL, *value = NULL;

	fread (&sign, sizeof(size_t), 1, fp); // read signature
	if (sign != rDB_SIGN)
		return rDB_ERROR;

	ret = rottDB_init (db);
	if (ret < 0) {
		return ret;
	}

	for (i=0 ;; ++i) {
		/* Read Key */
		fread (&keyl, sizeof(size_t), 1, fp); // Read key size (inc. '\0')
		
		key = malloc (keyl * sizeof(char));
		if (key == NULL)
			return rDB_NO_MEMORY;

		fread (key, sizeof(char), keyl, fp); // Read key

		/* Read Value */
		fread (&vall, sizeof(size_t), 1, fp); // Read value size (inc. '\0')

		value = malloc (vall * sizeof(char));
		if (value == NULL) {
			rDB_free (key); // Do not forget that we allocated it early
			return rDB_NO_MEMORY;
		}

		fread (value, sizeof(char), vall, fp); // Read value

		/* Check if end of file */
		if (feof(fp))
			break;
		
		/* Check if an reading error happened */
		if (ferror(fp)) {
			rDB_free (key);
			rDB_free (value);
			
			return rDB_ERROR;
		}

		/* Set Pair */
		ret = rottDB_set (db, key, value);
		if (ret < 0) {
			rDB_free (key);
			rDB_free (value);

			rottDB_shut (db); // free database too

			return ret;
		}

		rDB_free (key);
		rDB_free (value);
	}

	rDB_free (key);
	rDB_free (value);

	return rDB_SUCCESS;
}

int rottDB_exist (const rottDB *db, const char *key) {
	int i;
	unsigned long hash = rDB_hash(key);

	for (i=0; i < db->length; ++i) {
		if (rDB_isEqual(key, hash, db->pairs[i])) // Found
			return rDB_SUCCESS;
	}

	return rDB_KEY_NOT_EXIST;
}

unsigned long rDB_hash (const char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
} // djb2

size_t rDB_size (const char *str) {
	return strlen(str) + 1;
}
