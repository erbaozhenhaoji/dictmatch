/********************************************************************************
 * Describe : a string hash table, with integer values. 
 *          : surpport 'ADD', 'DELETE', 'MODIFY', 'SAVE', 'LOAD' operation
 *          : surpport dynamic increse 
 *          : using 64 bit signature to stand for string, witch increase speed and
 *          : decrease memory usage.
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Modify   : 2008-10-15
 *******************************************************************************/
#ifndef _py_dict_t_H
#define _py_dict_t_H

#include <py_sign.h>


// macros defined here
//
#define COMMON_NULL 0xFFFFFFFE


// data structure define here
//
typedef struct _pnode{
	unsigned int sign1;
	unsigned int sign2;
	int          code;
	int          value;
	unsigned int next;
}PNODE;


typedef struct _int_dict{
	unsigned int*     hashtab;
	unsigned int      hashsize;

	PNODE*            block;
	unsigned int      block_pos;
	unsigned int      block_size;
}py_dict_t;


// functions defined here
//

/*
 * func : create an py_dict_t struct
 *
 * args : hashsize, the hash table size
 *      : nodesize, the node array size
 *
 * ret  : NULL, error;
 *      : else, pointer the the py_dict_t struct
 */ 
py_dict_t*   pydict_create(const int hashsize, const int nodesize);


/*
 * func : load py_dict_t from disk file
 *
 * args : path, file
 *
 * ret  : NULL, error
 *      : else, pointer to py_dict_t struct
 */
py_dict_t*   pydict_load(const char* path, const char* file);



/*
 * func : load py_dict_t from disk file
 *
 * args : full_path
 *
 * ret  : NULL, error
 *      : else, pointer to py_dict_t struct
 */
py_dict_t*   pydict_load_fullpath(const char* full_path);



/*
 * func : free a py_dict_t struct
 */
void     pydict_free(py_dict_t* pydict);

/*
 * func : reset the hash table;
 */
void     pydict_reset(py_dict_t* pydict);

/*
 * func : save py_dict_t to disk file
 *
 * args : pydict, the py_dict_t pointer 
 *      : path, file, dest path and file
 *
 * ret  : 0, succeed; 
 *        -1, error.
 */
int      pydict_save(py_dict_t* pydict, const char* path, const char* file);

/*
 * func : add a value pair to the hash table;
 * 
 * args : pydict, the pointer to py_dict_t 
 *      : key, value, the input pair
 *
 * ret  : 1,  find a same key, value changed;
 *      : 0,  find NO same key, new node added,
 *      : -1, error;
 */
int      pydict_add(py_dict_t* pydict, const char* key, const int len, const int code, const int value);

/*
 * func : add a node to the hash table;
 * 
 * args : pydict, pointer to py_dict_t 
 *      : node , pointer to input node
 *
 * ret  : 1,  find a same key, value changed;
 *      : 0,  find NO same key, new node added,
 *      : -1, error;
 */
int      pydict_add_node(py_dict_t* pydict, PNODE* node);

/*
 * func : delete a node in the hash table
 *
 * args : pydict, pointer to hash table;
 *      : key, the tobe delete node key
 *
 * ret  : 0, NOT found; 1 founded.
 *
 * node : just mark delete, set pnode->code to -1 mean delete.
 */
int      pydict_del(py_dict_t* pydict, const char* key, const int len);

/*
 * func : find in the hash table
 *
 * args : pydict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int      pydict_find(py_dict_t* pydict, const char* key, const int len, int* code, int* value);

/*
 * func : find node in hash table by signature
 *
 * args : pydict, pointer to hash table
 *      : sign,  64 bit string signature
 *
 * ret  : NULL, not found
 *      : else, pointer to the founded node
 */
PNODE*   pydict_find_node(py_dict_t* pydict, SIGN64* sign);

/*
 * func : find int the hash table, return the founed node
 *
 * args : pydict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
PNODE*   pydict_find_node_str(py_dict_t* pydict, const char* key, const int len);

/*
 * func : get the first node in hash table
 *
 * args : pos, return the position of the first node
 *
 * ret  : NULL, can NOT get first node, hash table empty
 *      : else, pointer to the first node
 */
PNODE*   pydict_first(py_dict_t* pydict, unsigned int* pos);

/*
 * func : get next node from the hash table
 *
 * args : pos, the start position, get node after pos, 
 *
 * ret  : NULL, can NOT get next NODE, reach the end.
 *      : else, pointer to the next NODE
 */
PNODE*   pydict_next(py_dict_t* pydict, int* pos);

#endif
