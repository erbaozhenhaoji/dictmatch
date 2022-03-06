/***********************************************************************************
 * Descri   : index中所用到的term cache, 无换入换出，大value连续存储
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2010-09-28
 *
 * Update   : 2010-09-28
 **********************************************************************************/
#ifndef PY_TERM_CACHE_H
#define PY_TERM_CACHE_H
#include <pdict.h>
#include <pthread.h>

typedef struct _term_cache{
	PDICT*           key_dict;
	void*            buff;
	unsigned int     buff_pos;
	unsigned int     buff_size;
	pthread_mutex_t  mutex;
}term_cache_t;


/*
 * func : create a term_cache_t struct
 *
 * args : max_key_num, 
 *      : bufsize, 
 *
 * ret  : NULL, failed
 *      : pointer to the struct
 */
term_cache_t* term_cache_create(int max_key_num, int bufsize);

/*
 * func : free a term_cache_t struct
 *
 * args : pcache, struct pointer
 */
void term_cache_free(term_cache_t* pcache);

/*
 * func : store the term_cache to disk
 *
 * args : pcache, the cache pointer
 *      : path, file, disk file path and name
 *
 * ret  : 0, succeed
 *      : else, failed.
 */
int term_cache_save(term_cache_t* pcache, const char* path, const char* file);

/*
 * func : load term cache from disk
 *
 * args : path, file, term cache disk file path
 *
 * ret  : NULL, load failed.
 *      : else, pointer to the term cache struct
 */
term_cache_t* term_cache_load(const char* path, const char* file);

/*
 * func : add an element to term cache
 *
 * args : pcache, cache pointer
 *      : key, keylen, key and its length
 *      : val, vallen, value and its length
 *
 * ret  : 0, succeed
 *      : else, failed.
 */
int term_cache_add(term_cache_t* pcache, const char* key, int keylen, const void* val, int vallen);

/*
 * func : delete an element from term_cache
 *
 * args : pcache, cache pointer
 *      : key, keylen, key and its length
 *
 * ret  : 0, find the key and deleted
 *      : 1, key NOT found
 *      : else, failed.
 */
int term_cache_del(term_cache_t* pcache, const char* key, int keylen);


/*
 * func : get an element from term_cache
 *
 * args : pcache, cache pointer
 *      : key, keylen, key and its length
 *      : val, vallen, value pointer and its length
 *
 * ret  : 1, succeed
 *      : 0, key NOT found
 */
int term_cache_get(term_cache_t* pcache, const char* key, int keylen, void** val, int* vallen);


#endif
