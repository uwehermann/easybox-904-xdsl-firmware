/*
 * Copyright (c) 2001,西安邮电学院Linux兴趣小组
 * All rights reserved.
 * 
 * 文件名称：do_cmd.h
 * 摘    要：声明所有给parse_cmd模块使用的处理具体FTP命令的函数。
 * 当前版本：1.1
 * 作    者：王聪 刘洋
 * 完成日期：2007年6月16日
 */

#ifndef _DO_CMD_H

#define _DO_CMD_H

extern	int do_stat(const char *);
extern 	int do_cdup(void);
extern	int do_cwd(char *);
extern	int do_mkd(const char *);
extern	int do_rmd(const char *);
extern	int do_dele(const char *);
extern	int do_pasv(void);
extern	int do_stor(char *);
extern	int do_abor(char*);
extern	int do_type(char *);
extern	int do_list(char *);
extern	int do_nlst(char *);
extern	void do_noop(void);
extern	int do_user(char *);
extern	int do_pass(char *);
extern	int do_port(char*);
extern	int do_quit(void);
extern 	int do_rnfr(void);
extern	int do_rnto(const char *, const char *);
extern	int do_syst(void);
extern	int _r_chdir(char *);
extern	int do_pwd(void);
extern	int failed(const char *);
extern	int do_retr(const char *);
extern	int do_mode(const char *);
extern	int do_stru(char *);
#endif /*end of _DO_CMD_H*/
