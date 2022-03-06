#include <py_code.h>
#include <assert.h>
#include "py_utils.h"
#include "dictmatch.h"
typedef struct _rawlemma{
        char wordstr[256];
        int  wordlen;
        int  prop;
}rawlemma_t;


typedef struct _rawnode{
        int       node_num;
        int       value;
        int       lemma_pos;
        struct _rawnode*   child;
        struct _rawnode*   next;
}rawnode_t;

typedef struct _rawtree{
        rawnode_t* root;
        rawlemma_t*     lemma_array;
        int lemma_array_size;
        int lemma_array_pos;

}rawtree_t;


rawtree_t* rawtree_create(int max_lemma_size);
void       rawtree_free(rawtree_t* tree);
int        rawtree_find(rawtree_t* tree, const char* word, int wordlen);
int        rawtree_addword(rawtree_t* tree, const char* word, int wordlen, int prop);
int        rawtree_build_dmdict(rawtree_t* tree, const char* dictname);


static aytree_t* aytree_create(int array_size);
static void aytree_free(aytree_t* aytree);
static int get_next_word(dm_dict_t* dm_dict, const char* src, int srclen, int* pos, lemma_t** lemma);
static void dm_result_reset(dm_result_t* pack);
static int dm_result_append(dm_result_t* pack, lemma_t* lemma);

dm_dict_t* dm_dict_create(int array_size, int lemma_size, int wordbuf_size)
{
        dm_dict_t* dm_dict     = NULL;
        aytree_t*  aytree      = NULL;
        lemma_t*   lemma_array = NULL;
        char*      wordbuf     = NULL;

        if((dm_dict=(dm_dict_t*)calloc(1, sizeof(dm_dict_t)))==NULL){
                goto failed;
        }
        if((aytree=aytree_create(array_size))==NULL){
                goto failed;
        }
        if((lemma_array=(lemma_t*)calloc(lemma_size, sizeof(lemma_t)))==NULL){
                goto failed;
        }
        if((wordbuf=(char*)malloc(wordbuf_size))==NULL){
                goto failed;
        }

        dm_dict->wordbuf      = wordbuf;
        dm_dict->wordbuf_size = wordbuf_size;
        dm_dict->aytree       = aytree;
        dm_dict->lemma_array  = lemma_array;
        dm_dict->lemma_array_size    = lemma_size;

        return dm_dict;

failed:
        if(dm_dict!=NULL){
                free(dm_dict);
                dm_dict = NULL;
        }
        if(aytree!=NULL){
                aytree_free(aytree);
                aytree = NULL;
        }
        if(lemma_array!=NULL){
                free(lemma_array);
                lemma_array = NULL;
        }
        return NULL;
}

void dm_dict_free(dm_dict_t* dm_dict)
{
        if(dm_dict == NULL){
                return;
        }

        if(dm_dict->aytree){
                aytree_free(dm_dict->aytree);
                dm_dict->aytree = NULL;
        }
        if(dm_dict->lemma_array){
                free(dm_dict->lemma_array);
                dm_dict->lemma_array = NULL;
        }
	if(dm_dict->wordbuf){
		free(dm_dict->wordbuf);
		dm_dict->wordbuf = NULL;
	}
        free(dm_dict);
        dm_dict = NULL;
        return;
}



aytree_t* aytree_create(int array_size)
{
        aytree_t* aytree   = NULL;
        int*      base     = NULL;
        int*      check    = NULL;
        int*      lmpos    = NULL;

        if((aytree=(aytree_t*)calloc(1, sizeof(aytree_t)))==NULL){
                goto failed;
        }
        if((base=(int*)calloc(array_size, sizeof(int)))==NULL){
                goto failed;
        }
        if((check=(int*)calloc(array_size, sizeof(int)))==NULL){
                goto failed;
        }
        if((lmpos=(int*)calloc(array_size, sizeof(int)))==NULL){
                goto failed;
        }
        for(int i=0;i<array_size;i++){
                base[i]  = -1;
                check[i] = -1;
                lmpos[i] = -1;
        }

        aytree->base       = base;
        aytree->check      = check;
        aytree->lmpos      = lmpos;
        aytree->array_size = array_size;
        return aytree;

failed:
        if(aytree!=NULL){
                free(aytree);
                aytree = NULL;
        }
        if(base!=NULL){
                free(base);
                base = NULL;
        }
        if(check!=NULL){
                free(check);
                check = NULL;
        }
        if(lmpos!=NULL){
                free(lmpos);
                lmpos = NULL;
        }
        return NULL;
}

void aytree_free(aytree_t* aytree)
{
        if(aytree==NULL){
                return;
        }
        if(aytree->base){
                free(aytree->base);
                aytree->base = NULL;
        }
        if(aytree->check){
                free(aytree->check);
                aytree->check = NULL;
        }
        if(aytree->lmpos){
                free(aytree->lmpos);
                aytree->lmpos = NULL;
        }
        free(aytree);
        aytree = NULL;
        return;
}


int dm_dict_save(dm_dict_t* dm_dict, const char* filename)
{
        FILE* fp        = NULL;
        unsigned int array_size  = 0;
        unsigned int lemma_num   = 0;
        unsigned int wordbuf_pos = 0;
        int* base       = NULL;
        int* check      = NULL;
        int* lmpos      = NULL;

        // open file for write
        if((fp=fopen(filename, "wb"))==NULL){
                return 0;
        }

        // write aytree size;
        // write lemma num;
        array_size = dm_dict->aytree->array_size + 1;
        if(fwrite(&array_size, sizeof(int), 1, fp)!=1){
                assert(0);
        }
        lemma_num  = dm_dict->lemma_array_pos + 1;
        if(fwrite(&lemma_num, sizeof(int), 1, fp)!=1){
                assert(0);
        }
        wordbuf_pos = dm_dict->wordbuf_pos;
        if(fwrite(&wordbuf_pos, sizeof(int), 1, fp)!=1){
                assert(0);
        }

        // write aytree;
        aytree_t* aytree = dm_dict->aytree;
        base = aytree->base;
        check = aytree->check;
        lmpos = aytree->lmpos;

        if(fwrite(base, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }
        if(fwrite(check, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }
        if(fwrite(lmpos, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }

        // write lemma array
        lemma_t* lemma_array = dm_dict->lemma_array;
        if(fwrite(lemma_array, sizeof(lemma_t), lemma_num, fp)!=lemma_num){
                assert(0);
        }

        // write wordbuf
        char* wordbuf     = dm_dict->wordbuf;
        if(fwrite(wordbuf, sizeof(char), wordbuf_pos,fp)!=wordbuf_pos){
                assert(0);
        }

        fclose(fp);


        return 0;
}


dm_dict_t* dm_dict_load(const char* filename)
{
        FILE* fp = NULL;
        dm_dict_t* dm_dict = NULL;

        // open file for write
        if((fp=fopen(filename, "rb"))==NULL){
                return 0;
        }

        // read aytree size;
        // read lemma num;
        unsigned int array_size = 0;
        unsigned int lemma_num  = 0;
        unsigned int wordbuf_size = 0;
        if(fread(&array_size, sizeof(int), 1, fp)!=1){
                assert(0);
        }
        if(fread(&lemma_num, sizeof(int), 1, fp)!=1){
                assert(0);
        }
        if(fread(&wordbuf_size, sizeof(int), 1, fp)!=1){
                assert(0);
        }
        
        // create dm_dict
        if((dm_dict=dm_dict_create(array_size+1, lemma_num+1, wordbuf_size))==NULL){
                assert(0);
        }

        dm_dict->aytree->array_size = array_size;
        dm_dict->lemma_array_size   = lemma_num;
        dm_dict->wordbuf_size = wordbuf_size;

        aytree_t* aytree = dm_dict->aytree;
        int* base = aytree->base;
        int* check = aytree->check;
        int* lmpos = aytree->lmpos;

        if(fread(base, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }
        if(fread(check, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }
        if(fread(lmpos, sizeof(int), array_size, fp)!=array_size){
                assert(0);
        }

        // read lemma array
        lemma_t* lemma_array = dm_dict->lemma_array;
        if(fread(lemma_array, sizeof(lemma_t), lemma_num, fp)!=lemma_num){
                assert(0);
        }

        // read wordbuf
        char* wordbuf = dm_dict->wordbuf;
        if(fread(wordbuf, sizeof(char), wordbuf_size, fp)!=wordbuf_size){
                assert(0);
        }



        fclose(fp);

        return dm_dict;

}


int dm_dict_find(dm_dict_t* dm_dict, const char* word, int word_len)
{
        int          pos      = 0;
        int          bval     = 0;
        int curstate = 0;
        int newstate = 0;
        unsigned short symbol = 0;
        aytree_t* aytree = NULL;
        int*      base   = NULL;
        int*      check  = NULL;
        int*      lmpos  = NULL;

        aytree = dm_dict->aytree;
        base   = aytree->base;
        check  = aytree->check;
        lmpos  = aytree->lmpos;


        curstate = 0;
        pos      = 0;
        symbol = get_utf8_word(word, word_len, &pos);
        while(symbol!=0){
                // search in the tree
                bval = base[curstate];
                newstate = bval + symbol;
                if(check[newstate] == curstate){
                        curstate = newstate;
                }
                else{
                        break;
                }

                // get next symbol
                symbol = get_utf8_word(word, word_len, &pos);
        }

        // symbl end and there is a lemma mark in the node
        lemma_t* lemma_array = dm_dict->lemma_array;
        if(symbol==0&&lmpos[curstate]!=-1){
                int lemma_pos = lmpos[curstate];
                lemma_t* plm = lemma_array + lemma_pos;
                return plm->prop;
        }
        else{
                return 0;
        }

}



int dm_match(dm_dict_t* dm_dict, const char* buf, int buflen, dm_result_t* result)
{
        int pos        = 0;
        int wordlen    = 0;
        lemma_t* lemma = NULL;


	dm_result_reset(result);

        // get next word
	lemma = NULL;
        wordlen = get_next_word(dm_dict, buf, buflen, &pos, &lemma);
        while(wordlen>0){
		if(lemma!=NULL){//copy lemma to result
			if(dm_result_append(result, lemma)!=0){
				assert(0);
			}
		}
                // get next word
		lemma = NULL;
                wordlen = get_next_word(dm_dict, buf, buflen, &pos, &lemma);

        }

        return 0;
}

        

int get_next_word(dm_dict_t* dm_dict, const char* src, int srclen, int* pos, lemma_t** lemma)
{
        int begin             = 0;
        int wordlen           = 0;
        unsigned short symbol = 0;
        aytree_t* aytree      = NULL;
        lemma_t*  lemma_array = NULL;

        begin = *pos;
        aytree = dm_dict->aytree;
        lemma_array = dm_dict->lemma_array;

        *lemma = NULL;

        if(*pos>srclen){
                assert(0);
        }
        if(*pos==srclen){
                return 0;
        }


        int* base  = aytree->base;
        int* check = aytree->check;
        int* lmpos = aytree->lmpos;

        symbol  = get_utf8_word(src, srclen, pos);
        wordlen = *pos - begin;
        int curstate = 0;
        while(symbol!=0){
                //find the node intree?
                int newstate = base[curstate]+symbol;
                if(check[newstate]==curstate){ //yes
                        curstate = newstate;
                        if((lmpos[newstate]!=-1)){
                                wordlen = *pos-begin;
                                *lemma = lemma_array+lmpos[newstate];
                        }
                        symbol = get_utf8_word(src, srclen, pos);
                }
                else{
                        break;
                }
        }

        // return the length of the longest word
        *pos = begin+wordlen;
        return wordlen;
}


dm_result_t* dm_result_create(int max_keyword_num)
{
	dm_result_t* result      = NULL;
	lemma_t**    lemma_array = NULL;

	result = (dm_result_t*)calloc(1, sizeof(dm_result_t));
	if(!result){
		goto failed;
	}

	lemma_array = (lemma_t**)calloc(max_keyword_num, sizeof(lemma_t*));
	if(!lemma_array){
		goto failed;
	}

	result->lemma_array      = lemma_array;
	result->lemma_array_size = max_keyword_num;

	return result;

failed:
	if(result!=NULL){
		free(result);
		result = NULL;
	}
	if(lemma_array!=NULL){
		free(lemma_array);
		lemma_array = NULL;
	}
	return NULL;
}

void dm_result_free(dm_result_t* result)
{
	if(result==NULL){
		return;
	}
	if(result->lemma_array!=NULL){
		free(result->lemma_array);
		result->lemma_array     = NULL;
	}
	result->lemma_array_size = 0;
	free(result);
	result = NULL;
	return;
}

void dm_result_reset(dm_result_t* pack)
{
	pack->lemma_num = 0;
}

int dm_result_append(dm_result_t* pack, lemma_t* lemma)
{
	int lemma_num         = 0;
	int lemma_array_size  = 0;
	lemma_t** lemma_array = NULL;

	lemma_num        = pack->lemma_num;
	lemma_array_size = pack->lemma_array_size;

	if(lemma_num+1 >= lemma_array_size){
		return -1;
	}

	lemma_array = pack->lemma_array;
	lemma_array[lemma_num] = lemma;
	pack->lemma_num++;

	return 0;
}



int  dm_dict_build(const char* raw_filename, const char* bin_filename, int build_prop_func(const char* prop_str))
{
	char buff[1024];
	int keyword_num     = 0;
	int line            =0;
	rawtree_t* rawtree  = NULL;

	if(py_fline(raw_filename, &keyword_num)!=0){
		assert(0);
	}

	rawtree = rawtree_create(keyword_num+1);
	assert(rawtree);


	FILE* fp= fopen(raw_filename, "rb");
	assert(fp);
	vector<string> vec;
	while(fgets(buff, sizeof(buff), fp)){
		line++;
		if(line%1000==0){
			fprintf(stderr, "processed %d\n", line);
		}
		const char* word = NULL;
		int  prop;
		split(buff, vec, "\t\r\n");
		if(vec.size()!=2){
			fprintf(stderr, "bad format%s\n", buff);
		}
		word = vec[0].c_str();
		prop = build_prop_func(vec[1].c_str());
		if(rawtree_addword(rawtree, word, strlen(word), prop)!=0){
			assert(0);
		}
	}
	fclose(fp);

	if(rawtree_build_dmdict(rawtree, bin_filename)!=0){
		assert(0);
	}

	rawtree_free(rawtree);


	return 0;
}

static void free_node(rawnode_t* node);
static int subtree_to_ay(rawnode_t* curnode, unsigned int curstate, aytree_t* aytree);
int rawtree_to_aytree(rawnode_t* root, aytree_t* aytree);
inline int get_base_value(int* child_value_array, int child_num, int* base, int* check, int array_size);


rawtree_t* rawtree_create(int lemma_array_size)
{
        rawtree_t* tree = NULL;
        rawnode_t* root = NULL;
        rawlemma_t*   lemma_array = NULL;

        if((tree=(rawtree_t*)calloc(1, sizeof(rawtree_t)))==NULL){
                goto failed;
        }
        if((root=(rawnode_t*)calloc(1, sizeof(rawnode_t)))==NULL){
                goto failed;
        }
        if((lemma_array=(rawlemma_t*)calloc(lemma_array_size, sizeof(rawlemma_t)))==NULL){
                goto failed;
        }

        tree->root             = root;
        tree->lemma_array      = lemma_array;
        tree->lemma_array_size = lemma_array_size;
        return tree;
failed:
        if(tree!=NULL){
                free(tree);
                tree = NULL;
        }
        if(root!=NULL){
                free(root);
                root = NULL;
        }
        if(lemma_array!=NULL){
                free(lemma_array);
                lemma_array = NULL;
        }
        return NULL;
}



void rawtree_free(rawtree_t* tree)
{
        if(tree==NULL){
                return;
        }
        if(tree->root!=NULL){
                free_node(tree->root);
                tree->root = NULL;
        }
        if(tree->lemma_array!=NULL){
                free(tree->lemma_array);
                tree->lemma_array = NULL;
        }

        free(tree);
        tree  = NULL;
}

void free_node(rawnode_t* node)
{
        if(node==NULL){
                return;
        }

        // free all child node
        rawnode_t* child = node->child;
        for(;child!=NULL; child = child->next){
                free_node(child);
        }

        // free it self
        free(node);
        node = NULL;
}

int  rawtree_find(rawtree_t* tree, const char* word, int wordlen)
{
        int pos                = 0;
        unsigned short symbol  = 0;
        rawnode_t*     curnode = NULL;

        curnode = tree->root;
        symbol = get_utf8_word(word, wordlen, &pos);
        while(symbol!=0){
                // find in tree?
                rawnode_t* child = NULL;
                for(child=curnode->child;child!=NULL;child=child->next){
                        if(child->value == symbol){
                                break;
                        }
                }
                if(child == NULL){
                        return 0;
                }
                else{
                        curnode = child;
                }

                // get next symbol from word
                symbol = get_utf8_word(word, wordlen, &pos);
        }
        
        // reach the end of word, and current node is a lemma node
        int lemma_pos   = curnode->lemma_pos;
        rawlemma_t*     lemma_array = tree->lemma_array;
        rawlemma_t*     plm         = NULL;
        if(symbol==0 && lemma_pos != -1){
                plm = lemma_array+lemma_pos;
        }

        return plm->prop;
}

int rawtree_addword(rawtree_t* tree, const char* word, int wordlen, int prop)
{
        int            pos     = 0;
        unsigned short symbol  = 0;
        rawnode_t*     curnode = NULL;

        curnode = tree->root;
        symbol = get_utf8_word(word, wordlen, &pos);
        while(symbol!=0){
                // can find in the current node?
                rawnode_t* child = curnode->child;
                for(;child!=NULL;child=child->next){
                        if(child->value == symbol){
                                break;
                        }
                }
                if(child==NULL){// can NOT found
                        // insert a child for curnode
                        rawnode_t* pnode = NULL;
                        pnode = (rawnode_t*)calloc(1, sizeof(rawnode_t));
                        assert(pnode);
                        pnode->value     = symbol;
                        pnode->lemma_pos = -1;
                        pnode->next = curnode->child;
                        curnode->child = pnode;
                        curnode = pnode;
                }
                else{
                        curnode = child;
                }

                // get next symbol from word
                symbol = get_utf8_word(word, wordlen, &pos);
        }

        // copy lemma
        int      lemma_array_pos  = 0;
        int      lemma_array_size = 0;
        rawlemma_t* lemma_array      = NULL;
        rawlemma_t* plm              = NULL;
        lemma_array_pos  = tree->lemma_array_pos;
        lemma_array_size = tree->lemma_array_size; 
        lemma_array      = tree->lemma_array;
        plm              = lemma_array + lemma_array_pos;

        if(lemma_array_pos == lemma_array_size){
                return -1;
        }
        memcpy(plm->wordstr, word, wordlen+1);
        plm->wordlen = wordlen;
        plm->prop = prop;

        curnode->lemma_pos = lemma_array_pos;
        tree->lemma_array_pos++;

        return 0;
}


int rawtree_build_dmdict(rawtree_t* tree, const char* dictname)
{
        dm_dict_t* dm_dict = NULL;
        rawnode_t* root   = tree->root;
        int lemma_num = tree->lemma_array_pos;


        // create dm_dict
        if((dm_dict=dm_dict_create(MAX_SYMBOL_VALUE*20, lemma_num+1, 100*lemma_num))==NULL){
                assert(0);
        }

        // transfer tree
        aytree_t* aytree = dm_dict->aytree;
        if(rawtree_to_aytree(root, aytree)!=0){
                assert(0);
        }

        // copy lemma array
        char* wordbuf    = dm_dict->wordbuf;
        int wordbuf_size = dm_dict->wordbuf_size;
        int wordbuf_pos  = 0;
        for(int i=0;i<lemma_num;i++){
                rawlemma_t* plm_src = tree->lemma_array + i;
                lemma_t* plm_dst = dm_dict->lemma_array+i;
                char* wordstr = plm_src->wordstr;
                int   wordlen = plm_src->wordlen;

                if(wordbuf_pos+wordlen+1>=wordbuf_size){
                        assert(0);
                }
                plm_dst->wordstr = wordbuf_pos;
                plm_dst->wordlen = wordlen;
                plm_dst->prop    = plm_src->prop;
                memcpy(wordbuf+wordbuf_pos, wordstr, wordlen);
                wordbuf_pos += wordlen;
                wordbuf[wordbuf_pos++]='\0';
        }
        dm_dict->lemma_array_pos = lemma_num;
        dm_dict->wordbuf_pos = wordbuf_pos;

        // save dm_dict to file
        if(dm_dict_save(dm_dict, dictname)!=0){
                assert(0);
        }

        // free dm_dict
        dm_dict_free(dm_dict);
        dm_dict = NULL;

        return 0;

}

        
int rawtree_to_aytree(rawnode_t* root, aytree_t* aytree)
{
       return subtree_to_ay(root, 0, aytree);
}

int g_child_value_array[65535];

int subtree_to_ay(rawnode_t* curnode, unsigned int curstate, aytree_t* aytree)
{
        int* base        = NULL;
        int* check       = NULL;
        int* lmpos    = NULL;
        unsigned short ch = 0;
        int  newstate    = 0;
        int  base_val    = 0;
        rawnode_t* child = NULL;


        base     = aytree->base;
        check    = aytree->check;
        lmpos = aytree->lmpos;

        if(curnode->lemma_pos!=-1){
                lmpos[curstate] = curnode->lemma_pos;
        }

        child = curnode->child;
        if(child==NULL){ // it's  word end
                /*
                if(base[curstate]==0){
                        base[curstate] = -1 * curstate;
                }
                else{
                        base[curstate] *= -1;
                }
                */

                return 0;
        }

        // calculate the value of base[curstate] = bval
        //unsigned int bval = MAX_SYMBOL_VALUE;
        int* child_value_array = g_child_value_array;
        int  child_value_array_size = 65535;
        int  child_num = 0;
        for(child=curnode->child;child!=NULL;child=child->next){
                child_value_array[child_num++]=child->value;
                if(child_num >= child_value_array_size){
                        assert(0);
                }
        }
        //base_val = get_base_value(curnode, base, check, aytree->array_size);
        base_val = get_base_value(child_value_array, child_num, base, check, aytree->array_size);
        base[curstate]= base_val;

        // fill in check
        for(child=curnode->child;child!=NULL;child=child->next){
                ch = child->value;
                newstate = base_val + ch;
                check[newstate] = curstate;
                if(newstate>aytree->max_state){
                        aytree->max_state = newstate;
                }

        }

        // operate children recusively
        for(child=curnode->child;child!=NULL;child=child->next){
                ch = child->value;
                newstate = base_val + ch;
                subtree_to_ay(child, newstate, aytree);
        }

        return 0;
}
        
int get_base_value(int* child_value_array, int child_num, int* base, int* check, int array_size)
{
        int base_val = 0;
        int i = 0;
        int newstate = 0;

        for(base_val = 0; ; base_val++){
                for(i=0;i<child_num;i++){
                        newstate = base_val + child_value_array[i];
                        if(newstate>=array_size){
                                assert(0);
                        }
                        if(base[newstate] != -1 || check[newstate]!=-1){
                                break;
                        }
                }
                if(i==child_num){// found the base
                        break;
                }
        }

        return base_val;
}

char*      dm_dict_get_lemma_string(dm_dict_t* dm_dict, lemma_t* lemma)
{
	char* wordbuf = NULL;
	int   wordstr = 0;
	int   wordlen = 0;

	wordbuf = dm_dict->wordbuf;
	wordstr = lemma->wordstr;
	wordlen = lemma->wordlen;

	if(wordstr + wordlen >= dm_dict->wordbuf_size){
		return NULL;
	}
	else{
		return wordbuf + wordstr;
	}
}

