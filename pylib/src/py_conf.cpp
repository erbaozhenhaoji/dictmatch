/********************************************************
 * Filename : py_conf.cpp
 *
 * Describe : Implementation fo config class
 *
 * Author   : Paul Yang, zhenhaoji@gmail.com
 *
 * Date     : 2007-01-06
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <py_utils.h>
#include <py_conf.h>
#include <assert.h>


/*
 * func : create a CONFDATA struct
 *
 * args : max_confnum, the maximum config number
 *
 * ret  : NULL, error
 *      : else, pointer to CONFDATA struct
 */
CONFDATA* confdata_create(const int max_confnum)
{
	CONFDATA* confdata   = NULL;
	CONFITEM* conf_array = NULL;

	confdata = (CONFDATA*)calloc(1, sizeof(CONFDATA));
	if(confdata==NULL){
		goto failed;
	}

	conf_array = (CONFITEM*)calloc(max_confnum, sizeof(CONFITEM));
	if(!conf_array){
		goto failed;
	}

	confdata->conf_array = conf_array;
	confdata->conf_num   = 0;
	confdata->conf_size  = max_confnum;
	return confdata;

failed:
	if(confdata){
		free(confdata);
		confdata = NULL;
	}
	if(conf_array){
		free(conf_array);
		conf_array = NULL;
	}

	return NULL;
}

/*
 * func : load config from file
 *
 * args : path, file, config path and file
 *
 * ret  : NULL, error
 *      : else, pointer to the CONFDATA struct
 */
CONFDATA* confdata_load(const char* path, const char* file)
{
	char fullpath[256];
	cmps_path(fullpath, sizeof(fullpath)-1, path, file);

	return confdata_load(fullpath);
}


/*
 * func : load config from file
 *
 * args : fullpath, the file full name
 *
 * ret  : NULL, error
 *      : else pointer to CONFDATA struct
 */
CONFDATA* confdata_load(const char* fullpath)
{
	int       max_confnum  = 0;
	FILE*     fp           = NULL;
	CONFDATA* confdata     = NULL;
	char      buff[1024];
	char*     parts[5];

	if(py_fline(fullpath, &max_confnum)<0){
		return NULL;
	}
	if((confdata=confdata_create(max_confnum+50))==NULL){
		return NULL;
	}

	if((fp=fopen(fullpath, "rb"))==NULL){
		confdata_free(confdata);
		confdata = NULL;
		return NULL;
	}

	int       conf_num   = 0;
	CONFITEM* conf_array = confdata->conf_array;
	while(fgets(buff, sizeof(buff), fp)){
		// skip comments
		unsigned int i=0;
		for(;buff[i]==' '||buff[i]=='\t';i++);
		if(buff[i]=='#'){
			continue;
		}

		int num = split_c(buff, parts, 5, " \t\r\n");
		if(num!=3 || parts[1][0]!=':'){ // bad format
			continue;
		}

		strncpy(conf_array[conf_num].key, parts[0], PY_MAX_WORDLEN-1);
		conf_array[conf_num].key[PY_MAX_WORDLEN-1] = '\0';
		strncpy(conf_array[conf_num].val, parts[2], PY_MAX_WORDLEN-1);
		conf_array[conf_num].val[PY_MAX_WORDLEN-1] = '\0';
		conf_num++;
	}

	fclose(fp);
	confdata->conf_num = conf_num;

	return confdata;
	
}

/*
 * func : free a confdata struct
 */
void  confdata_free(CONFDATA* confdata)
{
	if(!confdata){
		return;
	}
	if(confdata->conf_array){
		free(confdata->conf_array);
		confdata->conf_array = NULL;
	}
	free(confdata);
	confdata = NULL;
}

/*
 * func : save CONFDATA to a disk file
 *
 * args : confdata, pointer to CONFDATA
 *      : path, file, dest path and dest file
 *
 * ret  : 0, succeed
 *      : -1, error
 */
int confdata_save(CONFDATA* confdata, const char* path, const char* file)
{
	FILE* fp = NULL;
	char fullpath[256];
	
	if(!confdata){
		return -1;
	}

	cmps_path(fullpath, sizeof(fullpath), path, file);
	if((fp=fopen(fullpath, "wb"))==NULL){
		return -1;
	}
	
	int       conf_num   = confdata->conf_num;
	CONFITEM* conf_array = confdata->conf_array;
	for(int i=0;i<conf_num;i++){
		if(conf_array[i].key[0] == '\0'){
			continue;
		}
		fprintf(fp, "%s : %s", conf_array[i].key, conf_array[i].val);
	}

	fclose(fp);

	return 0;
}

/*
 * func : add a key value pair to CONFDATA struct
 *
 * args : confdata, the pointer to CONFDATA
 *      : key, val, the key<-->value pair
 *
 * ret  : 1, data added successfully
 *      : 0, key already exist, but modified successfully
 *      : -1, error
 */
int confdata_additem(CONFDATA* confdata, const char* key, const char* val)
{
	int i          = 0;
	int conf_size  = 0;
	int conf_num   = 0;
	CONFITEM*  conf_array = NULL;

	conf_array = confdata->conf_array;
	conf_size  = confdata->conf_size;
	conf_num   = confdata->conf_num;

	// check if the key already exist
	for(i=0;i<conf_num;i++){
		if(strcmp(conf_array[i].key, key)==0){
			strncpy(conf_array[i].val, val, PY_MAX_WORDLEN);
			conf_array[i].val[PY_MAX_WORDLEN-1] = '\0';
			return 0;
		}
	}
	
	// find a blank unit
	for(;i<conf_num;i++){
		if(conf_array[i].key[0] == '\0'){
			break;
		}
	}
	if(i==conf_size){  // CONFDATA is alreay full, add can NOT be executed.
		return -1;
	}

	strncpy(conf_array[i].key, key, PY_MAX_WORDLEN);
	conf_array[i].key[PY_MAX_WORDLEN-1] = '\0';
	strncpy(conf_array[i].val, val, PY_MAX_WORDLEN);
	conf_array[i].val[PY_MAX_WORDLEN-1] = '\0';

	if(i==conf_num){  // add on tail
		conf_num++;
		confdata->conf_num=conf_num;
	}

	return 1;
}

/*
 * func : get the value of a specified key form CONFDATA
 *
 * args : confdata, pointer to confdata
 *      : key, the specified key to get
 *      : val, the result value of the key
 * ret  : 1, find the key, and get the value successfully
 *      : 0, can not find the key
 *      : -1, error
 */
int confdata_getitem(CONFDATA* confdata, const char* key, char* val, const int valsize)
{
	int conf_num = 0;
	CONFITEM* conf_array = NULL;

	if(!confdata){
		return -1;
	}
	conf_array = confdata->conf_array;
	conf_num   = confdata->conf_num;

	for(int i=0;i<conf_num;i++){
		if(strcmp(conf_array[i].key, key)==0){
			strncpy(val, conf_array[i].val, valsize);
			val[valsize-1] = '\0';
			return 1;
		}
	}

	return 0;

}

/*
 * func : delete an item from CONFDATA
 *
 * args : confdata, pointer to CONFDATA
 *      : key, the key to find
 *
 * ret  : 1, find the key, and deleted
 *      : 0, can NOT find the key
 *
 * note : set key[0] = '\0' means delete
 */
int confdata_delitem(CONFDATA* confdata, const char* key)
{
	int conf_num = 0;
	CONFITEM* conf_array = NULL;

	conf_array = confdata->conf_array;
	conf_num   = confdata->conf_num;

	for(int i=0;i<conf_num;i++){
		if(strcmp(conf_array[i].key, key)==0){
			conf_array[i].key[0] = '\0';
			return 1;
		}
	}

	return 0;
}
	

