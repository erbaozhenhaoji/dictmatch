/******************************************************************************
 * Describe : socket connection pool for multi-thread server
 * 
 * Author   : Paul Yang, zhenhaoji@gmail.com
 *
 * Create   : 2008-10-16
 *
 * Modify   : 2008-10-16 
 *
 *****************************************************************************/
#ifndef _SOCKPOOL
#define _SOCKPOOL

#include <py_queue.h>
#include <pthread.h>


// data type defined here
//



typedef struct _sockpool{
	int     timeout;
	int     listen_port;
	int     listen_queue;
	int     sock_listen;
}SOCKPOOL;

/*
 * func : create a socket pool
 *
 * args : sock_num, max socket num, capacity of the pool
 *      : queue_size, ready socket queue size, should be <= sock_num
 *      : timeout, how long does a ready socket become timeout
 *
 * ret  : NULL, error; 
 *      : else, pointer of SOCKPOOL struct;
 */
SOCKPOOL* sockpool_create(int port, int queue_size,  int timeout);



/*
 * func : free a socket pool
 *
 * args : sockpool, the socket pool pointer
 *
 * ret  :
 */
void sockpool_free(SOCKPOOL* sockpool);

/*
 * func : manage a socket pool, usually called by main thread
 * 
 * args : sockpool, socket pool pointer
 *
 * ret  :
 */

/*
 * func : get a ready socket from socket pool, if can not get one, it will wait blocked
 * 
 * args : sockpool, the socket pool pointer
 *      : sock, the ready socket
 *      : handle, used for socket pool management
 * ret  : 0, succeed; else faied;
 */
int sockpool_get(SOCKPOOL* sockpool);

#endif
