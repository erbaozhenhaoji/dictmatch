/***********************************************************************************
 * Describe : a simple hash table, interger key, interger value
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Modify   : 2008-10-15
 *
 **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <py_sign.h>
#include <py_utils.h>
#include <py_log.h>
#include <intmap.h>


// macros defined here
//
#define BLOCK_STEP   50000
#define COMMON_NULL  0xFFFFFFFE

/*
 * func : create an INTMAP struct
 *
 * args : hashsize, the hash table size
 *
 * ret  : NULL, error; else pointer the the INTMAP struct
 */ 
INTMAP*  intmap_create(const int hashsize, const int block_size)
{
	INTMAP*         pdict = NULL;
	INTNODE*        block = NULL;
	unsigned int*   hashtab = NULL;

	// create the struct
	pdict = (INTMAP*)calloc(1, sizeof(INTMAP));
	if(!pdict){
		goto failed;
	}
	
	// create hash table
	hashtab = (unsigned int*)calloc(hashsize, sizeof(unsigned int));
	if(!hashtab){
		goto failed;
	}
	for(int i=0;i<hashsize;i++){
		hashtab[i] = COMMON_NULL;
	}

	block = (INTNODE*)calloc(block_size, sizeof(INTNODE));
	if(!block){
		goto failed;
	}
	
	pdict->hashtab      = hashtab;
	pdict->hashsize     = hashsize;
	pdict->block        = block;
	pdict->block_size   = block_size;
	pdict->block_pos    = 0;

	return pdict;

failed:
	if(pdict){
		free(pdict);
		pdict = NULL;
	}
	if(hashtab){
		free(hashtab);
		hashtab = NULL;
	}
	if(block){
		free(block);
		block = NULL;
	}
	return NULL;
}

/*
 * func : free a INTMAP struct
 */
void intmap_free(INTMAP* pdict)
{
	if(!pdict){
		return;
	}
	if(pdict->hashtab){
		free(pdict->hashtab);
		pdict->hashtab=NULL;
	}
	if(pdict->block){
		free(pdict->block);
		pdict->block = NULL;
	}
	free(pdict);
	pdict = NULL;
}

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
int intmap_count(INTMAP* pdict, unsigned int key)
{
	unsigned int   pos        = 0;
	unsigned int   hashsize   = 0;
	unsigned int*  hashtab    = NULL;
	INTNODE*         curnode    = NULL;

	hashtab  = pdict->hashtab;
	hashsize = pdict->hashsize;
	pos = key % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos  = pdict->block_pos;
		if(block_pos==block_size){ // if block array is full, realloc block array
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}

		curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = 1;
		curnode->next  = COMMON_NULL;
		hashtab[pos]   = block_pos; 

		block_pos++;
		pdict->block_pos = block_pos;
		return 0;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		INTNODE*     pnode   = pdict->block+nodepos;
		while((unsigned int)pnode->next!=COMMON_NULL){
			if(pnode->key==key) {
				break;
			}
			pnode     = pdict->block+pnode->next;
		}
		if(pnode->key == key){ // find same key node
			pnode->value += 1;
			return 0;
		}

		// can not find same key node, add a new node
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos = pdict->block_pos;
		if(block_pos==block_size){
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}
		
		INTNODE* curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = 1;
		curnode->next  = hashtab[pos];  // front insert
		hashtab[pos] = block_pos;

		block_pos++;
		pdict->block_pos = block_pos;
		return 1;
	}

}

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
int intmap_accumulate(INTMAP* pdict, unsigned int key, const int value)
{
	unsigned int   pos        = 0;
	unsigned int   hashsize   = 0;
	unsigned int*  hashtab    = NULL;
	INTNODE*         curnode    = NULL;

	hashtab  = pdict->hashtab;
	hashsize = pdict->hashsize;
	pos = key % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos  = pdict->block_pos;
		if(block_pos==block_size){ // if block array is full, realloc block array
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}

		curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = value;
		curnode->next  = COMMON_NULL;
		hashtab[pos]   = block_pos; 

		block_pos++;
		pdict->block_pos = block_pos;
		return 0;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		INTNODE*     pnode   = pdict->block+nodepos;
		while((unsigned int)pnode->next!=COMMON_NULL){
			if(pnode->key==key) {
				break;
			}
			pnode     = pdict->block+pnode->next;
		}
		if(pnode->key == key){ // find same key node
			pnode->value += value;
			return 0;
		}

		// can not find same key node, add a new node
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos = pdict->block_pos;
		if(block_pos==block_size){
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}
		
		INTNODE* curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = value;
		curnode->next  = hashtab[pos];  // front insert
		hashtab[pos] = block_pos;

		block_pos++;
		pdict->block_pos = block_pos;
		return 1;
	}

}


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
int intmap_add(INTMAP* pdict, unsigned int key, const int value)
{
	unsigned int   pos        = 0;
	unsigned int   hashsize   = 0;
	unsigned int*  hashtab    = NULL;
	INTNODE*         curnode    = NULL;

	hashtab  = pdict->hashtab;
	hashsize = pdict->hashsize;
	pos = key % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos  = pdict->block_pos;
		if(block_pos==block_size){ // if block array is full, realloc block array
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}

		curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = value;
		curnode->next  = COMMON_NULL;
		hashtab[pos]   = block_pos; 

		block_pos++;
		pdict->block_pos = block_pos;
		return 0;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		INTNODE*     pnode   = pdict->block+nodepos;
		while((unsigned int)pnode->next!=COMMON_NULL){
			if(pnode->key==key) {
				break;
			}
			pnode     = pdict->block+pnode->next;
		}
		if(pnode->key == key){ // find same key node
			pnode->value = value;
			return 0;
		}

		// can not find same key node, add a new node
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos = pdict->block_pos;
		if(block_pos==block_size){
			INTNODE* block = pdict->block;
			block = (INTNODE*)realloc(block, sizeof(INTNODE)*(block_size+BLOCK_STEP));
			if(!block){
				py_log_write(PY_LOG_FATAL, "realloc block failed: %d", block_size+BLOCK_STEP);
				return -1;
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}
		
		INTNODE* curnode = pdict->block+block_pos;
		curnode->key   = key;
		curnode->value = value;
		curnode->next  = hashtab[pos];  // front insert
		hashtab[pos] = block_pos;

		block_pos++;
		pdict->block_pos = block_pos;
		return 1;
	}

}

/*
 * func : reset the hash table;
 */
void intmap_reset(INTMAP* pdict)
{
	assert(pdict);

	for(unsigned int i=0;i<pdict->hashsize;i++){
		pdict->hashtab[i] = COMMON_NULL;
	}
	pdict->block_pos = 0;
}


/*
 * func : find in the hash table
 *
 * args : pdict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int intmap_find(INTMAP* pdict, const int key, int* value)
{
	INTNODE* pnode = NULL;

	pnode = intmap_find(pdict, key);
	if(pnode==NULL){
		return 0;
	}else{
		*value = pnode->value;
		return 1;
	}
}

/*
 * func : find int the hash table, return the founed node
 *
 * args : pdict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
INTNODE* intmap_find(INTMAP* pdict, unsigned int key)
{
	unsigned int   hashsize   = 0;
	unsigned int   pos        = 0;
	unsigned int*  hashtab    = NULL;
	INTNODE*         pnode      = NULL;

	hashtab    = pdict->hashtab;
	hashsize   = pdict->hashsize;
	pos        = key % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		return NULL;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		pnode = pdict->block+nodepos;
		while(pnode->next!=COMMON_NULL){
			if(pnode->key == key){
				break;
			}
			pnode = pdict->block + pnode->next;
		}
		if(pnode->key==key){ // find same key node
			return pnode;
		}

		return NULL;

	}
	
}

