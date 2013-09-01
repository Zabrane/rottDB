rottDB
======

What is rottDB?
---------------
* rottDB is a key-value store which:
	* aims to be easy to use
	* aims to be safe (if something fails your data stays untouched)
	* aims to be clean

* rottDB stores everythig in memory.
	* But you can save it to disk too. See `rottDB_save` and `rottDB_load`.

* rottDB uses linear search to locate key-value pairs.
	* It uses a hashing algorithm(dbj2) to hash the keys. When searchimg, first
	rottDB will be search for the same hash. When the both hashes are same, it
	will compare two keys using `strcmp`.

Usage
-----
See [rottDB Programming](https://github.com/boraalper4/rottDB/blob/master/docs/rottDB_Programming.md) and [rottDB.h](https://github.com/boraalper4/rottDB/blob/master/src/rottDB.h).

Dependencies
------------
No dependencies.

License
-------
[Simplified BSD License (The BSD 2-Clause License)](https://github.com/boraalper4/rottDB/blob/master/LICENSE)
