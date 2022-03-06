#ifndef _WORDSEG_H
#define _WORDSEG_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "py_code.h"


#define MAX_SYMBOL_VALUE 0x10000




// 词条定义
typedef struct _lemma{
        unsigned wordstr:24;
        unsigned wordlen:8;
        unsigned prop:32;
}lemma_t;

// 基于双数组的trie树定义
typedef struct _aytree{
        int* base;
        int* check;
        int* lmpos;
        int  array_size;
        int  max_state;
}aytree_t;

// 多模式匹配的词典定义
typedef struct _dm_dict{
        aytree_t* aytree;
        lemma_t* lemma_array;
        int      lemma_array_size;
        int      lemma_array_pos;
        char*   wordbuf;
        int     wordbuf_size;
        int     wordbuf_pos;
}dm_dict_t;

dm_dict_t* dm_dict_create(int array_size, int lemma_size, int wordbuf_size);
void       dm_dict_free(dm_dict_t* dict);
int        dm_dict_build(const char* rawfile, const char* binfile, int build_prop_func(const char* prop_str));
dm_dict_t* dm_dict_load(const char* filename);
int        dm_dict_save(dm_dict_t* dm_dict, const char* filename);
char*      dm_dict_get_lemma_string(dm_dict_t* dm_dict, lemma_t* lemma);



typedef struct _dm_result{
        lemma_t** lemma_array;
        int       lemma_array_size;
        int       lemma_num;
}dm_result_t;

dm_result_t* dm_result_create(int max_lemma_num);
void       dm_result_free(dm_result_t* result);


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
int      dm_match(dm_dict_t* pdict, const char* buf, int buflen, dm_result_t* res);


#endif
