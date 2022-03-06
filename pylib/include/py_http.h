/***********************************************************************************
 * Descri   : functions to deel with http protocal
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2011-2-18
 *
 * Update   : 2012-2-18
 **********************************************************************************/
#ifndef PY_HTTP_H
#define PY_HTTP_H

#define PY_HTTP_MAX_PAGE_SIZE   2*1024*1024
#define PY_HTTP_MAX_HEAD_SIZE   8*1024



// structure to hold the http response
// do NOT use in stack, because of its big size.

typedef struct _http_resp{
	int   status;
	char* head;
	int   head_len;
	char* page;
	int   page_len;
	char  buff[PY_HTTP_MAX_PAGE_SIZE];
}http_resp_t;


/*
 * func : send a http GET command, then store respose to http_resp
 *
 * args : url, the url to be fetched
 *      : ip,  the site ip
 *      : http_resp, structure to hold the http response
 *
 * ret  : 0, succeed
 *      : else, failed.
 */
int py_http_get(const char* url, const char* ip, http_resp_t* http_resp);



#endif
