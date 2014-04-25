/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 * 
 * 文件名称：命令处理
 * 摘    要：处理MKD RMD DELE PASV STOR命令
 * 
 * 当前版本：1.1
 * 作    者：聂海海 王亚刚 郭拓 贾孟树
 * 完成日期：2007年5月30日
 * 修 改 者：刘洋 林峰 王聪 聂海海
 * 修改日期：2007年6月16日
 * 取代版本：1.0
 * 摘    要：原来增加的命令很符合要求，重建此文件。全部根据要求，现将命令单独在test目录中通过的逐条加入。
 * 今天仅加入STAT。
 *14日加入USER。
 *6.15: 修正英文语法错误，排版错误。
 *6.16: 添加do_mkd，do_rmd, do_dele, do_retr
 *6.16:添加do_pass
 */

#define _GNU_SOURCE
#include "xylftp.h"
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

extern struct user_env user_env;
extern struct run_env run_env;

static void _stat_fail_450(void);
static void _stat_error_421(void);
static void _stat_error1_501(void);
static void _stat_error2_501(void);
static void _stat_error3_501(void);
static void _stat_fail_550(void);
static void _stat_success_150(void);
static void _stat_success_226(void);
static void _stat_fail_501(void);
static int _get_local_ip_address(int sock, char* ip_addr);
static int _stat_mkd(const char *path);    
static int _stat_rmd(const char *path);    
static int _stat_dele(const char *path);
static int _stat_retr(const char *);
static int _ignore_sigpipe(void);
static int _analysis_ipaddr(char *, char **, int *);
static int _close_connection(int);
static int _chdir(char*);
static int _write_to_socket(char*);
int do_quit(void);
int failed(const char *s);

int do_user(char username[])
{
	const char anonymous[] = "anonymous";
	const char inf_buf[] = "331 Please send you password.\r\n";
	const char no_anonymous[] = "555 Anonymous not allowed on this server.\r\n"; 
	const char noname[] = "500 USER: command requires a parameter.\r\n";	
	const char logged[] = "503 You have already logged in.\r\n";

	if (username[0] == '\0') {
		write(user_env.connect_fd, noname, strlen(noname));
		#ifdef DEBUG
		printfConsole(noname);
		#endif
		return -1;
	}	

	if (user_env.login_in == TRUE) {
		write(user_env.connect_fd, logged, strlen(logged));
		#ifdef DEBUG
		printfConsole(logged);
		#endif
		return -1;
	}

	if (strcmp(username, anonymous) == 0){
		if(run_env.anonymous_enable){
			strcpy(user_env.user_name, username);	
			write(user_env.connect_fd, inf_buf, strlen(inf_buf));
		#ifdef DEBUG
		printfConsole(inf_buf);
		#endif
		}
		else {
			write(user_env.connect_fd, no_anonymous, strlen(no_anonymous));
		#ifdef DEBUG
		printfConsole(no_anonymous);
		#endif
		}			
	}

	/*anonymous client authentication*/
	else {
		strcpy(user_env.user_name, username);		
		write(user_env.connect_fd, inf_buf, strlen(inf_buf));
		#ifdef DEBUG
		printfConsole(inf_buf);
		#endif
	}

	return 0;
}
/*implement of USER*/

/*implement of PASS*/
int do_pass(char *pass)
{
	char mess[50];
	char password[16];
	char md[16];
	char name[16];
	const char log_error[] = "Login failed!\r\n";
	const char logged[] = "503 You have already logged in.\r\n";
	FILE *fp;
	size_t len = 0;
	ssize_t k;
	char *line = NULL, *tmp;
	int i, j, pass_len, id;

	if (user_env.login_in == TRUE) {
		write(user_env.connect_fd, logged, strlen(logged));
		return -1;
	}

	if (strcmp(user_env.user_name, "anonymous") == 0) {
		snprintf(mess, 50, "230 User anonymous logged in.\r\n");
		user_env.login_in = TRUE;
		user_env.enable_upload = FALSE;
		write(user_env.connect_fd, mess, strlen(mess));
		return 0;
	} 

	if ((fp = fopen(run_env.user_pass_file, "r")) == NULL) {
		write_log("open user_pass_file error",0);
		write(user_env.connect_fd, log_error, strlen(log_error));
		return -1;
	}

	while ((k = getline(&line, &len, fp)) != -1) {
		fscanf(fp, "%d", &id);
		tmp = line;
		j = 0;
		for (i = 0; ; i++) {
			if (tmp[i] == ':') {
				i = i + 2;
				break;
			}
		}
		for( ; ; i++) {
			if(tmp[i] == ':') {
				name[j] = '\0';
				break;
			} else {
				name[j++] = tmp[i];
			}
		}

		if (strcmp(name, user_env.user_name) == 0)
			break;
	}

	if (k == -1) {
		snprintf(mess, 50, "530 Login incorrect.\r\n");
		write(user_env.connect_fd, mess, strlen(mess));
		free(line);
		return -1;
	}

	user_env.user_id = id;
	tmp = line;
	for (i = 0, j = 0; tmp[i] != '\n' && j < 6; i++) {
		if (tmp[i] == ':') {
			j++;
		}
		if (j == 4) {
			i = i + 2;
			if (tmp[i] == 'w') {
				user_env.enable_upload = TRUE;
			}			
		}
	}

  #if 0
	for (j = 0; j < 16; ) {
		password[j++] = tmp[i++];
	}
  #else
	memset( password, 0, sizeof(password) );
	for (j = 0; j < 16 && tmp[i] != '\0' && tmp[i] != '\n'; ) {
		password[j++] = tmp[i++];
	}
  #endif
	free(line);

	pass_len=strlen(pass);
  #if 0
	MD5((const unsigned char *)pass, pass_len, (unsigned char *)md);
  #else
	strcpy( md, pass );
  #endif

	fclose(fp);

  #if 0
	if (!memcmp(password, md, 16) ) {
  #else
	if (!strcmp(password, md) ) {
  #endif
		snprintf(mess, 50, "230 User %s logged in.\r\n", user_env.user_name);
		user_env.login_in = TRUE;
		write(user_env.connect_fd, mess, strlen(mess));
		return 0;
	} else {
		snprintf(mess, 50, "530 Login incorrect.\r\n");
		write(user_env.connect_fd, mess, strlen(mess));
		return -1;
	}

}

/*implement of STAT*/

static char* _get_line_info(struct stat *stat_buf, char *buf, int *width)
{
	char att[11] = "----------", tm[26];
	unsigned int t = S_IRUSR;
	int i;
  #if 1
	struct tm * timeinfo;
  #endif

	if (S_ISREG(stat_buf->st_mode)) {
		att[0] = '-';
	}

	else if (S_ISDIR(stat_buf->st_mode)) {
		att[0] = 'd';
	}
	else if (S_ISCHR(stat_buf->st_mode)) {
		att[0] = 'c';
	}
	else if (S_ISBLK(stat_buf->st_mode)) {
		att[0] = 'b';
	}
	else if (S_ISFIFO(stat_buf->st_mode)) {
		att[0] = 'p';
	}
	else if (S_ISLNK(stat_buf->st_mode)) {
		att[0] = 'l';
	}
	else if (S_ISSOCK(stat_buf->st_mode)) {
		att[0] = 's';
	}
	for (i = 1;i < 10;i++,t >>= 1) {
		if (stat_buf->st_mode & t) {
			switch (i % 3) {
			case 1: att[i] = 'r';
				break;
			case 2: att[i] = 'w';
				break;
			case 0: att[i] = 'x';
			}
		}
	}
  #if 0
	if (snprintf(tm, 25, "%s", ctime(&(stat_buf->st_ctime))) == -1) {
		write_log("Read system time error!", 0);
	}
  #else
	  timeinfo = localtime ( &(stat_buf->st_ctime) );
	  strftime (tm,25,"%b %d %H:%M",timeinfo);
  #endif
	if (snprintf(buf, MAX_PATH, "%s% *d %*s %*s% *d %s", att, width[2], (size_t)stat_buf->st_nlink, width[0],
				run_env.visible_user_name,width[1], run_env.visible_group_name,width[3], 
				(size_t)stat_buf->st_size, tm) == -1){
		write_log("The path string is overflow!", 0);
	}
	return buf;	
}

static int _get_int_len(int n)
{
	int i;
	for (i = 1;n /= 10;i++) {
		;
	}
	return i;
}

static int _stat_no_arg(void)
{
	char msg[MAX_MSG_LEN]={0};
	if (snprintf(msg, MAX_MSG_LEN, "211-Status for user %s from %s:\r\n"
			"211-Stored %d files,%d KB\r\n211-Retrieved %d files,%d KB\r\n211 End of Status.\r\n",
			user_env.user_name, user_env.client_ip, user_env.upload_files,
			user_env.upload_kbytes, user_env.download_files, user_env.download_kbytes) == -1) {
		write_log("The message is overflow.",0);
	}
	return write(user_env.connect_fd, msg, strlen(msg));
}

static int _stat_with_arg(const char *cmd_arg)
{
	char buf[MAX_PATH], full_path[MAX_PATH], tmp[MAX_PATH];
	const char not_found[] = "450 Target path doesn't exist.\r\n";
	struct dirent *direntp;
	DIR *target_dir;
	struct stat stat_buf;
	int max_width[4] = {0}; /*max length of user name, group name,link number and the file length number*/
	int t;

	if (*cmd_arg == '/') {
		if (snprintf(buf, MAX_PATH, "%s", cmd_arg) == -1) {
			write_log("Path string overflows.", 0);
		}
	}
	else {
		if (snprintf(buf, MAX_PATH, "%s%s", user_env.current_path, cmd_arg) == -1) {
			write_log("Path string overflows.", 0);
		}
	}

	strcpy(full_path, buf); /*the length of buf won't greater than the full_path so it's safe.*/

	if (!(target_dir = opendir(buf))) {
		write(user_env.connect_fd, not_found, strlen(not_found));
		return closedir(target_dir);
	}

	max_width[0] = (t = strlen(run_env.visible_user_name)) > max_width[0]?t:max_width[0];
	max_width[1] = (t = strlen(run_env.visible_group_name)) > max_width[1]?t:max_width[1];

	while ((direntp = readdir(target_dir)) != NULL) {
		if (snprintf(buf, MAX_PATH, "%s/%s", full_path, direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}

		if (stat(buf, &stat_buf) == -1) {
			write_log("Read local file status error", 0);
			closedir(target_dir);
			return -1;
		}

		max_width[2] = (t = _get_int_len(stat_buf.st_nlink)) > max_width[2]?t:max_width[2];
		max_width[3] = (t = _get_int_len(stat_buf.st_size)) > max_width[3]?t:max_width[3];
	}
	rewinddir(target_dir);

	if (snprintf(buf, MAX_PATH, "211-Status of %s\r\n", cmd_arg) == -1) {
		write_log("Path string overflows.",0);
	}

	write(user_env.connect_fd, buf, strlen(buf));

	while ((direntp = readdir(target_dir)) != NULL) {
		if (*(direntp->d_name) == '.') {
			continue;
		}
		if (snprintf(buf, MAX_PATH,"%s/%s", full_path, direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}

		if (stat(buf, &stat_buf) == -1) {
			write_log("Read local file status error", 0);
			closedir(target_dir);
			return -1;
		}
		if (snprintf(buf, MAX_PATH, "211-%s %s\r\n", _get_line_info(&stat_buf, tmp, max_width),
				direntp->d_name) == -1) {
			write_log("Path string overflows.", 0);
		}
		write(user_env.connect_fd, buf, strlen(buf));
	}

	if (snprintf(buf, MAX_PATH, "211 End of Status.\r\n") == -1) {
		write_log("Path string overflows.", 0);
	}
	write(user_env.connect_fd, buf, strlen(buf));
	return closedir(target_dir);
}

int do_stat(const char *cmd_arg)
{
	if (!strlen(cmd_arg)) {
		return _stat_no_arg();
	}

	return _stat_with_arg(cmd_arg);
}
/*end of the implement of STAT*/

int do_list(char *filename)
{
	int m_width[4]={3,10,10,10};
	char each_dir_inf[BUF_LEN] = {0};
	char buf[BUF_LEN] = {0};
	const char finished[] = "226 Transfer complete.\r\n";
	const char success[] = "150 Opening ASCII mode data connection for file list.\r\n";
	const char fail[] = "450 No such file or directory.\r\n";
	DIR *dir_inf_str;
	struct dirent *dirp;
	struct stat file_inf;
	char user_path[PATH_NAME_LEN] ={""};
	char inte_dir_inf[BUF_LEN]={0};

	write(user_env.connect_fd, success, strlen(success));
	if(filename[0]!='/')	{
		snprintf(buf, BUF_LEN, "%s/%s", user_env.current_path, filename);
	}
	else {
		snprintf(buf, BUF_LEN, "%s", filename);
	}
	if (stat(filename, &file_inf) == 0){
		if (!S_ISDIR(file_inf.st_mode)) {
			snprintf(inte_dir_inf, BUF_LEN, "%s %s\r\n",
					_get_line_info(&file_inf, each_dir_inf,
						m_width), filename);
			write(user_env.data_fd, inte_dir_inf,
				strlen(inte_dir_inf));
			write(user_env.connect_fd, finished, strlen(finished));
			close(user_env.data_fd); 
			return 0;
		}
	}
#ifdef DEBUG
	printfConsole("Get here with %s\n", buf);
#endif
	if((dir_inf_str = opendir(buf)) != NULL){
		while((dirp = readdir(dir_inf_str)) != NULL){
			memset(each_dir_inf, 0, BUF_LEN);
			memset(inte_dir_inf, 0, BUF_LEN);
			snprintf(user_path, PATH_NAME_LEN, "%s/%s", buf, dirp->d_name);
			if(stat(user_path, &file_inf) != -1){
				snprintf(inte_dir_inf, BUF_LEN, "%s %s\r\n",
					_get_line_info(&file_inf, each_dir_inf,
						m_width), dirp->d_name);
				write(user_env.data_fd, inte_dir_inf,
				strlen(inte_dir_inf));
			}
		}
		write(user_env.connect_fd, finished, strlen(finished));
		close(user_env.data_fd);
	} 
	else {
		write(user_env.connect_fd, fail, strlen(fail));
		close(user_env.data_fd);
		return -1;
	}
	closedir(dir_inf_str);
	return 0;
}

int do_nlst(char *filename)
{
	char            each_dir_inf[BUF_LEN] = {0};
	char            buf[BUF_LEN] = {0};
	const char      finished[] = "226 Transfer complete.\r\n";
	const char      success[] = "150 Opening ASCII mode data connection for file list.\r\n";
	const char      fail[] = "450 No such file or directory.\r\n";
	DIR            *dir_inf_str;
	struct dirent  *dirp;
	struct stat     file_inf;
	char            user_path[PATH_NAME_LEN] = {""};
	char            inte_dir_inf[BUF_LEN] = {0};

	write(user_env.connect_fd, success, strlen(success));
    if(filename[0] != '/')
    {
        snprintf(buf, BUF_LEN, "%s/%s", user_env.current_path, filename);
    }
	else
    {
		snprintf(buf, BUF_LEN, "%s", filename);
	}
	if (stat(filename, &file_inf) == 0)
    {
		if (!S_ISDIR(file_inf.st_mode))
        {
			snprintf(inte_dir_inf, BUF_LEN, "%s\r\n", filename);
			write(user_env.data_fd, inte_dir_inf, strlen(inte_dir_inf));
			write(user_env.connect_fd, finished, strlen(finished));
			close(user_env.data_fd); 
			return 0;
		}
	}
#ifdef DEBUG
	printfConsole("Get here with %s\n", buf);
#endif
	if((dir_inf_str = opendir(buf)) != NULL)
    {
		while((dirp = readdir(dir_inf_str)) != NULL)
        {
			memset(each_dir_inf, 0, BUF_LEN);
			memset(inte_dir_inf, 0, BUF_LEN);
			snprintf(user_path, PATH_NAME_LEN, "%s/%s", buf, dirp->d_name);
			snprintf(inte_dir_inf, BUF_LEN, "%s\r\n", dirp->d_name);
			write(user_env.data_fd, inte_dir_inf, strlen(inte_dir_inf));
		}
		write(user_env.connect_fd, finished, strlen(finished));
		close(user_env.data_fd);
	} 
	else
    {
		write(user_env.connect_fd, fail, strlen(fail));
		close(user_env.data_fd);
		return -1;
	}

	closedir(dir_inf_str);

	return 0;
}

/* 命令MKD的处理 */
int do_mkd(const char *path)           /*处理命令MKD的入口*/
{
	char str[PATH_NAME_LEN] = {""};
	if (user_env.enable_upload == 1) {                  /*判断权限*/
		if (path[0] == '/') {                   /*参数是路径名?*/
                	path=strcat(str,path);
             	}
		else {                                 /*参数是目录名?*/
			strcpy(str, user_env.current_path);                
			if(str[strlen(str)-1] != '/') strcat(str, "/");
                	path=strcat(str, path);
		}
		if(_stat_mkd(path) == 0) {
			return 0;
        	}
        	else {
        		return -1;
		}
 	}
	else {
            _stat_error_421();
            return -1;
        }
     }

static int _stat_mkd(const char *path)
{     
	char buf[50+PATH_NAME_LEN] = {};
#ifdef DEBUG
	printfConsole("mkd: path=%s \r\n", path);
#endif
	if (mkdir(path, S_IRWXU) == 0) {              /*创建目录成功*/
		snprintf(buf, 50+PATH_NAME_LEN, "257 Directory successfully created:%s.\r\n", path);
		write(user_env.connect_fd,buf,strlen(buf)+1);
		return 0;
	}
	else {
		if(errno == EEXIST){
			_stat_error1_501();
        	} 
		else if (errno == ENAMETOOLONG) {
			_stat_error2_501();
		} 
    		else {
			_stat_fail_450();
		}
		return -1;
	}
}

/*
 *命令RMD的处理
 */
int do_rmd(const char *path)                /*处理命令RMD的入口*/
{
	char str[PATH_NAME_LEN]={""};
	if (user_env.enable_upload == 1) {	/*判断权限*/
		if (path[0] == '/') {	/*参数是路径名?*/
			path=strcat(str,path);
		}
		else {	/*参数是目录名?*/
			strcpy(str, user_env.current_path);
			if(str[strlen(str)-1] != '/') {
				strcat(str, "/");
			}
			path=strcat(str,path);
		}
		if(_stat_rmd(path) == 0) {
			return 0;
		}
		else {
			return -1;
		}
	}
        else {
            _stat_error_421();
            return(-1);
	}
}

static int _stat_rmd(const char *path)
{
	const char buf[] = "250 RMD command successful.\r\n";
#ifdef DEBUG
	printfConsole("rmdir at %s \r\n", path);
#endif
	if (rmdir(path) == 0) {	/*删除目录成功*/
		write(user_env.connect_fd, buf, strlen(buf));
		return 0;
        }
	else {
		if(errno == ENOENT) {
			_stat_error3_501();
		} 
		else if (errno == ENAMETOOLONG) {
			_stat_error2_501();
		} 
		else {
			_stat_fail_450();
		}
		return -1;
	}
}

/*
 *命令DELE的处理
 */
int do_dele(const char *path)          /*处理命令DELE的入口*/
{
	char str[PATH_NAME_LEN] = {""};
	if (user_env.enable_upload == 1) {    /*判断权限*/
		if (path[0] == '/') {         /*参数是路径名?*/
			path = strcat(str, path);
		}
		else{       /*参数是目录名?*/
			strcpy(str, user_env.current_path);
			if(str[strlen(str)-1] != '/') {
				strcat(str, "/");
			}
			path=strcat(str,path);
		}
		if(_stat_dele(path) == 0) {
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
		_stat_fail_550();
		return -1;
	}
}

int _stat_dele(const char *path)
{
	const char buf[] = "250 File sucessfully deleted.\r\n";
#ifdef DEBUG
	printfConsole("dele at %s \r\n",path);
#endif
	if(unlink(path) == 0) {	/*删除文件成功*/
		write(user_env.connect_fd, buf, strlen(buf));
		return 0;
	}
	else {
		if(errno == ENOENT) {
			_stat_error3_501();
		}
		else if(errno == ENAMETOOLONG) {
			_stat_error2_501();
		}
		else { 
			_stat_fail_450();
		}
		return -1;
	} 
}

/*
 *命令RETR的处理
 */
static int _stat_retr(const char *path)
{ 
	int fd;
	ssize_t i = 0;
	char buf[BUF_LEN]={""};      
#ifdef DEBUG
	printfConsole("retr from %s\n",path);
#endif
	fd = open(path, O_RDONLY);
	if(fd != -1) {
		_stat_success_150();
		while((i = read(fd, buf, BUF_LEN)) != 0) {
			if (i == -1) {
				close(user_env.data_fd);
				_stat_fail_501();
				return -1;
			}
			else {
				write(user_env.data_fd, buf, i);
				user_env.download_kbytes += i/1000;
			}
		}
		user_env.download_files++;
		r_close(fd);
		close(user_env.data_fd); 
		_stat_success_226();
		return 0;
	} else {
		close(user_env.data_fd);
		_stat_fail_501();
		return -1;
	} 
}

int do_retr(const char *path)
{
	char str[PATH_NAME_LEN]={""};
	if (strlen(path) == 0) {
		close(user_env.data_fd); 
		return failed("RETR");
	}
	if (path[0] == '/') {                               /*参数是路径名?*/
		path = strcat(str, path);
	} else {                      /*参数是文件名?*/
		strcpy(str, user_env.current_path);
		if(str[strlen(str)-1] != '/') {
			strcat(str,"/");
		}
		path = strcat(str,path);
	}
	return _stat_retr(path);
}

/*命令执行失败*/
static void _stat_fail_450(void)
{
	const char buf[] = "450 File operation failed.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

/*出现错误*/
static void _stat_error_421(void)
{
	const char buf[] = "421 Service not available, closing control connection.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
	do_quit();
}

/*命令执行失败*/
static void _stat_fail_550(void)
{
	const char buf[] = "550 Requested action not taken. File unavailable.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

/*出现错误*/
static void _stat_error1_501(void)
{
	const char buf[] = "501 Wrong arguments, the filename exists.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

static void _stat_error2_501(void)
{
	const char buf[] =
		"501 Diretory or file name is too long.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

static void _stat_error3_501(void)
{
	const char buf[] =
		"501 Arguments wrong,the file or directory does not exists!\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

static void _stat_success_150(void)
{ 
	const char buf[] = "150 File status okay; about to open data connection.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

static void _stat_success_226(void)
{ 
	const char buf[] = "226 Closing data connection."
             "Requested file-action succeed.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}
 
static void _stat_fail_501(void)
{
	const char buf[] = "501 Syntax error in parameters or arguments.\r\n";
	write(user_env.connect_fd, buf, strlen(buf));
}

int do_mode(const char *arg)
{
	const char succ[] = "200 MODE S OK.\r\n";
	const char fail[] = "504 Command not implemented for that parameter.\r\n";
	if ((strlen(arg) == 1) && ((*arg == 's') || (*arg == 'S'))) {
		write(user_env.connect_fd, succ, strlen(succ));
		return 0;
	}
	else {
		write(user_env.connect_fd, fail, strlen(fail));
		return 1;
	}
}

static int _get_local_ip_address(int sock, char* ip_addr)
{
	char *ip = NULL;  
	int fd = sock, intrface;
	struct ifreq buf[MAX_INTERFACES];
	struct ifconf ifc;

	ifc.ifc_len = sizeof buf;
	ifc.ifc_buf = (caddr_t) buf;
	if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			while (intrface-- > 0) {
				if (!(ioctl(fd, SIOCGIFADDR, (char*)&buf[intrface]))) {
					ip = (inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
					strcpy(ip_addr, ip);
					break;
				}
			} 
	} 
	return 0;
}

int do_pasv(void)
{
	char ip_addr[16] = {0};  /*I think 16 is just enough.*/
	char port_buf[64] = {0};
	/*const char *pasv_ready = "125 Data connection already open; transfer starting.\r\n";*/
	const char pasv_fail[] = "425 Can't open data connection.\r\n";
	int sock, ret;
	int data_port; 
	int opt = SO_REUSEADDR;
	socklen_t i;
	struct sockaddr_in data, cliaddr;
	unsigned int port, port1, port2;
	struct timeval tv;
	char *tmp;

	tv.tv_sec = run_env.data_connection_timeout;
	tv.tv_usec = 0;
	bzero(&data, sizeof(data)); 
	data_port = 0;
	data.sin_family = AF_INET;    /*建立数据连接*/
	data.sin_port = htons(data_port);
	data.sin_addr.s_addr = htonl(INADDR_ANY);

	i = sizeof(cliaddr);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
#ifdef DEBUG
		perrorConsole("socket");
#else
		write_log("socket failed!", 0);
#endif
		return -errno;
	} 
	_get_local_ip_address(sock, ip_addr);
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
  	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sock, (struct sockaddr *)&data, i) != 0) {
#ifdef DEBUG
		perrorConsole("bind");
#else
		write_log("Can't bind!", 0);
#endif
		return -errno;
	}

	getsockname(sock, (struct sockaddr *)&data, (socklen_t*) &i); 
#ifdef DEBUG
	printfConsole("pasv: ip=%s\n",  ip_addr);
	printfConsole("pasv: port=%d\n", ntohs(data.sin_port));
#endif
	port = ntohs(data.sin_port);
	port1 = port / 256;
	port2 = port % 256;
	while ((tmp = strchr(ip_addr, '.')) != NULL) {
		*tmp = ',';
		tmp++;
	}

	if(listen(sock, 1) != 0){
#ifdef DEBUG
		perrorConsole("listen");
#else
		write_log("Can't listen!", 0);
#endif
		return -errno;
	}

	snprintf(port_buf, 64, "227  Entering Passive Mode (%s,%d,%d).\r\n", ip_addr, port1, port2);
	write(user_env.connect_fd, port_buf, strlen(port_buf));

	ret = accept(sock, (struct sockaddr *)&cliaddr, &i);
	if(ret!=-1){
		user_env.data_fd = ret;
		//write(user_env.connect_fd, pasv_ready, strlen(pasv_ready));
		close(sock);		
		return 0;				
	} 
	else{
		write(user_env.connect_fd, pasv_fail, strlen(pasv_fail));
		close(sock);		
		return -errno;
	}
} 

int do_syst(void)
{
	const char wel[] = "215 UNIX Type: L8\r\n";
	
	if ((write(user_env.connect_fd, wel, strlen(wel))) == -1) {
	#ifdef DEBUG		
		perrorConsole("wirte error");
	#else
		write_log("write error in do_syst()", 0);
	#endif
	}
	return 0;	
}

int do_noop(void)
{
	const char mess[] = "200 NOOP command successful.\r\n";
	if ((write(user_env.connect_fd, mess, strlen(mess))) == -1) {
	#ifdef DEBUG		
		perrorConsole("wirte error");
	#else
		write_log("write error in do_noop()", 0);
	#endif
	}
	return 0;
}

int do_type(char *arg)
{
	if (strcasecmp(arg, "I") == 0) {
		user_env.ascii_on = FALSE;
		write(user_env.connect_fd, "200 Type set to I.\r\n", 20);
	} else if (strcasecmp(arg, "A") == 0) {
		user_env.ascii_on = TRUE;
		write(user_env.connect_fd, "200 Type set to A.\r\n", 20);
	} else {
		write(user_env.connect_fd, "500 Type not understood.\r\n", 26);	
		return -1;
	}

	return 0;
}

int do_stru(char *arg)
{
	const char fail[] = "501 'STRU' not understood.\r\n";
	const char succ[] = "200 Structure set to F.\r\n";
	const char unsupported[] = "504 Unsupported structure type.\r\n";
	const char unknown[] = "501 Unrecognized structure type.\r\n";

	if (strcmp(arg, "") == 0) {
		write(user_env.connect_fd, fail, strlen(fail));
		return -1;
	} else if(strcasecmp(arg, "F") == 0) {
		write(user_env.connect_fd, succ, strlen(succ));
	} else if (strcasecmp(arg, "P") == 0
				|| strcasecmp(arg, "R") == 0) {
		write(user_env.connect_fd, unsupported, strlen(unsupported));
	} else {
		write(user_env.connect_fd, unknown, strlen(unknown));	
	}

	return 0;
}

/*
 *Note! When this is called in parse_cmd, it means
 *we received an 'ABOR' when no data connection exists.
 *So we just reply a 226 and do nothing actually.
 */
int do_abor(char *arg)
{
	const char fail[] = "501 Can not understood.\r\n";
	const char succ[] = "226 Abort successful.\r\n";

	if (strcmp(arg, "") != 0) {
		write(user_env.connect_fd, fail, strlen(fail));
		return -1;
	} else {
		write(user_env.connect_fd, succ, strlen(succ));	
	}

	return 0;
}

int do_cwd (char *dir)
{
	if (_chdir(dir) < 0) {
		return -1;
	}
	else {
		_write_to_socket("250 Command ok.\r\n");
		return 0;
	}
}


int do_cdup(void)
{
	if (_chdir("..") < 0) {
		return -1;
	}
	else {
		_write_to_socket("250 CDUP Command ok.\r\n");
		return 0;
	}
}

int do_pwd(void)
{
	char mess[PATH_NAME_LEN+6] = {0};
	snprintf(mess, 4096, "257 \"%s\"\r\n", user_env.current_path);
	_write_to_socket(mess);
	return 0;
}

int do_rnfr(void)
{
	if (_write_to_socket("350 Please send the RNTO command.\r\n") < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

int do_rnto(const char *old_path, const char *new_path)
{
	char *mess;
#ifdef DEBUG
	printfConsole("%s,%s\n", old_path, new_path);
#endif
	if (user_env.enable_upload != 1) {
		write(user_env.connect_fd, "550 Permission denied!\r\n",
			strlen("550 Permission denied!\r\n"));
		return -1;
	}

	if (rename(old_path, new_path) < 0) {
		switch (errno) {
		case EISDIR:
			mess = "553 New path is a directory.\r\n";
			break;
		case EBUSY:
			mess = "502 The files are in use now.\r\n";
			break;
		default:
			mess = "501 Can't rename this file.\r\n";
		}
		_write_to_socket(mess) ;
		return -1;
	}
	_write_to_socket("250 Command succeed.\r\n") ;
	return 0;
}

int do_port(char *arg)
{
	struct sockaddr_in client;
	int error;
	int retval;
	int sock;
	int port;
	char *addr;

	if (_analysis_ipaddr(arg, &addr, &port) < 0) {
		return -1;
	}
#ifdef DEBUG
	printfConsole("addr=%s, port=%d\n", addr, port);
#endif

	bzero((char *)&client, sizeof(client)) ;
	client.sin_port = htons(port);
	if (inet_pton(AF_INET, addr, &client.sin_addr) == 0) {/*convert decimal address to binary*/
		_write_to_socket("501 Incorrect IP address.\r\n");
		return -1;
	}
	client.sin_family = AF_INET;
	if ((_ignore_sigpipe() == -1) || ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)) {
		_write_to_socket("421 Failed to create data connection.\r\n");
		return -1;
	}
	if (((retval = connect(sock, (struct sockaddr *)&client, sizeof(client))) == -1)
			&& ((errno ==  EINTR)||(errno == EALREADY))) {
		/*create data connection*/;
	}
	if (retval == -1) {
		error = errno;
		while ((close(sock) == -1) && (errno == EINTR))
			/*do nothing*/;
		errno = error;
		_write_to_socket("421 Failed to create data connection.\r\n");
		return -1;
	}
	_write_to_socket("200 Succeed to create data connection.\r\n");
	user_env.data_fd = sock;
	return 0;
}

static int _ignore_sigpipe(void)
{
	struct sigaction act;
	if (sigaction(SIGPIPE,(struct sigaction *)NULL, &act) == -1) {
		return -1;
	}
	if (act.sa_handler == SIG_DFL) {
		act.sa_handler = SIG_IGN;
		if (sigaction(SIGPIPE,&act, (struct sigaction *)NULL) == -1) {
			return -1;	
		}
	}
	return 0;
}

static int _analysis_ipaddr(char *str, char **re_addr, int *re_port)
{
	static char addr[16];
	int ip[4];
	int port[2];
	int m;
	int i = 0;
	sscanf(str, "%d,%d,%d,%d,%d,%d", &ip[0],  &ip[1], &ip[2], &ip[3], &port[0], &port[1]);
	while(i < 4) {
		if((ip[i] > 255) || (ip[i] < 0)) {
			return -1;
		}
		i++;
	} 
	m = port[0]*256 + port[1];
	if(m > 65535) {
		return -1;	
	}
	snprintf(addr, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	*re_port = m;
	*re_addr = addr;
	return 0;
}

static int _write_to_socket (char *buf)
{ 
	int byteswrite;
	int nbyte = strlen(buf);
	while (((byteswrite = write(user_env.connect_fd, buf, nbyte)) == -1) && (errno == EINTR));
	if (byteswrite < 0) { /*If cannot write to socket,close all connections and kill the process*/
		_close_connection(user_env.connect_fd);
		   /*？是否要关闭数据连接？*/
		write_log("remote host computer did not responsed.", 1);
	}
	return byteswrite;
}

static int _close_connection(int fd) 
{
	int i;
	if (fd < 0) {
		return 0;    /*there is not the file descriptor*/
	}
	while ((i = close(fd)) < 0 && (errno == EINTR)) {
		/*do nothing*/;
	}
	if (i < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

static int _chdir(char *cdir)
{
	char *buffer = NULL;
	int size = 128;

	if (strlen(cdir) == 0) {
		strcpy(user_env.current_path,"/");
		if (chdir("/") < 0) {
			write_log("chdir failed", 0);
			return -1;
		}
		return 0;
	}

	if ((chdir(cdir)) < 0) {
		#ifdef DEBUG
		perrorConsole("chdir");
		#endif
		_write_to_socket("501 Can't change directory.\r\n");
		return -1;	 	
	}
	/*检查是否合法,即是否超过跟目录*/
	if ((buffer = malloc(size)) == NULL) {
		_write_to_socket("550 Can't change directory.\r\n");
		write_log("malloc failed", 0);
		return -1;
	}
	while ((getcwd(buffer, size) == NULL) && (errno == ERANGE)){
		size *= 2;
		if((buffer = realloc(buffer, size)) == NULL){
			_write_to_socket("550 Can't change directory.\r\n");
			write_log("realloc failed", 0);
			return -1;
		}
	}

	strcpy(user_env.current_path, buffer);
	free(buffer);
	return 0;
}

int do_quit(void)
{
	const char wel[] = "221 Goodbye.\r\n";

	while ((write(user_env.connect_fd, wel, strlen(wel))) == -1) {
#ifdef DEBUG
		perrorConsole("wirte error");
#else
		write_log("wirte error", 0);
#endif
	}
	close(user_env.connect_fd);
	free_sources();
	kill(getpid(), SIGTERM);
	return 0;
}

int do_stor(char *arg)
{
	char pathname[MAX_PATH]={""};
	const char stor_ok[] = "226 Transfer complete.\r\n";
	int sd, fd;
	ssize_t rsize;
	char buff[BUF_LEN];

#ifdef DEBUG
	printfConsole("****STOR; arg = %s\n", arg);
	printfConsole("user_env.current_path=%s\n", user_env.current_path);
#endif
	if (!user_env.enable_upload) {
		write(user_env.connect_fd, "550 Permission denied.\r\n", strlen("550 Permission denied.\r\n"));
		close(user_env.data_fd);
		write_log("Attempt to write.", 1);
		return -1;
	}
	if (arg[0]=='/'){
#if 0
		strcpy(pathname, run_env.ftp_dir);
		if(pathname[strlen(pathname)-1] != '/') {
			strcat(pathname, "/");
		}
		if(arg[0]=='/') {
			arg++;
		}
#endif
		strncpy(pathname, arg, MAX_PATH);
	}
	else {
		strcpy(pathname, user_env.current_path);
		if(pathname[strlen(pathname)-1] != '/')
			strcat(pathname, "/");
		strcat(pathname, arg);
	} 
#ifdef DEBUG
	printfConsole("pathname=%s.\n", pathname);
#endif
	fd = open(pathname, O_RDWR|O_CREAT);
	if (fd < 0) {
		write(user_env.connect_fd, "550 Permission denied.\r\n", 23);
		close(user_env.data_fd);
#ifdef DEBUG
		perrorConsole("open");
#else
		write_log("Open error.", 0);
#endif
		return -errno;
	}
	_stat_success_150();
#ifdef DEBUG
	printfConsole("%s create OK! \n", pathname);
#endif
	sd = user_env.data_fd;	
	for(;;){
		rsize = read(sd, buff, BUF_LEN);
		if(rsize == 0) {
			break;
		}
		if (rsize < 0) {
		#ifdef DEBUG
			perrorConsole("read");
		#else
			write_log("Read socket error.", 0);
		#endif
			break;
		}
		write(fd, buff, rsize);
		user_env.upload_kbytes += rsize/1024;
	}
	r_close(fd);
	r_close(sd);
	write(user_env.connect_fd, stor_ok, strlen(stor_ok));
	user_env.upload_files++;
#ifdef DEBUG
	printfConsole("%d files, %d KB.\n", user_env.upload_files, user_env.upload_kbytes);
#endif
	return 0;
}

/*reply the wrong or unsupported command*/
int failed(const char *s)
{
	char msg500[MAX_PATH] = {0};
	int m_len;
	if ((m_len = snprintf(msg500, MAX_PATH, 
			"500 \'%s\' command is not supported.\r\n", s)) >= MAX_PATH || m_len == -1) {
		write_log("Too long command got.", 0);
		return -1;
	}
	write(user_env.connect_fd, msg500, strlen(msg500));
	return 0;
}
