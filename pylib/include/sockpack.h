/********************************************************************************
 * Descri : a text data transfer protocol wrapper. this protocol is somehow 
 *        : like HTTP. dates can by divied into two main part: 
 *        :   1. header items, a group of key-->value pairs, like HTTP headers
 *        :   2. body buffers, a big buffer to hold long data, like HTTP page
 *
 * Author : Paul Yang, zhenhaoji@gmail.com
 *
 * Create : 2008-10-15
 *
 * Update : 2008-10-15
 ********************************************************************************/
#ifndef SOCKPACK_H
#define SOCKPACK_H

// macros defined here
//
#define MAX_PACK_WORDLEN 256

// data type defined here
//
typedef struct _pitem{
	char key[MAX_PACK_WORDLEN];
	char val[MAX_PACK_WORDLEN];
}PITEM;

typedef struct _sockpack{
	PITEM*        item_array;
	unsigned int  item_array_size;
	unsigned int  item_num;
	
	char*         body;
	unsigned int  body_size;

}SOCKPACK;

/*
 * func : create SOCKPACK struct
 *
 * args : max_item_num, max item num
 *      : body_size, body buffer size;
 * 
 * ret  : NULL, failed; 
 *      : else, pointer of the struct
 */
SOCKPACK* sockpack_create(unsigned int max_item_num);

/*
 * func : free a SOCKBUFF
 */
void      sockpack_free(SOCKPACK* sockpack);

/*
 * func : reset item buffer
 */
void      sockpack_reset(SOCKPACK* sockpack);

/*
 * func : send data via proxyio
 *
 * args : spack, the SOCKPACK pointer
 *      : pg, proxyio handle
 *      : timeout, time out
 * ret  : 0, succeed;
 *      : else, failed;
 */
int       sockpack_send(SOCKPACK* sockpack, int sock, int timeout);

/*
 * func : recv data via proxyio
 *
 * args : spack, the SOCKPACK pointer
 *      : pg, proxyio handle
 *      : timeout, time out
 * ret  : >=0, succeed, body lenght
 *      : else, failed;
 */
int       sockpack_recv(SOCKPACK* sockpack, int sock, int timeout);

/*
 * func : add a key<-->value paire to SOCKPACK
 *
 * args : key, value, the key<-->value pair
 *
 * ret  : 0,  succeed
 *      : -1, error
 */
int       sockpack_additem(SOCKPACK* sockpack, const char* key, const char* value);

/*
 * func : get the value of a specified key from SOCKPACK
 *
 * args : ppack, pointer to SOCKPACK
 *      : key, the input key
 *      : value, result value of the key
 *      : value_size, value buffer size
 *
 * ret  : 1, key fouded
 *      : 0, key NOT found
 */
int       sockpack_getitem(SOCKPACK* sockpack, const char* key, char* value, const int value_size);


/*
 * func : set body buffer to SOCKPACK
 *
 * args : body, bodylen, the input body buffer and its length
 */
void      sockpack_setbody(SOCKPACK* ppack, char* body, const int bodysize);

/*
 * func : get body buffer and its length
 *
 * args : body_len, hold the body buffer length
 *
 * ret  : pointer to body buffer 
 */
char*     sockpack_getbody(SOCKPACK* ppack, int* body_len);


#endif
