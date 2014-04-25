/* All rights reserved.
 *
 * 文件名称：xylsocket.c
 * 摘    要：对socket函数进行封装
 * 当前版本：1.0
 * 作    者：董溥
 * 完成日期：2007年5月13日
 *
 *修改人员：林峰
 *修改日期：2007年6月6日
 */

#include "xylftp.h"

extern struct user_env user_env;
extern struct run_env run_env;

extern void telnet(void);

int r_close(int fd)
{
	int state;
	while ((state = close(fd)) == -1 && errno == EINTR) {
		continue;
	}
	return state;
}
void listen_connect(void)
{
	int socket_fd;
	int connect_fd;
	pid_t child_pid;
	#if 0/*def 	DEBUG*/
	run_env.ftp_port = 1986;
	#endif
	if ((socket_fd = xyl_listen(run_env.ftp_port)) == -1) {
		exit(1);
	}
	while (1) {
		if ((connect_fd = xyl_accept(socket_fd)) == -1) {
			if (r_close(socket_fd) == -1) {
				write_log("close error",0);
			}
			exit(1);
		}	
		user_env.connect_fd = connect_fd;	
		#ifdef DEBUG 
		write_log("run to fork",0);
		#endif

		signal(SIGCHLD,SIG_IGN);		/*to avoid zombie process*/

		if ((child_pid = fork()) == -1) {
			write_log("fork error",0);
		}
		else if (child_pid) {			/*parent*/
			if (r_close(user_env.connect_fd) == -1) {
				write_log("close error",0);
			}
			continue;
		}
		else if (child_pid == 0) {
			if (r_close(socket_fd) == -1) {
				write_log("close error",0);
			}
			
			telnet();

			if (r_close(user_env.connect_fd) == -1) {
				write_log("close connect_fd error",0);
			}
			exit(0);
		}
	}
}
int xyl_listen(short port) 
{
	int socket_fd;
	struct sockaddr_in server_addr;
	bzero((char*) &server_addr,sizeof(server_addr));	/*initial socket address to zero*/
	server_addr.sin_family = AF_INET;			/*use IPv4*/
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	/*use localhost IP*/
	server_addr.sin_port = htons(port);			/*convert from host byte order to network byte order*/

	if ((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perrorConsole("socket error\n");	
		if (errno == ENFILE){
			perrorConsole("The  system  limit  on  the  total number of open files has been reached.\n");
		}
		else if (errno == EPROTONOSUPPORT) {
			perrorConsole("The  protocol  type  or  the specified protocol is not supported within this domain.");
		}
		else if (errno == EACCES) {
			perrorConsole("Permission  to create a socket of the specified type and/or protocol is denied.");
		return -1;
		}
	}

	if ((bind(socket_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1)) {
		perrorConsole("bind error");
		if (errno == EADDRINUSE) {
			perrorConsole("The given address is already in use\n");
		}
		else if (errno == EBADF) {
			perrorConsole("sock_fd is not a valid descriptor\n");
		}
		else if (errno == EACCES) {
			perrorConsole("The address is protected\n");
		}
		if (r_close(socket_fd) == -1) {
			perrorConsole("close socket_fd error\n");
		}
		return -1;
	}

	if (listen(socket_fd,LISTENQ) == -1) {
		perrorConsole("listen error");
		if (errno == EADDRINUSE) {
			perrorConsole("Another socket is already listening on the same port.");
		return -1;
		}
	}

	return socket_fd;
}


int xyl_accept(int socket_fd)
{
	int connect_fd;
	socklen_t len;
	struct sockaddr_in client_addr;
	while (1) {
		len = sizeof(client_addr);
		if ((connect_fd = accept(socket_fd,(struct sockaddr *)&client_addr,&len)) == -1 && errno == EINTR) {
			continue;
		}
		if (connect_fd == -1) {
			perrorConsole("accept error errno is %d",errno);
			if (errno == ECONNABORTED) {
				perrorConsole("A connection has been aborted.\n");
			}
			else if (errno == EPERM) {
				perrorConsole("Firewall rules forbid connection.\n");
			}
			else if (errno == ENFILE) {
				perrorConsole("The system limit on the total number of open files has been reached.\n");
			}
			else if (errno == EINTR) {
				perrorConsole("The system call was interrupted by a signal that was caught before a valid connection arrived.");
			}
		}
		break;
	}
	user_env.client_ip = malloc(sizeof(char) * (strlen(inet_ntoa(client_addr.sin_addr)) + 1));
	strcpy(user_env.client_ip, inet_ntoa(client_addr.sin_addr));
	return connect_fd;
}


int xyl_connect(char *hostname,short port)
{
	int socket_fd;
	int ret;
	struct sockaddr_in server_addr;
	socklen_t len;

	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
	if ((server_addr.sin_addr.s_addr = inet_addr(hostname)) == (unsigned int)-1) {
		perrorConsole("invalid address\n");
		return -1;
	}
	/*bzero(&(server_addr.sin_zero),8);*/

	if ((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perrorConsole("socket error\n");	
		if (errno == ENFILE){
			perrorConsole("The  system  limit  on  the  total number of open files has been reached.\n");
		}
		else if (errno == EPROTONOSUPPORT) {
			perrorConsole("The  protocol  type  or  the specified protocol is not supported within this domain.");
		}
		else if (errno == EACCES) {
			perrorConsole("Permission  to create a socket of the specified type and/or protocol is denied.");
		}
		return -1;

	}	

	while (1) {
		len = sizeof(server_addr);
		if (((ret = connect(socket_fd,(struct sockaddr *)&server_addr,len)) == -1 && errno == EINTR)
				|| errno == EALREADY) {
			continue;
		}
		if (ret == -1) {
			perrorConsole("connect error\n");
			if (errno == ENETUNREACH) {
				perrorConsole("Network is unreachable.\n");
			}	
			else if (errno == ETIMEDOUT) {
				perrorConsole("Timeout while attempting connection\n");
			}
			else if (errno == EISCONN) {
				perrorConsole("The socket is already connected.\n");
			}
			else if (errno == ECONNREFUSED) {
				perrorConsole("No one listening on the remote address.\n");
			}	
			if (r_close(socket_fd) == -1) {
				perrorConsole("close socket_fd error");
			}
			return -1;
		}
		break;
	}
	return socket_fd;
}



