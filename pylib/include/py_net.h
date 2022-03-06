/********************************************************************************
 * Describe : wrapers for net operation
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2008-10-15
 *
 * Update   : 2008-10-15
 ********************************************************************************/
#ifndef _PY_NET_H
#define _PY_NET_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/*
 * function : connect to a server
 *
 * args     : host, server host name or ip
 *          : port, server port
 *
 * ret      : -1, error 
 *          : else, socket handler
 */
int py_tcpconnect(const char* host, int port);

/*
 * function : reconnect to a server
 *
 * args     : sock, the old socket
 *          : host, server host name or ip
 *          : port, server port
 *
 * ret      : -1, error
 *          : else, socket handler
 */
int py_reconnect(int sock, char* host, int port);


/*
 * function : read all data from socket with a timeout
 *
 * args     : sock, the socket
 *          : buff, buffsize, data buffer and its size
 *          : timeout, time out
 *
 * ret      : -1, error
 *          :  0, time out;
 *          : else, data length
 */
int py_readall_tv(int sock, void* buff, size_t buffsize, struct timeval timeout);

/*
 * function : read all data from socket with a timeout
 *
 * args     : sock, the socket
 *          : buff, buffsize, data buffer and its size
 *          : timeout, time out
 *
 * ret      : -1, error;
 *          :  0, time out;
 *          : else, data length
 */
int py_writeall_tv(int sock, const char* buff, size_t bufflen, struct timeval timeout);

/*
 * function : create a TCP server
 *
 * args     : port, listen port
 *          : queue, listen queue
 *
 * ret      : -1, error;
 *          : else, listening socket
 */
int py_tcplisten(int port, int queue);

#endif
