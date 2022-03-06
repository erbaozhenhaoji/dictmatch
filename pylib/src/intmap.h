/********************************************************************************
 * Describe : a simple hash table, interger key, interger value
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Modify   : 2008-10-15
 *
 ********************************************************************************/
#ifndef _INTMAP_H
#define _INTMAP_H


// datatypes defined here
//
typedef struct _intnode{
	unsigned int key;
	unsigned int next;
	unsigned short value;
	unsigned short type;
}INTNODE;


typedef struct _intmap{
	unsigned int*     hashtab;
	unsigned int      hashsize;
	INTNODE*          block;
	unsigned int      block_pos;
	unsigned int      block_size;
}INTMAP;


// functions defined here
//

/*
 * func : create an INTMAP struct
 *
 * args : hashsize, the hash table size
 *
 * ret  : NULL, error; else pointer the the INTMAP struct
 */ 
INTMAP* intmap_create(const int hashsize, const int blocknum);

/*
 * func : free a INTMAP struct
 */
void intmap_free(INTMAP* pdict);

/*
 * func : reset the hash table;
 */
void intmap_reset(INTMAP* pdict);

/*
 * func : add a node to the hash table;
 * 
 * args : pdict, pointer to INTMAP 
 *      : node , pointer to input node
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int intmap_add(INTMAP* pdict, unsigned int key, const int value);

/*
 * func : add a node to the hash table;
 * 
 * args : pdict, pointer to INTMAP 
 *      : node , pointer to input node
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int intmap_count(INTMAP* pdict, unsigned int key);

/*
 * func : add a node to the hash table;
 * 
 * args : pdict, pointer to INTMAP 
 *      : node , pointer to input node
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int intmap_accumulate(INTMAP* pdict, unsigned key, const int value);

/*
 * func : find in the hash table
 *
 * args : pdict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int intmap_find(INTMAP* pdict, unsigned int key, int* value);

/*
 * func : find int the hash table, return the founed node
 *
 * args : pdict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
INTNODE* intmap_find(INTMAP* pdict, unsigned int key);



#endif
