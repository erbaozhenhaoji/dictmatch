/***********************************************************************************
 * Descri   : check code type
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2012-02-20
 *
 * Update   : 2012-02-20
 **********************************************************************************/
#include <py_code.h>
#include <stdlib.h>
#include <string.h>

 /*
 * func : judge the code type of an html page
 *
 * args : page, pagelen, the page and its length
 *
 * ret  : code type
 */
code_type_t py_check_code_html(const char* page, int pagelen)
{
	char buff[1024];
	char* codestr = NULL;
	int len = 0;
	code_type_t code;


	if(pagelen>1024){
		len = 1024;
	}
	else{
		len = pagelen;
	}
	py_2lower(page, len, buff, sizeof(buff));
	codestr = strstr(buff, "charset=");
	if(NULL==codestr){
		return CODE_TYPE_GB;
	}
	codestr += 8;

	if(strncmp(codestr, "gb", 2)==0){
		code = CODE_TYPE_GB;
	}
	else if(strncmp(codestr, "big5", 4)==0){
		code = CODE_TYPE_BIG5;
	}
	else if(strncmp(codestr, "utf-8", 5)==0){
		code = CODE_TYPE_UTF8;
	}
	else{
		code = CODE_TYPE_GB;
	}

	return code;
}
 


