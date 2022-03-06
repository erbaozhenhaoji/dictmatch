/********************************************************************************
 * Describe : This is a simple timer lib which stat time cosuming
 *
 * Author   : Yichuan Liu, shituo.lyc@taobao.com
 *
 * Create   : 2010-08-26
 *
 * Notice   : 1. NOT multi-thread safe, the correct usage is one per thread
 *            2. the nested usage is not supported currently
 ********************************************************************************/

#ifndef PY_TIMER_H
#define PY_TIMER_H

#include <sys/time.h>

#define PY_TIMER_BUF_MIN   256  //minimum buffer len of struct py_timer_t's buf
typedef struct
{
	struct timeval   tv;	// store the beginning of timer
	char           * buf;	// store timer info
	char           * pos;	// string tail in 'buf' (current pos)
	unsigned int     len;	// buf's capacity
	unsigned int     remain;// buf's remained capacity, corresponding to pos
} py_timer_t;

py_timer_t *  init_py_timer(unsigned int buflen);	// init
void          destroy_py_timer(py_timer_t * timer);	// destroy
void          clean_py_timer(py_timer_t * timer);	// clean timer to unused status
int           start_py_timer(py_timer_t * timer, char * str);	// start a timing
int           finish_py_timer(py_timer_t * timer);		// end  a timing
char *        get_timer_buf(py_timer_t * timer);		// get buf to print 
#endif
