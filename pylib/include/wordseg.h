/***********************************************************************************
 * Describe : chinese word segment lib,only Max Forward Match Algorithm is used,
 *          : NO xiaoqi, NO named entity
 *          : hash table is used to hold the dictionary.
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Update   : 2008-10-15
 **********************************************************************************/
#ifndef _WORDSEG_H
#define _WORDSEG_H

#include <pdict.h>

// macros defined here
//
#define WS_MAX_WORDLEN   256
#define WS_MAX_GBK_RANGE 65535


// data struct defined here
//

enum WS_LEMMA_TYPE{
	WS_STATIC=1,  // static lemma, in dictionary
	WS_DYNAMIC,   // dynamic lemma, strings and numbers
};


// lemma is a structure of a word and its properies
typedef struct _ws_lemma{
	const char*   pstr;       // string pointer
	unsigned      weight:16;  // lemma weight, just for extension
	unsigned      len:8;      // lemma string length
	WS_LEMMA_TYPE type:8;     // lemma type
}WS_LEMMA;


// structure to hold the word segment result
typedef struct _ws_result{
	WS_LEMMA**    lemma_array;       // result lemma pointer array, 
	unsigned int  lemma_array_size;
	unsigned int  lemma_num;

	char*         strbuf;            // dynamic lemma string buffer
	unsigned int  strbuf_size;
	unsigned int  strbuf_pos;
	
	WS_LEMMA*     tmplemma_array;    // daynamic lemma array
	unsigned int  tmplemma_array_size;
	unsigned int  tmplemma_num;
}WS_RESULT;

typedef struct _ws_dict{
	char*        strbuf;             // word string buffer
	unsigned int strbuf_size;
	unsigned int strbuf_pos;

	WS_LEMMA*      lemma_array;      // lemma array
	unsigned int   lemma_array_size;
	unsigned int   lemma_array_pos;
	PDICT*         lemma_dict;       // word hash table

	// first word max length index
	unsigned char  head_symbol_index[WS_MAX_GBK_RANGE];
}WS_DICT;


// functions defined here
//

/*
 * func : load dictionary
 *
 * args : path, dict path
 *      : fname, dict file name
 *
 * ret  : NULL, failed;
 *      : else, pointer to the dict.
 */
WS_DICT* ws_dict_load(const char* path, const char* fname);

/*
 * func : free a WS_DICT struct
 *
 * args : pdict, pointer to the struct
 *
 * ret  :
 */
void     ws_dict_free(WS_DICT* pdict);

/*
 * func : segment string into words
 *
 * args : pdict, pointer to WS_DICT
 *      : inbuf, inbuf_len, input buffer and its length
 *      : result, nsize, result buffer and its size
 *
 * ret  : <0,   error;
 *      : else, word number
 *
 * note : in the result buffer, words are splited by single blank. 
 *      : traditional interface, NOT recommened.
 */
int      ws_seg(WS_DICT* pdict, const char* buff, const int buffsize, char* result, const int nsize);

/*
 * func : segment string into lemmas
 *
 * args : pdict, pointer to WS_DICT
 *      : inbuf, inbuf_len, input buffer and its length
 *      : result, pointer to a WS_RESULT struct, to hold the result lemma
 *
 * ret  : 0, succeed; 
 *      :<0, error
 *
 * note : word number is stored in result->lemma_num
 */
int      ws_seg(WS_DICT* pdict, const char* buff, const int buffsize, WS_RESULT* result);

/*
* func : create a struct to hold the word segment result
*
* args : max_lemma_num, the maximum lemma that the result can hold
*
* ret  : NULL, failed;
*      : else, pointer to the struct
*/
WS_RESULT* ws_result_create(int max_lemma_num);

/*
 * func : free a WS_RESULT struct
 */
void       ws_result_free(WS_RESULT* result);

/*
 * func : get a symbol from GBK string
 *
 * args : inbuf, inbuf_len, input buffer and its length
 *      : pos, current position in inbuf
 *
 * ret  : symbol value
 *
 * note : GBK symbol is composed by 1 or 2 bytes
 */

unsigned short get_symbol(const char *inbuf, const int inbuf_len, int* pos);

#endif
