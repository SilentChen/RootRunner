#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define IN
#define OUT
#define SH_LOGFILE_NAME "run.log"
#define FILENAME_MAXSIZE 20
#define SHARG_MAXSIZE 2048
#define SHCMD_MAXSIZE 100

#ifndef CONFIG_FILE
#define CONFIG_FILE "config"
#endif

static int	shFileValidate(IN const char* filename);
static void printHelp();

/**
 * @param argc: the len of argv
 * @param argv: argv[0] is filename, others is self defined argument
 * @desc  in this program, argv[1] is the sh file, the argv[2] is the sh executing log, others is sh argument;
 * @return int success 0, 1 sh file not exist.
 * */
int main(int argc, char *argv[])
{
	char* filename;
	// argv[1] is nessary.
	if(!argv[1]) {
		printHelp();
		return 1;
	}else{
		filename = argv[1];
	}
	
	char* logfile;

	if(argv[2]) {
		logfile = argv[2];
	}else{
		logfile = SH_LOGFILE_NAME;
	}
	
	// set execute user for root
	uid_t uid ,euid;
	uid = getuid() ;
	euid = geteuid();
	setreuid(euid, uid);

	// cmd buffer, only sh file support
	char cmd[SHCMD_MAXSIZE];
	char shArgs[SHARG_MAXSIZE];
	if(argc > 2) {
		int index;
		for(index = 3; index < argc; ++ index){
			sprintf(shArgs, "%s \"%s\"", shArgs, argv[index]);
		}
	}
	sprintf(cmd,"%s %s > %s 2>&1",filename,shArgs,logfile);
	//printf("cmd is: %s and the argc is %d and the shArgs is: %s \n", cmd, argc, shArgs);
	int validateRet;
	if(0 == (validateRet = shFileValidate(filename))) {
		system(cmd);

		// make log
		FILE *fp;
		char ch[64];
		if((fp=fopen(argv[2],"r"))!= NULL)
		{
			while(!feof(fp))
			{
				if(fgets(ch,64,fp) != NULL) 
					printf("%s",ch);
			}
		}
	}else{
		printf("Bad File, Error No %d. \n", validateRet);
	}

	return 0;
}

/**
 * @param char* filename
 * @return int: 0 means ok
 *				1 means is not exist
 *				2 means file cannot be executed
 *				3 means is not a sh file
 *				4 means not a config sh file
 *				5 means no config file
 **/
static int shFileValidate(IN const char* filename) {
	if(-1 == access(filename, F_OK)){
		return 1;
	}	

	if(-1 == access(filename, X_OK)) {
		return 2;
	}

	char original_filename[100];
	strcpy(original_filename, filename);
	char* postfix;
	char* buffer;
	postfix = strtok((char*)filename, ".");
	while((buffer = strtok(NULL, "."))) {
		if(buffer) postfix = buffer;
	}
	if(0 == strcmp("sh", postfix)) {
		FILE* fd;
		char* cf = CONFIG_FILE;
		if(NULL == (fd = fopen(cf, "r"))) {
			return 5;
		}else{
			char row[100];
			int flag = 0;
			while(!feof(fd)) {
				fscanf(fd,"%s",row);
				if(0 == strcmp(original_filename, row)) {
						flag = 1;
						break;
				}
			}	
			if(1 == flag) 
				return 0;
			else
				return 4;
		}
	}else{
		return 3;
	}


	return 0;
}

static void printHelp() {
	char* head		= "############################### Instruction: ##################################";
	char* interval	= "|";
	char* useage	= "|- HowToUse:	touch a config file and run xxx.sh xxx.log \"argument1\" \"argument2\" ... \"argumentn\"";
	char* errdesc	= "|- ErrCodeDesc:	1 sh file not exist; 2 file cannot be executed; 3 not a sh file; 4 not a config sh file; 5 no config file.";
	char* warna1	= "|- Argument1:	this arg must be a existing sh script file";
	char* warna2	= "|- Argument2:	this arg is a option for log file";
	char* warna3	= "|- Argumentn:	except arg1 and arg2, the others are option for sh argument";
	char* warnlog	= "|- DefaultlLog:	default log file is current dir's run.log";
	char* warnarg	= "|- ShWarning:	the offering sh script should validate all the arguments before use it";
	char* warnauth= "|- AuthWarning: make sure php process cannot change config file and sh file which was written in config.";
	printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", head, useage, interval, warna1, interval, warna2, interval, warna3, interval, errdesc, interval, warnlog, interval, warnarg, interval, warnauth);
}
