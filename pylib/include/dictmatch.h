/***********************************************************************************
 * Describe : 多模式匹配库，在一段文本中提取中满足某种条件的特征词。
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Update   : 2008-10-15
 **********************************************************************************/
#ifndef _WORDSEG_H
#define _WORDSEG_H

#include "pdict.h"


// macros defined here
//
#define DM_MAX_WORDLEN   256
#define DM_HEAD_SYMBOL_RANGE 65535



// data struct defined here
//

// lemma is a structure of a word and its properies
typedef struct _dm_lemma{
	const char*       pstr;       // string pointer
	unsigned int      len;      // lemma string length
	unsigned int      prop;  // lemma property
}DM_LEMMA;


// structure to hold the word segment result
typedef struct _dm_result{
	DM_LEMMA**    lemma_array;       // result lemma pointer array, 
	unsigned int  lemma_array_size;
	unsigned int  lemma_num;
	
	unsigned int* offset_array;
	unsigned int  offset_array_size;
	unsigned int  offset_num;
	
}DM_RESULT;

typedef struct _dm_dict{
	char*          strbuf;             // word string buffer
	unsigned int   strbuf_size;
	unsigned int   strbuf_pos;

	DM_LEMMA*      lemma_array;      // lemma array
	unsigned int   lemma_array_size;
	unsigned int   lemma_array_pos;
	PDICT*         lemma_dict;       // word hash table

	// first word max length index
	unsigned char  head_symbol_index[DM_HEAD_SYMBOL_RANGE];
}DM_DICT;


// functions defined here
//

/*
 * func : 载入特征词提取词典
 *
 * args : path, 词典文件路径
 *      : fname, 词典文件名
 *
 * ret  : NULL, 出错
 *      : else, 指向词典结构的指针
 */
DM_DICT* dm_dict_load(const char* path, const char* fname, int(*line_func)(const char* line, char* word));

/*
 * func : 载入特征词提取词典
 *
 * args : path, 词典文件路径
 *      : fname, 词典文件名
 *
 * ret  : NULL, 出错
 *      : else, 指向词典结构的指针
 */
DM_DICT* dm_dict_load(const char* fullpath, int(*line_func)(const char* line, char* word));

/*
 * func : free a DM_DICT struct
 *
 * args : pdict, pointer to the struct
 *
 * ret  :
 */
void     dm_dict_free(DM_DICT* pdict);


/*
 * func : 在一段文本中提取特征词
 *
 * args : pdict, 词典结构指针
 *      : buf, buflen, 输入文本及其长度 
 *      : res,  特征词提取结果
 *
 * ret  : 0, 成功 
 *      : else, 失败
 */
int      dm_seg(DM_DICT* pdict, const char* buf, int buflen, DM_RESULT* res);

/*
 * func : 查找一段文本中是否含有某种指定类型的特征词
 *
 * args : pdict, 辞典结构指针
 *      : buf, buflen, 输入文本及其长度
 *      : res, 存储特征词匹配的结果
 *      : type, 查找的类型
 *
 * ret  : true, 文本包含这种类型的特征词
 *      : false, 文本不包含这种类型地特征词
 */
bool dm_has_type_word(DM_DICT* pdict,  DM_RESULT* res, const char* buf, int buflen,int type);

/*
* func : 生成一个存储特征词提取结果的结构
*
* args : max_lemma_num, 希望该结构最多存储多少个特征词
*
* ret  : NULL, 出错
*      : else, 指向新生成结构地指针
*/
DM_RESULT* dm_result_create(int max_lemma_num);

/*
 * func : 释放一个特征此提取结果结构
 */
void       dm_result_free(DM_RESULT* result);

/*
 * func : 向词典中增加一个特征词。
 * 
 * args : pdict, 指向词典结构的指针
 *      : word, 新特征词的字符串
 *      : wordlen, 新特征词长度
 *      : prop, 新特征词属性
 *
 * ret  : 0, 成功
 *      : else, 出错
 */
int dm_dict_add(DM_DICT* pdict, const char* word, int worlen, unsigned int prop);


#endif
