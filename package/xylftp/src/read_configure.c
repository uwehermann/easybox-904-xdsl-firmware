/*
 * Copyright (c) 2007,西安邮电学院Linux兴趣小组
 * All rights reserved.
 *
 * 文件名称：read_configure.c
 * 摘要：读取配置文件内容
 *
 * 当前版本：0.1
 * 作者：林峰
 * 完成日期：2007年5月15日
 *
 * 修改者：林峰
 * 修改日期：2007年6月6日
 */
#include "xylftp.h"
#define CONFIG_NUM 15	/*item number of the configue file*/
static int _read_line(int fd,char *buf);
static int _analyze_para(char *buf,int *array);
static int _check_configure(int *array);

extern struct user_env user_env;
extern struct run_env run_env;

#ifdef DEBUG
void print(void);

void print(void)
{
	printfConsole("run_env.anonymous_enable = %d\n",run_env.anonymous_enable);
	printfConsole("run_env.ftp_port = %d\n",run_env.ftp_port);
	printfConsole("run_env.local_umask = %d\n",run_env.local_umask);
	printfConsole("run_env.log_file_enable = %d\n",run_env.log_file_enable);
	printfConsole("run_env.log_file = %s\n",run_env.log_file);
	printfConsole("run_env.idle_session_timeout = %d\n",run_env.idle_session_timeout);
	printfConsole("run_env.data_connection_timeout = %d \n",run_env.data_connection_timeout);
	printfConsole("run_env.ftpd_banner = %s\n",run_env.ftpd_banner);
	printfConsole("run_env.max_clients = %d\n",run_env.max_clients);
	printfConsole("run_env.max_links = %d\n",run_env.max_links);
	printfConsole("run_env.passive_port = %d,%d\n",run_env.passive_port_min,run_env.passive_port_max);
	printfConsole("run_env.ftp_dir = %s\n",run_env.ftp_dir);
	printfConsole("run_env.user_pass_file = %s\n",run_env.user_pass_file);
	printfConsole("run_env.visible_user_name = %s\n",run_env.visible_user_name);
	printfConsole("run_env.visible_group_name = %s\n",run_env.visible_group_name);
}
#endif

static int _analyze_para(char *buf,int *array)
{
	char *tmp;
	char *option = NULL,*value = NULL;			/*used to store the option strings and value strings*/
	char *saveptr1,*saveptr2;		/*used for the function strtok_r*/
	if ((tmp = strtok_r(buf,"=",&saveptr1)) == NULL) {
		perrorConsole("Error in configure file\n");
		return -1;
	}		/*get the option string*/
	if ((option = malloc(sizeof(char) * (strlen(tmp) + 1))) == NULL) {
		perrorConsole("No enough memory!\n");
		goto ret0;
	}	
	strcpy(option,buf);			/*store option*/
	if((tmp = strtok_r(NULL,"=",&saveptr1)) == NULL) {
		perrorConsole("Error in configure file\n");
		return -1;
	}
	if ((value = malloc(sizeof(char) * ((strlen(tmp) + 1)))) == NULL) {
		perrorConsole("No enough memory!\n");
		goto ret1;
	}
	strcpy(value,tmp);			/*store value*/

/*codes below are used to value the parameters of the struct run_env*/
	if (!strncmp(option,"Anonymous_enable", strlen("Anonymous_enable"))) {
		if (!strcmp(value,"YES")) {
			run_env.anonymous_enable = TRUE;	
		} else {
			run_env.anonymous_enable = FALSE;
		}
		array[0] = 1;
	} else if (!strncmp(option,"FTP_port", strlen("FTP_port"))) {
		run_env.ftp_port = (unsigned int)(atoi(value));
		array[1] = 1;
	} else if (!strncmp(option,"Local_umask", strlen("Local_umask"))) {
		run_env.local_umask = (unsigned int)(atoi(value));
		array[2] = 1;
	} else if (!strncmp(option,"Log_file_enable", strlen("Log_file_enable"))) {
		if (!strcmp(value,"YES")) {
			run_env.log_file_enable = 1;
		} else {
			run_env.log_file_enable = 0;
		}
		array[3] = 1;
	} else if (!strncmp(option,"Log_file",strlen("Log_file_enable"))) {
		if ((run_env.log_file = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			perrorConsole("Not enough memory!\n");
			goto ret2;
		}
		strcpy(run_env.log_file,value);
		array[4] = 1;
	} else if (!strncmp(option,"Idle_session_timeout", strlen("Idle_session_timeout"))) {
		run_env.idle_session_timeout = (unsigned int)(atoi(value));
		array[5] = 1;
	} else if (!strncmp(option,"Data_connection_timeout", strlen("Data_connection_timeout"))) {
		run_env.data_connection_timeout = (unsigned int)(atoi(value));
		array[6] = 1;
	} else if (!strncmp(option,"Ftpd_banner",strlen("Ftpd_banner"))) {
		if ((run_env.ftpd_banner = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			perrorConsole("No enough memory!\n");
			goto ret3;
		}
      strcpy(run_env.ftpd_banner,value);
		array[7] = 1;
	} else if (!strncmp(option,"Max_clients",strlen("Max_clients"))) {
		run_env.max_clients = (unsigned int)(atoi(value));
		array[8] = 1;
	} else if (!strncmp(option,"Max_links", strlen("Max_links"))) {
		run_env.max_links = (unsigned int)(atoi(value));
		array[9] = 1;
	} else if (!strncmp(option,"Passive_port", strlen("Passive_port"))) {
		if ((tmp = strtok_r(value,",",&saveptr2)) == NULL) {
			perrorConsole("Error in configure file\n");
			goto ret3;
		}
		run_env.passive_port_min = (unsigned int)(atoi(tmp));		/*get the min_port and store it*/
		if ((tmp = strtok_r(NULL,",",&saveptr2)) == NULL) {
			perrorConsole("Error in configure file\n");
			goto ret3;
		}								/*get the max_port and store it*/
		run_env.passive_port_max = (unsigned int)(atoi(tmp));
		array[10] = 1;
	} else if (!strncmp(option,"FTP_dir",strlen("FTP_dir"))) {
		if ((strlen(value) + 1) > PATH_NAME_LEN) {
			perrorConsole( "Path name is too long!\n");
			goto ret3;
		}
      strcpy(run_env.ftp_dir,value);
		array[11] = 1;
	} else if (!strncmp(option,"User_pass_file",strlen("User_pass_file"))) {
		if ((run_env.user_pass_file = malloc((strlen(value) + 1) * sizeof (char))) == NULL) {
			perrorConsole("No enough memory!\n");
			goto ret;
		}
                strcpy(run_env.user_pass_file,value);
		array[12] = 1;
	} else if (!strncmp(option, "Visible_user_name",strlen("Visible_user_name"))) {
		if ((strlen(value) + 1) > USER_NAME_LEN) {
			perrorConsole("Visible_user_name is too long!\n");
			goto ret;
		}
		strcpy(run_env.visible_user_name, value);
		array[13] = 1;	
	} else if (!strncmp(option, "Visible_group_name",strlen("Visible_group_name"))) {
		if ((strlen(value) + 1) > USER_NAME_LEN) {
			perrorConsole("Visible_group_name is too long!\n");
			goto ret;
		}
		strcpy(run_env.visible_group_name, value);
		array[14] = 1;	
	} else {
		perrorConsole("Parameter can't be analyzed!\n");
		goto ret;
	}
/*codes above are used to value the parameters of the struct run_env*/

	return 0;
ret:
	free(run_env.user_pass_file);	/*free the memory*/
ret3:
	free(run_env.ftpd_banner);	/*free the memory*/
ret2:
	free(run_env.log_file);		/*free the memory*/
ret1:
	free(value);			/*free the memory*/
ret0:
	free(option);			/*free the memory*/
	return -1;
}


int read_configure()
{
	int fd;			
	char buffer[_BUFFER_LENGTH];
	int i;
	int config_array[CONFIG_NUM];
	
	for (i = 0;i < CONFIG_NUM;i++) {
		config_array[i] = 0;
	}
		
	i = 0;
	if ((fd = open(CONFIG_FILE,O_RDONLY)) == -1) {		/*open the configure file error*/
		perrorConsole("Can't open the configure file xylftp.conf!\n");
		return -1;
	} else {
		i = _read_line(fd,buffer);			/*get an valuable line from the configure file*/
		
		while (i != EOF ) {
			
			if (i == 1) {
				#ifdef DEBUG
				printfConsole("%s\n",buffer);
				#endif
				/*analyze the line got from the file*/
				if ((i = _analyze_para(buffer,config_array)) == -1) {
					goto back;
				}				
			
			}
			i = _read_line(fd,buffer);
		}
		#ifdef DEBUG
		print();
		#endif
back:
		close(fd);
	}

	if (_check_configure(config_array) == -1) {
		write_log("Configure file error!",0);
		return -1;
	}
	return 0;
}

static int _read_line(int fd,char *buf)
{
	char ch;
	int i = 0,j;
	
	if ((j = read(fd,&ch,1)) == 0) {		/*end of the file*/
		return EOF;
	}		
	

	
	while (ch != '\n' && j != 0 ){		/*if the character we got is not an end of a line or the end of the file*/
		if (ch == '#' ) {							/*if it is a comment line,skip it over*/
			while (ch != '\n' ) {
				if ((j = read(fd,&ch,1))  == 0) {
					break;	
				}
			}
			j = read(fd,&ch,1);
		} else if (ch == ' ') {					/*if it is a space,skip to the next character*/
			if (i != 0) {
				buf[i++] = ch;
			}
			j = read(fd,&ch,1);
		} else if (j != 0){					/*a valuable character,store it*/
			buf[i++] = ch;
			j = read(fd,&ch,1);
		}
	}

	if ( j == 0 ) {							/*end of the line,also the end of the file,return EOF*/
		buf[i] = '\0';
		return EOF;
	} else if (i != 0) {							/*end of an valuable line*/
		buf[i] = '\0';
		return 1;
	}
	return 0;
}

static int _check_configure(int *array)
{
	int i = 0;
	if(!array[0]) {
		perrorConsole("No Anonymous_enable item or error!\n");
	} 
	if (!array[1]) {
		perrorConsole("No FTP_port item or error!\n");
	}
	if (!array[2]) {
		perrorConsole("No Local_umask item or error!\n");
	}
	if (!array[3]) {
		perrorConsole("No Log_file_enable item or error!\n");
	}
	if (!array[4]) {
		perrorConsole("No Log_file item or error!\n");
	}
	if (!array[5]) {
		perrorConsole("No Idle_session_timeout item or error!\n");
	}
	if (!array[6]) {
		perrorConsole("No Data_connection_timeout item or error!\n");
	}
	if (!array[7]) {
		perrorConsole("No Ftpd_banner item or error!\n");
	}
	if (!array[8]) {
		perrorConsole("No Max_clients item or error!\n");
	}
	if (!array[9]) {
		perrorConsole("No Max_links item or error!\n");
	}
	if (!array[10]) {
		perrorConsole("No Passive_port item or error!\n");
	}
	if (!array[11]) {
		perrorConsole("No FTP_dir item or error!\n");
	}
	if (!array[12]) {
		perrorConsole("No User_pass_file item or error!\n");
	}
	if (!array[13]) {
		perrorConsole( "No Visible_user_name item or error!\n");
	}
	if (!array[14]) {
		perrorConsole( "No visible_group_name item or error!\n");
	}

	for (i = 0;i < CONFIG_NUM;i++) {
		if (array[i] == 0) {
			return -1;
		}
	}

	return 0;
}
