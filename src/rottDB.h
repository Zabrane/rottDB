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
#ifndef ROTTDB_H
#define ROTTDB_H

/* ================================ Macros ================================= */
#define rDB_free(ptr) {free (ptr); ptr = NULL;}
#define rDB_isEqual(key, hash, pair) (hash == pair.hash &&					 \
					pair.key != NULL && strcmp(key, pair.key) == 0)

/* ----------------------------- rottDB SIGN ------------------------------ */
#define rDB_SIGN 0x74746f72

/* ----------------------------- Return Codes ------------------------------ */
/* Error Codes */
#define rDB_ERROR			-1
#define rDB_NO_MEMORY		-2
#define rDB_KEY_INVALID		-3
#define rDB_KEY_EXIST		-4
#define rDB_KEY_NOT_EXIST	-5


/* Success Codes */
#define rDB_SUCCESS	0
#define rDB_ADDED	1
#define rDB_UPDATED	2

/* ================================ Structs ================================ */
typedef struct {
	unsigned long hash;
	char *key, *value;
} rottDBpair;

typedef struct {
	size_t length;
	rottDBpair *pairs;
} rottDB;

/* ========================== Function Prototypes ========================== */
int rottDB_init (rottDB *db);
void rottDB_shut (rottDB *db);

int rottDB_set (rottDB *db, const char *key, const char *value);
char *rottDB_get (const rottDB *db, const char *key);

int rottDB_del (rottDB *db, const char *key);
void rottDB_delall (rottDB *db);

int rottDB_copy (const rottDB *source, const char *key, rottDB *destination,   \
				 int overwrite);
int rottDB_copyall (const rottDB *source, rottDB *destination, int overwrite);

int rottDB_move (rottDB *source, const char *key, rottDB *destination, \
				 int overwrite);
int rottDB_moveall (rottDB *source, rottDB *destination, int overwrite);

int rottDB_clone (const rottDB *source, rottDB *clone);

int rottDB_exist (const rottDB *db, const char *key);

int rottDB_save (const rottDB *db, FILE *fp);
int rottDB_load (rottDB *db, FILE *fp);


unsigned long rDB_hash (const char *str);
size_t rDB_size (const char *str);

#endif // #ifndef ROTTDB_H
