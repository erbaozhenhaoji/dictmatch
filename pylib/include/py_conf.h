/********************************************************************************
 * Describe : This is the config parsing lib, here config is treated as a group
 *          : of key-->value pairs.
 *
 * Author   : Paul Yang, zhenhaoji@gmail.com
 *
 * Create   : 2008-12-16
 *
 * Update   :
 ********************************************************************************/
#ifndef PY_CONF_H
#define PY_CONF_H

// macros defined here
//
#define PY_MAXCONF_NUM    1024
#define PY_MAX_WORDLEN    128


// data types defined here
//
typedef struct _confitem{
	char key[PY_MAX_WORDLEN];
	char val[PY_MAX_WORDLEN];
}CONFITEM;

typedef struct _confdata{
	CONFITEM*  conf_array;    // config pair array
	int        conf_num;      // current config number
	int        conf_size;     // max config number the structure can hold
}CONFDATA;

// functions defined here
//

/*
 * func : create a CONFDATA struct
 *
 * args : max_confnum, the maximum config number
 *
 * ret  : NULL, error
 *      : else, pointer to CONFDATA struct
 */
CONFDATA* confdata_create(const int max_confnum);


/*
 * func : load config from file
 *
 * args : path, file, config path and file
 *
 * ret  : NULL, error
 *      : else, pointer to the CONFDATA struct
 */
CONFDATA* confdata_load(const char* path, const char* file);

/*
 * func : load config from file
 *
 * args : fullpath, the file full name
 *
 * ret  : NULL, error
 *      : else pointer to CONFDATA struct
 */
CONFDATA* confdata_load(const char* fullpath);

/*
 * func : free a confdata struct
 */
void      confdata_free(CONFDATA* confdata);

/*
 * func : save CONFDATA to a disk file
 *
 * args : confdata, pointer to CONFDATA
 *      : path, file, dest path and dest file
 *
 * ret  : 0, succeed
 *      : -1, error
 */
int       confdata_save(CONFDATA* confdata, const char* path, const char* file);

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
int       confdata_additem(CONFDATA* confdata, const char* key, const char* val);

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
int       confdata_delitem(CONFDATA* confdata, const char* key);

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
int       confdata_getitem(CONFDATA* confdata, const char* key, char* val, const int valsize);

#endif
