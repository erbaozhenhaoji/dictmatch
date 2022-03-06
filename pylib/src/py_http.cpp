/***********************************************************************************
 * Descri   : functions to deel with http protocal
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2012-2-18
 *
 * Update   : 2012-2-18
 **********************************************************************************/
#include <stdio.h>
#include <py_http.h>
#include <py_url.h>
#include <py_log.h>
#include <py_net.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#define PY_HTTP_TIMEOUT 30

static int connect_to_site(const char* host, int port);
static int nb_connect(int sockfd, struct sockaddr* saptr, socklen_t salen, int nsec);
static int makecmd(char *buff, int buff_size, char *host, char *path);
static int parse_http_response(const char* buff, int buff_len, http_resp_t* http_resp);

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
int py_http_get(const char* url, const char* ip, http_resp_t* http_resp)
{
	char site[1024];
	char path[1024];
	int port   = 0;
	int sockfd = -1;
	const char* host = NULL;
	char* buff = NULL;

	http_resp->status  = 0;
	http_resp->head = NULL;
	http_resp->head_len = 0;
	http_resp->page = NULL;
	http_resp->page_len = 0;
	http_resp->buff[0] = '\0';

	if(py_parse_url(url, site, sizeof(site), &port, path, sizeof(path))!=0){
		py_log_write(PY_LOG_NOTICE, "parse url failed %s", url);
		return -1;
	}

	// connect to server
	if(ip!=NULL){
		host = ip;
	}
	else{
		host = site;
	}
	sockfd = connect_to_site(host, port);
	if(sockfd == -1){
		py_log_write(PY_LOG_NOTICE, "connect to url :%s failed\n", url);
		return -1;
	}

	// send http GET command
	char command[2048];
	int  len = 0;
	struct timeval tv;
	tv.tv_sec  = PY_HTTP_TIMEOUT;
	tv.tv_usec = 0;
	if(path[0] == '\0'){
		strcpy(path, "/");
	}
	len = makecmd(command, sizeof(command), site, path);
	if(py_writeall_tv(sockfd, command, len, tv)!=len){
		close(sockfd);
		return -1;
	}

	// get response data
	buff      = http_resp->buff;
	len = py_readall_tv(sockfd, buff, PY_HTTP_MAX_PAGE_SIZE, tv);
	if(len<=0){
		return -1;
	}

	// parse date
	if(parse_http_response(buff, len, http_resp)!=0){
		return -1;
	}

	return 0;
}

/*
 * func : connect to the site host by ip or by url
 *
 * args : url, the url to be fetched
 *      : ip,  the site ip
 *
 * ret  : >0, sockfd
 *      : else, failed.
 */
int connect_to_site(const char* host, int port)
{
	char buff[8192];
	int sockfd = -1;
	int err    = -1;
	int ret    = -1;
	struct sockaddr_in sa;
	struct hostent he, *p;


	// fill in server adress info
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port   = htons(port);
	bzero(sa.sin_zero, 8);

	if((sa.sin_addr.s_addr=inet_addr(host))==INADDR_NONE){
		if ((ret=gethostbyname_r(host, &he, buff, sizeof(buff), &p, &err)) < 0 ){
			return -1; 
		}
		memcpy(&sa.sin_addr.s_addr, he.h_addr, sizeof(sa.sin_addr.s_addr));
	}

	// open a socket
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){
		return -1;
	}

	// connet to server in no block mode
	int salen = sizeof(struct sockaddr);
	if(nb_connect(sockfd, (struct sockaddr*)&sa, salen, PY_HTTP_TIMEOUT)!=0){
		py_log_write(PY_LOG_NOTICE, "connect to %s failed!\n",host);
		return -1;
	}

	return sockfd;
}

/*
 * func : setup a noblock connection
 *
 * args : sockfd, the socket handle
 *      : saptr, salen, the target address
 *      : nsec, timeout
 *
 * ret  : 0, succeed
 *      : else, failed.
 */
int nb_connect(int sockfd, struct sockaddr* saptr, socklen_t salen, int nsec)
{
	int flags      = 0;
	int n          = 0;
	int error      = 0;
	socklen_t len  = 0;
	fd_set rset;
	fd_set wset;
	struct timeval tval;

	// set socket to noblock
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	// connect to host
	errno = 0;
	n = connect(sockfd, saptr, salen);
	if(n==0){
		goto done;
	}
	else{
		if(errno!=EINPROGRESS){
			return -1;
		}
	}


	// select to check the connection is completed
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset         = rset;
	tval.tv_sec  = nsec;
	tval.tv_usec = 0;
	if(tval.tv_sec<=0){
		tval.tv_sec = 1;
	}
	n = select(sockfd+1, &rset, &wset, NULL, &tval);
	if(n==0){
		close(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}
	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)){
		len = sizeof(error);
		if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)<0){
			return -1;
		}
	}
	else{
		py_log_write(PY_LOG_NOTICE, "sockfd not set, select error\n");
	}

done:

	// restore socket flags
	fcntl(sockfd, F_SETFL, flags);
	if(error){
		close(sockfd);
		errno = error;
		return -1;
	}
	return 0;
}

int makecmd(char *buff, int buff_size, char *host, char *path)
{
	int len = 0;
	len =  snprintf( buff,buff_size,
			"%s%s%s%s%s",
			"GET ",
			path,
			" HTTP/1.1\r\nHost: ",
			host,
			"\r\nConnection: close"
			"\r\nUser-Agent: PaulSpider+(+http://www.kengni.com/spider.htm)"
			"\r\n\r\n");
	return len;
}


int parse_http_response(const char* buff, int buff_len, http_resp_t* http_resp)
{
	int   status         = 0;
	const char* head     = NULL;
	int   head_len       = 0;
	const char* page     = NULL;
	int   page_len       = 0;
	const char* end      = 0;
	char  version[256];


	// extract status from the 1st line, like : HTTP/1.1 200 OK
	if(sscanf(buff, "%s %d", version, &status)!=2){
		return -1;
	}
	
	// get http head and its length
	head   = strstr(buff, "\r\n");
	if(head==NULL){
		return -1;
	}
	head +=2;

	end = strstr(head, "\r\n\r\n");
	if(NULL == end){
		return -1;
	}
	head_len = end - head;

	// get http page and its length
	page = end + 4;
	page_len = buff + buff_len - page;

	// fill in http_resp struct
	http_resp->status   = status;
	http_resp->head     = (char*)head;
	http_resp->head_len = head_len;
	http_resp->page     = (char*)page;
	http_resp->page_len = page_len;

	return 0;
}
