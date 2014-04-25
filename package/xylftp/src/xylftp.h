/*
 *Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：xylftp.h
 * 摘    要：全局结构定义，宏定义，函数原型
 * 当前版本：1.1
 * 作    者：董溥
 * 完成日期：2007年5月15日
 * 取代版本：1.0
 * 修改人员：刘洋 林峰
 * 最后修改：2007年6月10日
 * 说    明：defines.h将被废弃不用，此文件将取代它，在原来的基础上增加了
 * 四组关于当前用户的统计数据，使STAT命令容易实现。增加关于命令名长度和参数最大长度的声明。
 */

#ifndef _XYLFTP_H /*prevent reincluding this file*/
#define _XYLFTP_H

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
/*#include <openssl/md5.h>*/
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <ctype.h>

extern int  read_configure(void);
extern int  write_log(char *message,int level);
extern int	printfConsole(const char *sFmt, ...);
#define		perrorConsole	printfConsole
extern void listen_connect(void);
extern int  xyl_listen(short port);
extern int  xyl_accept(int socket_fd);
extern int  xyl_connect(char *server,short port);
extern int  r_close(int fd);
extern void free_sources(void);

typedef unsigned int bool;
#define TRUE 1
#define FALSE 0
#define MAXFD 65535
#define LISTENQ 128
#define MAX_ADDR 256
#define _BUFFER_LENGTH 150
#define CONFIG_FILE "/etc/config/xylftp.conf"
#define PATH_NAME_LEN 1024
#define USER_NAME_LEN 32
#define MAX_MSG_LEN 256
#define MAX_INTERFACES 16
#define BUF_LEN 4096

/*运行时的环境变量，从配置文件中读取*/
struct run_env{
	bool anonymous_enable;				/*是否允许匿名登录*/
	unsigned short ftp_port;			/*FTP使用的端口号*/
	unsigned int local_umask;			/*上传文件权限*/
	unsigned int log_file_enable;			/*是否启用日志*/
	char *log_file;					/*日志文件存储路径*/
	unsigned int idle_session_timeout;		/*控制链接的最大空闲时间，超时断开链接*/
	unsigned int data_connection_timeout;		/*数据链接的最大空闲时间，超时断开链接*/
	char *ftpd_banner;				/*登录欢迎信息*/
	unsigned int max_clients;			/*允许的最大客户数目*/
	unsigned int max_links;				/*允许的最大链接数目*/
	unsigned int passive_port_max;			/*被动模式下监听的端口范围*/
	unsigned int passive_port_min;
	char ftp_dir[PATH_NAME_LEN];			/*FTP根目录位置*/
	char *user_pass_file;				/*用户数据文件目录*/
	char visible_user_name[USER_NAME_LEN];		/*用户所看到的文件所有者*/
	char visible_group_name[USER_NAME_LEN];	/*用户所看到的文件所有者所在的用户组*/
};


/*用户登录服务器的环境变量*/
struct user_env{
	bool login_in;				/*是否已经登录*/
	char user_name[USER_NAME_LEN];		/*登录的用户名*/
	unsigned int user_id;			/*登录的用户ID*/
	unsigned int client_data_port;		/*客户端数据连接使用端口*/
	char *client_ip;			/*客户端ip*/
	unsigned short client_port;		/*客户所使用的端口号*/
	unsigned long login_time;		/*登录时间*/
	unsigned long last_operation_time;	/*上次操作时间*/
	char current_path[PATH_NAME_LEN];	/*当前路径*/
	unsigned int enable_upload;		/*是否允许上传*/
	bool passive_on;			/*是否为被动被模式*/
	bool ascii_on;				/*是否为ascii码模式*/
	int connect_fd;				/*控制连接*/
	int data_fd;				/*数据连接*/
	unsigned int upload_files;
	unsigned int upload_kbytes;
	unsigned int download_files;
	unsigned int download_kbytes;		/*以上四项为传输过程中的统计数据*/
};

/*用户命令的声明*/

#endif /*the _XYLFTP_H end*/
