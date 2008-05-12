#include "confmanager.h"

int conf_read(const char *filename) {
	
	char *tmp = calloc(512, 1);
	char buffer;
	int fd;
	int i=0;
	int nread;
	
	printf("opening %s\n", filename);
	if((fd=open(filename, O_RDONLY|O_EXCL))<0) {
		logger(ALARM_INFO, "Unable to open configuration file\n");
		return -1;
	}
	
	while((nread=read(fd,&buffer,1))>0){
		tmp[i]=buffer;
		
		if(tmp[i]=='\n'){
			if(strchr(tmp, '#')!=NULL) {
				printf("Comment\n");
			}
			else if(strlen(tmp)<=2) {
				printf("Empty line\n");
			}
			else {
				conf_save_value(tmp);
			}
			
			
			memset(tmp,0,strlen(tmp));
			i=0;
		}
		else
			i++;
	}
}

int conf_save_value(const char *line) {
	if(line==NULL) {
		logger(ALARM_INFO, "Unable to read line\n");
		return -1;
	}
	
	char *line_dup = calloc(strlen(line), 1);
	int i, j;
	for(i=0, j=0; i<strlen(line); i++) {
		if(line[i] != ' ' && line[i] != '\n' && line[i] != '\t') {
			line_dup[j++] = line[i];
		}
	}
	line_dup[j]='\0';
	
	char *equal = strchr(line_dup, '=');
	int left_len = equal-line_dup;
	char *left = calloc(left_len+1, 1);
	strncpy(left, line_dup, left_len);
	printf("left: %s\n", left);
	
	int right_len = (&line_dup[strlen(line_dup)])-equal-2;
	char *right = calloc(right_len+1, 1);
	equal++;
	strncpy(right, equal, right_len);
	printf("right: %s\n", right);
	
	if(strcmp(left, "qlen")==0)
		qlen = atoi(right);
	else if(strcmp(left, "buffer_len")==0)
		buffer_len = atoi(right);
	else if(strcmp(left, "path")==0)
		path = right;
	else if(strcmp(left, "gen_start")==0)
		gen_start = atoll(right);
	else if(strcmp(left, "max_len")==0)
		max_len = atoi(right);
	else if(strcmp(left, "max_thread")==0)
			max_thread = atoi(right);
	else if(strcmp(left, "max_fd")==0)
			max_fd = atoi(right);
	else if(strcmp(left, "datadir")==0)
			datadir = right;
	
	return 1;
	
}
