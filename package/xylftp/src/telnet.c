    /*
    * Copyright (c) 2007,西安邮电学院Linux兴趣小组
    * All rights reserved.
     *
     * 文件名称：telnet.c
     * 摘    要：进行命令解析操作 
     * 当前版本：2.0
     * 修改:去掉了函数参数中的控制连接套接字	
     * 作    者：贾孟树
     * 完成日期：2007年5月13日
     */

#include "xylftp.h"

extern struct user_env user_env;
extern struct run_env run_env;

extern int	parse_cmd(char *);

void telnet(void)
{
	int	read_cnt;	/*每次read的实际返回值*/	
	int	write_cnt;
	char	buf[BUF_LEN]={0};
	int	banner_len = (int)strlen(run_env.ftpd_banner) + 8;
	char	banner_mess[banner_len];
	
	snprintf(banner_mess, banner_len, "220 %s\r\n", run_env.ftpd_banner);
	if ((write_cnt = write(user_env.connect_fd, banner_mess, strlen(banner_mess))) == -1) {
		write_log("wirte error", 0);	
		exit(errno);
	}
	while (1) {
		if ((read_cnt = read(user_env.connect_fd, buf, BUF_LEN)) <= 0) {
			continue;		
		} 
		parse_cmd(buf);
		memset(buf, 0, read_cnt+1);	/*清空缓冲区*/
	}	
}
