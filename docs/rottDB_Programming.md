# rottDB Programming

## What is rottDB?
* rottDB is an key-value store which:
	* aims to be easy to use: no need to make things complicated
	* aims to be safe: if something fails, your data stays untouched.
	* aims to be simple: code is written for humans, not machines.

* rottDB stores everythig in memory.
	* But you can save it to disk too. See `rottDB_save` and `rottDB_load`.

* rottDB uses linear search to locate key-value pairs.
	* It uses a hashing algorithm(dbj2) to hash the keys. When searchimg, first
	rottDB will be search for the same hash. When the both hashes are same, it
	will compare two keys using `strcmp`.

## Starting rottDB Programming
Actually, you don't even need to read this tutorial to use rottDB. You can
just look at the header file and you will probably understand the basics. So
there is no need to write a "simple" document to start using it.

This document is written to understand what's under the hood; **this is not a
tutorial**.

### Data Stuctures
> ...git actually has a simple design, with stable and reasonably
> well-documented data structures. In fact, I'm a huge proponent of designing
> your code around the data, rather than the other way around, and I think it's
> one of the reasons git has been fairly successful […] I will, in fact, claim
> that the difference between a bad programmer and a good one is whether he
> considers his code or his data structures more important. Bad programmers
> worry about the code. Good programmers **worry about data structures and
> their relationships**.
>
> Linus Torvalds on (git mailing list)[http://lwn.net/Articles/193245/]

In a header file far far away:

	typedef struct {
		unsigned long hash;
		char *key, *value;
	} rottDBpair;

	typedef struct {
		size_t length;
		rottDBpair *pairs;
	} rottDB;

I think it's clear enough. But be careful about that the `length` is the count
of the pairs in `pairs`. Not clear? Okay:

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

See? `db->length = 1`. There is just one item in array and the `length` is one
too.

### rottDB_set
Congratulations! You just finished %50 of the tutorial; and the rest %50 is
this part. Other functions are not that important; they are just simple little
functions.

rottDB_set work in this order:
* Make a one pass:
	* If there is NULL position in the `pairs` array, save it's index.
	* If you found the key, update it's value and return `rDB_UPDATED`.
* Check if there is a NULL position:
	* Use saved index to access NULL position and store the pair here.
* Key not exist and there isn't a NULL position:
	* `realloc` the `pairs` array and use the new position.

#### Talk is cheap, show me code:

	int rottDB_set (rottDB *db, const char *key, const char *value) {
		size_t i, null_pos = db->length;
		const unsigned long hash = rDB_hash(key);

As it name says, `null_pos` stores the index of the NULL position. Maybe
you are confused about why it's initial value is `db->length`. It's because
an index can never equal to the `db->length`. If there is 2 items in the
array, `db->length` will be 3; for 3 items in the array, `db->length` is 4;
and it goes like this.

##### Make one pass

	for (i=0; i < db->length; ++i) {

See: it's not `i <= db->length`; it's `i < db->length`.

###### If there is a NULL position in the `pairs` array, save it's index

		/* If you find an empty area, store it's position at null_pos. If key
		 * not exist, use this position to set pair.
		 */
		if (db->pairs[i].key == NULL) { // Found a null position
			null_pos = i; // save it

			continue;
		}

When you a pair is deleted; it's `hash` is 0 and `key` & `value` is NULL.
For more info, look at the source of `rottDB_del` and `rDB_free`.

###### If you found the key, update it's value and return `rDB_UPDATED`

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

`rDB_isEqual` is just a simple macro. It will compare hashes, then will call
`strcmp` to compare strings. Remember that, **rottDB is not a hash table**.

We said that rottDB aims to be safe. That's why we use `tptr`. Before freeing
`db->pairs[i].value`, we are allocating `tptr` to be sure. So if malloc fails,
your data stays untouched.

##### Check if there is a NULL position

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

##### Key not exist and there isn't a NULL position

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

## Author
**Bora M. Alper** <boraalper4 at gmail dot com>

Please do not hesitate; you can ask/say/recommend anything about rottDB. Have a good day!

