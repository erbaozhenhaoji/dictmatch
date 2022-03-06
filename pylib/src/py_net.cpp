/***********************************************************************************
 * Describe : functions for TCP operation
 * Author   : Paul Yang, zhenahoji@gmail.com
 * Create   : 2008-10-15
 * Modify   : 2008-10-15
 **********************************************************************************/
#include <py_net.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>


/*
 * function : connect to a server
 *
 * args     : host, server host name or ip
 *          : port, server port
 * ret      : -1, failed; else, socket handler
 */
int py_tcpconnect(const char* host, int port)
{
	int fd  = -1;
	int err = -1;
	int ret = -1;
	struct sockaddr_in sin; 
	struct hostent he, *p;
	char buff[8192];

	if((fd=socket(AF_INET, SOCK_STREAM, 0))<0){
		return -1;
	}   

	bzero(&sin, sizeof(sin));

	if((sin.sin_addr.s_addr=inet_addr(host))==INADDR_NONE){
		if ((ret=gethostbyname_r(host, &he, buff, sizeof(buff), &p, &err)) < 0 ){
			close(fd);
			return -1; 
		}

		memcpy(&sin.sin_addr.s_addr, he.h_addr, sizeof(sin.sin_addr.s_addr));
	}

	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	if(connect(fd, (struct sockaddr*)&sin, sizeof(sin))<0){
		close(fd);
		return -1;
	}
	return fd;
}

/*
 * function : reconnect to a server
 *
 * args     : sock, the old socket
 *          : host, server host name or ip
 *          : port, server port
 * ret      : -1, failed; else, socket handler
 */
int py_reconnect(int sock, char* host, int port)
{
	unsigned int times = 0;

	close(sock);
	sock = -1;
	while(sock<0 && times<3){
		sock = py_tcpconnect(host, port);
		++times;
		sleep(1);
	}

	return sock;
}

/*
 * function : read all data from socket with a timeout
 *
 * args     : sock, the socket
 *          : buff, buffsize, data buffer and its size
 *          : timeout, time out
 * ret      : -1, failed; 0, time out; else, data length
 */
int py_readall_tv(int sock, void* buff, size_t buffsize, struct timeval timeout)
{
	int           ret  = 0;
	int           len  = 0;
	unsigned int  dcnt = 0;
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(sock,&fds);
	ret = select(sock+1, &fds, NULL, NULL, &timeout);
	if(ret<=0){
		return ret;
	}

	while(true){
		len = read(sock, (char*)buff+dcnt, buffsize-dcnt);
		if(len<0){
			return -1;
		}
		else if(len==0){
			break;
		}
		else if(dcnt==buffsize){
			break;
		}
		else{
			dcnt+=len;
			if(dcnt>buffsize){
				return -1;
			}
		}
	}


	return dcnt;
}

/*
 * function : read all data from socket with a timeout
 *
 * args     : sock, the socket
 *          : buff, buffsize, data buffer and its size
 *          : timeout, time out
 * ret      : -1, failed; 0, time out; else, data length
 */
int py_writeall_tv(int sock, const char* buff, size_t bufflen, struct timeval timeout)
{
	int           ret  = 0;
	int           len  = 0;
	const char*   src  = NULL;
	unsigned int  left = 0;
	fd_set fds;

	src  = buff;
	left = bufflen;
	FD_ZERO(&fds);
	FD_SET(sock,&fds);
	ret = select(sock+1, NULL, &fds, NULL, &timeout);
	if(ret<=0){
		return ret;
	}

	while(left>0){
		len = write(sock, src, left);
		if(len<0){
			return -1;
		}

		left -= len;
		src  += len;
	}


	return bufflen;
}



/*
 * function : create a TCP server
 *
 * args     : port, listen port
 *          : queue, listen queue
 * ret      : -1, failed; else, listening socket
 */
int py_tcplisten(int port, int queue)
{
	int  sock = -1;
	const int on=1;
	struct sockaddr_in sin;

	if((sock=socket(PF_INET, SOCK_STREAM, 0)) <0 )
		return -1;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bzero(&sin, sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=htonl(INADDR_ANY);
	sin.sin_port=htons(port);
	if(bind(sock, (struct sockaddr*)&sin, sizeof(sin))<0){   
		close(sock);
		return -1;
	}

	(queue <=0 )?queue=5:queue;
	if(listen(sock, queue)<0){
		close(sock);
		return -1;
	}

	return sock;
}

