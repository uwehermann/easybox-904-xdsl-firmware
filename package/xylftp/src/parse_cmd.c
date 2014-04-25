/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：parse_cmd.c
 * 摘    要：从一行数据中解析出命令和参数 并调用相应的命令处理模块 
 * 对读取的字符串做了初步分析 提高了容错能力 对参数的接收做了限制，一次只接收一个参数。
 * 重写了接收参数模块,去掉了无用参数buf_size,_line_cmd()的返回值改成了结构体指针。
 * 当前版本：4.0
 * 作    者：贾孟树
 * 完成日期：2007年6月13日
 * 取代版本：3.0
 * 完成日期：2007年6月8日
 * 修改者：王聪
 * 修改日期：2007年6月15日
 */


#include "xylftp.h"
#include "do_cmd.h"

#define MAX_CMD 5
#define MAX_ARG 4096

extern struct user_env user_env;
extern struct run_env run_env;
struct parse_cmd{
	char	cmd[MAX_CMD];
	char	arg[MAX_ARG];
} ;


const char *commands[] = {"USER","PASS","SYST","QUIT","RETR","STOR","RNFR","RNTO","ABOR","DELE",
		"RMD","MKD","PWD","CWD","CDUP","PORT","NOOP","PASV","TYPE","MODE",
		"STAT","STRU","LIST", "NLST"};			/*服务器支持的所有命令*/

static int _line_cmd(char *line_buf, struct parse_cmd *user_cmd)
{
	int	i = 0;
	int	j = 0;
	int	k = 0;

	while (!isalpha(line_buf[i])) {
		i++;							
	} 		/*<--略过不是字母的字符*/		 		
	while (line_buf[i] != ' '
			&& line_buf[i] != '\r'
			&& line_buf[i] != '\n') {
		if (k < MAX_CMD-1) {
			user_cmd->cmd[k++] = line_buf[i];
			i++;
		}
		else {
			return -1;
		}
	}		/*<--接收以字母开头的字符串（命令）*/
	while (line_buf[i] == ' ') {		
		i++;	
	} 		/*<--清除命令与参数之间的空格*/
	while (line_buf[i] != '\0'
			&& line_buf[i] != '\r'
			&& line_buf[i] != '\n') {
		user_cmd->arg[j++] = line_buf[i];
		i++;
	}		/*<--只提取第一个参数*/
	
	for (;k >= 0;k--) {		/*检查命令中字母的大小写，若是小写字母，则转化为大写字母*/
		user_cmd->cmd[k] = toupper(user_cmd->cmd[k]);
	}
#ifdef DEBUG
	printfConsole("\n");
	printfConsole("CMD:");
	printfConsole("%s",user_cmd->cmd);
	printfConsole("\n");
	printfConsole("ARG:");
	printfConsole("%s",user_cmd->arg);
	printfConsole("\n");
#endif
	return 0;
}

static int _cmd_num(struct parse_cmd cmd)
{
	int i;
	for (i = 0; i < (int)(sizeof(commands)/sizeof(commands[0])); i++) {
		if (strcmp(cmd.cmd, commands[i]) == 0) {
			return i+1;
		}		
	}
	return 0;
}


int parse_cmd(char *p_buf)
{	
	char	rnfr_arg[MAX_ARG];
	struct parse_cmd user_cmd;
	int i = 0;

#ifdef DEBUG
	printfConsole( "parse_cmd(): %s\n", p_buf );
#endif

	memset(&user_cmd, 0, sizeof(struct parse_cmd));
	if (_line_cmd(p_buf, &user_cmd) == -1) {
		failed(user_cmd.cmd);
		return 2;
	}
	if (( i = _cmd_num(user_cmd)) <= 4 || user_env.login_in == TRUE) {
#ifdef DEBUG
		printfConsole("****_cmd_num() %d\n", i);
#endif
		switch (i) {
		case 1:
#ifdef DEBUG
				printfConsole("****call user()\n");
#endif
				do_user(user_cmd.arg);
				break;
		case 2:
#ifdef DEBUG
				printfConsole("call pass()\n");
				printfConsole("username=%s\n", user_env.user_name);
#endif
				if (strlen(user_env.user_name) != 0) { 
					if (do_pass(user_cmd.arg) == 0) {
						if (chroot(run_env.ftp_dir) < 0) {
							write_log("chroot error", 0);
							do_quit();
							break;	
						}
						if (chdir("/") < 0) {
							write_log("chdir error", 0);	
							do_quit();
							break;
						}
					} else {
						break;
					}
				} else {
					const char	*mess = "220 No username input.\r\n";
					write(user_env.connect_fd, mess, strlen(mess));
				}

				break;
		case 3:
#ifdef DEBUG
				printfConsole("call syst()\n");
#endif
				do_syst();
				break;
		case 4:
#ifdef DEBUG
				printfConsole("call quit()\n");
#endif
				do_quit();
				break;	
		case 5:
#ifdef DEBUG
				printfConsole("call retr()\n");
#endif
				do_retr(user_cmd.arg);
				break;
		case 6:
#ifdef DEBUG
				printfConsole("call stor()\n");
#endif
				do_stor(user_cmd.arg);
				break;
		case 7:
#ifdef DEBUG
				printfConsole("call rnfr()\n");
#endif
				do_rnfr();
				strcpy(rnfr_arg, user_cmd.arg);
				break;
		case 8:
#ifdef DEBUG
				printfConsole("call rnto()\n");
#endif
				do_rnto(rnfr_arg, user_cmd.arg);
				memset(rnfr_arg, 0, MAX_ARG);
				break;
		case 9:
#ifdef DEBUG
				printfConsole("call abor()\n");
#endif
				do_abor(user_cmd.arg);
				break;
		case 10:
#ifdef DEBUG
				printfConsole("call dele()\n");
#endif
				do_dele(user_cmd.arg);
				break;
		case 11:
#ifdef DEBUG
				printfConsole("call rmd()\n");
#endif
				do_rmd(user_cmd.arg);
				break;
		case 12:
#ifdef DEBUG
				printfConsole("call mkd()\n");
#endif
				do_mkd(user_cmd.arg);
				break;
		case 13:
#ifdef DEBUG
				printfConsole("call pwd()\n");
#endif
				do_pwd();
				break;
		case 14:
#ifdef DEBUG
				printfConsole("call cwd()\n");
#endif
				do_cwd(user_cmd.arg);
				break;
		case 15:
#ifdef DEBUG
				printfConsole("call cdup()\n");
#endif
				do_cdup();
				break;
		case 16:
#ifdef DEBUG
				printfConsole("call port()\n");
#endif
				do_port(user_cmd.arg);
				break;	
		case 17:
#ifdef DEBUG
				printfConsole("call noop()\n");
#endif
				do_noop();	
				break;
		case 18:
#ifdef DEBUG
				printfConsole("call pasv()\n");
				printfConsole("\n");
#endif
				do_pasv();
				break;
		case 19:
#ifdef DEBUG
				printfConsole("call type()\n");
#endif
				do_type(user_cmd.arg);
				break;
		case 20:
#ifdef DEBUG
				printfConsole("call mode()\n");
#endif
				do_mode(user_cmd.arg);
				break;
		case 21:
#ifdef DEBUG
				printfConsole("call stat()\n");
#endif
				do_stat(user_cmd.arg);
				break;
		case 22:
#ifdef DEBUG
				printfConsole("call stru()\n");
#endif
				do_stru(user_cmd.arg);
				break;
		case 23:
#ifdef DEBUG
				printfConsole("call list()\n");
#endif
				do_list(user_cmd.arg);
				break;
		case 24:
#ifdef DEBUG
				printfConsole("call nlst()\n");
#endif
				do_nlst(user_cmd.arg);
				break;
		default:
#ifdef DEBUG
				printfConsole("call failed()\n");
#endif
				failed(user_cmd.cmd);
				break;
		}		
	} else if (i <= 23 && i >= 1) {
		write(user_env.connect_fd, "530 Please login with USER and PASS.\r\n",
			strlen("530 Please login with USER and PASS.\r\n"));
	} else {
		failed(user_cmd.cmd);
	}
	return 1;
}
