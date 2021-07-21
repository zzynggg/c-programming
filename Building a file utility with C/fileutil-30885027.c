/* 
FIT 2100 Assignment 1
Author: Yong Zi Ying
Student ID: 30885027
Start date: 22/8/2020
Last modified date: 10/9/2020

Description: 
For task 1, the contents of logfile.txt need to be printed out to the terminal.
For task 2, the contents of argv[1] will be printed out to the terminal.
For task 3, the source file (a.txt) need to be copied to the destination directory.
For task 4, the source file (a.txt) need to be moved to the destination directory.
For task 5, it's a combination of task 3 and task 4 which are FORCE the source file
to be moved or copied to the destination directory.
Every tasks will be explained more in details in in-line comments.

User guideline on how to test with this fileutil-30885027.c file: 
Refer to user_manual.txt

The overall approach for this assignment are getting some online information 
and ask seniors for help.
I apologies that I code every tasks in the same file due to my poor programming skill.  
 */

#include <fcntl.h>			// for file creation flags
#include <unistd.h>			// for access ()
#include <stdlib.h>			// for exit ()
#include <string.h>			// for string operations
#include <errno.h>			// for system error numbers (errno)
#include <libgen.h>			// for dirname ()


int main(int argc, char *argv[]){
	
	int infile, infile2;	// act as file descriptor 
	int optt;				// for getopt
	// act as separator
	int jump_read = 0;		// Task 1
	int jump_read2 = 0;		// Task 2
	int jump_d = 0;			// Task 3
	int copy = 0;			// Task 3
	int jump_M = 0;			// Task 4
	int force_copy = 0;		// Task 5
	int jump_F = 0;			// Task 5
	
	char last_c;		// check last character of path
	
/*===================================================================*/
	/* 
	*** Task 1 *** 
	Before open the "logfile.txt", access() is used to check whether the file exist
	or not exist. If the file exist, access() will return 0. Open the "logfile.txt"
	and set jump_read seperator to 1 in order to output the contents of "logfile.txt"
    in terminal. If the the file is not exist, a standard error	will return.
    strerror(errno) is used to returns a pointer to a string that describes the 
    error code passed in the argument errno. File descriptor 2 is used output 
    standard error stream.
	*/
	if (argc == 1){
		if(access("logfile.txt", F_OK) == 0){	// check the file exists or not
			// exists
			infile = open("logfile.txt", O_RDONLY);		
			jump_read = 1;
		}
		else{
			// not exists [error]
			write(2, strerror(errno), strlen(strerror(errno)));
			exit(1);
		}
	}
	
/*===================================================================*/
	/* 
	*** Task 2 *** 
	The argv[1] which is the file path of other file will be duplicated in order 
	to prevent error. dirname() is used to check whether the argv[1] is an
	absolute path or not. If it's an absolute path, dirname() will return a pointer
	to a string that is the parent directory of path. Before open the file, access()
	is used to check the file exist or not exist same like the explanation for 
	task 1. The jump_read2 seperator is set to 1 in order to output the contents
	of the file. If the specified source file is not exist then the contents of 
	default file which is "logfile.txt" will output in terminal. 
	*/
	
	char *res;
	// duplicate argv[1] in order to prevent error
	if (argc > 1){
		res = strdup(argv[1]);
	}
	// check if there's argv[1], prevent out of index error
	if (argc == 2){		
		// if the argument res is an absolute path
		if (strcmp(dirname(res),".") != 0){	
			if(access(argv[1], F_OK) == 0){	 // check the file exists or not
				// exists
				infile = open(argv[1], O_RDONLY);
				jump_read2 = 1;
			}
			else{
				// not exists [error]
				write(2, strerror(errno), strlen(strerror(errno)));
				exit(1);
			}
		}
		// if the argument res is not absolute path, open default file
		// path = NULL pointer, dirname() returns "."
		else{
			// open default file ("logfile.txt")
			infile = open("logfile.txt", O_RDONLY);
			jump_read = 1;
		}
	
	}
	
/*===================================================================*/
	/* 
	Task 3, Task 4, Task 5 are coded within the getopt.
	getopt() is used to parse command line arguments.
	getopt() will return '-1' is there are no more otions to process,
	':' will be returned when encounters an option with a missing argument,
	the return value depends on the first character in optstring which is ':',
	'?' will be returned when there is an unrecognized option. 
	*/
	
	char dir[1024];		// temp for optarg
	// parse command line arguments
	while((optt = getopt(argc, argv, ":d:MF")) != -1){
		switch(optt){
/*===================================================================*/		
			/* 
			*** Task 3 *** 
			If -d option is used. The option target need to be duplicated by using 
			strdup() then dirname() is used to check whether option target is an
			absolute path or not. Same like the explaination in the previous task for dirname().
			If it is an absolute path, copy the option target into dir. Check the last 
			character of file path then append the dir with either a.txt or /a.txt. 
			It's depend on how user input the file path in order to prevent error occurs.
			After appending the file path, set jump_d seperator into 1 means that -d is found.
			If -d is found, open the source file (a.txt) then copy the source file into
			destination path. Set the copy seperator into 1, then copy the source file 
			into destination file.
			*/
			case 'd':{		
				char *opt = strdup(optarg);		// duplicate the optarg
				// for 7th example commands
				if (argc < 4){
					// logfile.txt
					if (strcmp(dirname(opt),".") != 0){	// if argument optarg is a file path
						strcpy(dir, optarg);
						// check last character
						int len = strlen(dir);
						last_c = dir[len-1];
						//append optarg with logfile.txt
						if (last_c == '/')
							strcat(dir, "logfile.txt");		//True
						else
							strcat(dir, "/logfile.txt");	//False
					}
					// if argument optarg is NOT a file path
					else{
						// error
						write(2, strerror(errno), strlen(strerror(errno)));
						exit(2);
					}
				}
				else{
					// a.txt
					if (strcmp(dirname(opt),".") != 0){	// if argument optarg is a file path
						strcpy(dir, optarg);	
						// check last character
						int len = strlen(dir);
						last_c = dir[len-1];
						//append optarg with a.txt
						if (last_c == '/')
							strcat(dir, "a.txt");	//True
						else
							strcat(dir, "/a.txt");	//False
						
					}
					// if argument optarg is NOT a file path
					else{
						// error
						write(2, "Invalid argument: immediately after -d, a directory path was expected\n", strlen("Invalid argument: immediately after -d, a directory path was expected\n"));
						exit(2);
					}
				}
				
				free(opt);
				jump_d = 1;		// -d found
				
				if(jump_d && argc <= 4){	
					if(argc == 4){
						// open and read infile
						infile = open(res, O_RDWR);
					
						// 0666 is default permission 
						// copy infile into destination directory
						// O_EXCL is used to prevent the existing file to be created again
						if ((infile2 = open(dir, O_CREAT | O_EXCL | O_WRONLY | O_APPEND, 0666)) < 0) {
							// error
							write(2, strerror(errno), strlen(strerror(errno)));
							exit(2);
						}
					}
				
					// for 7th example commands
					else if (argc == 3){
						// open logfile.txt
						infile = open("logfile.txt", O_RDWR);		
						// 0666 is default permission 
						// copy infile into destination directory
						if ((infile2 = open(dir, O_CREAT | O_EXCL | O_WRONLY | O_APPEND, 0666)) < 0) {
							// error
							write(2, strerror(errno), strlen(strerror(errno)));
							exit(2);
						}
					}
					copy = 1;
				}			
			}
				break;
			
/*===================================================================*/			
			/* 
			** Task 4 **	
			If -M option is used, set jump_M separator into 1. Task 4 will be
			further explained in another part of codes.
			*/
			
			case 'M':{		
				// if -M found
				jump_M = 1;		// -M found
			}
				break;

/*===================================================================*/				
			/* 
			*** Task 5 ***	
			Task 5 is the combination of Task 3 and Task 4.
			If -F option is used, set jump_F seperator into 1. 
			If user input -M and -F means FORCE move, else user input -d and -F
			means FORCE copy. So, for FORCE copy O_TRUNC is used to allow 
			existing file to be replaced by source file instead of using O_EXCL to 
			show error when there is an existing file in destination directory. 
			That is the only different between normal copy(Task 3) 
			and force copy(Task 5). Refer to Task 3 documentation
			to get more explaination about how the copy file works. 
			If the destination directory exist.
			Set the force_copy seperator into 1, then copy the source file into 
			destination directory. If the destination directory has a file with
			same name as the source file, the file should replaced by the source file.
			There's another documentation for FORCE move in another part of code.	
			*/
			
			case 'F':{	
				// if -F found		
				jump_F = 1;		// -F found
				
				// FORCE COPY
				if (jump_d && jump_F && !jump_M){
					// open and read infile
					infile = open(res, O_RDWR);				
					// 0666 is default permission 
					// copy infile into destination directory
					// O_TRUNC is used to allow existing file to be replaced to new file
					if ((infile2 = open(dir, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0666)) < 0) {
						// error
						write(2, strerror(errno), strlen(strerror(errno)));
						exit(2);
					}
					force_copy = 1;
				}
			}	
				break;
	
/*===================================================================*/			
			/* encounters an option with a missing argument, the return value 
			depends on the first character in optstring */	
			case ':':{
				write(2, strerror(errno), strlen(strerror(errno)));
				exit(3);
			}
				break;
		
			// unrecognized option	
			case '?':{	
				write(2, "Unknown option %c\n", optopt);
				exit(3);
			}
				break;
		}	
	}
	
/*===================================================================*/	
	/*
	This part of codes is used to get the number of bytes of the file by using 
	SEEK_END. After getting the number of bytes of the file, SEEK_SET is used
	to read the file in the correct order which is from the beginning of the file.
	*/
	// to get the number of bytes by using SEEK_END
	int num_byte = lseek(infile, 0, SEEK_END); 
	char buffer[num_byte];
	// reset lseek by using SEEK_SET
	lseek(infile, 0, SEEK_SET);
	int n = read(infile, buffer, num_byte);
	
/*===================================================================*/	
	/* 
	~~ separator for each tasks ~~
	Prevent entire code messed up.
	Prevent unnecessary output been printed out.
	*/
	if ((jump_read || jump_read2) && argc <= 4){	// True
		
		/* 
		~ Separator for Task 1 ~
		If jump_read separator is set to 1, output the contents of "logfile.txt"
    	in terminal. 
		*/
		if (jump_read){ 
			write(1, buffer, n);
		}
		
		/* 
		~ Separator for Task 2 ~
		If jump_read2 separator is set to 1, output the contents of other file
    	(a.txt) in terminal. 
		*/
		if(jump_read2){
			write(infile, buffer, n);
			write(1, buffer, n);
		}
	}

	/* 
	~ Separator for Task 3 ~
	If copy separator is set to 1, the source file will be copied into the 
	destination directory and output Copy successful in terminal.
	*/
	// NORMAL COPY //
	if (copy){
		write(infile2, buffer, n);
		write(1, "Copy successful\n", strlen("Copy successful\n"));
	}


	/* 
	~ Separator for Task 5 ~
	If force_copy separator is set to 1, the source file will be force copied 
	into the destination directory and output Force copy successful in terminal.
	*/
	// FORCE COPY //
	if (force_copy){
		write(infile2, buffer, n);
		write(1, "Force copy successful\n", strlen("Force copy successful\n"));
	}

/*===================================================================*/
	/* 
	*** Task 4 ***
	If -M option is used. As usual, check the file path whether exist or not exist.
	If the file path exist link() is used to link the old path (source path) and 
	new path (destination path) in order to create new link into destination path.
	After linking them, unlink the old path (source path). The source file in
	source path will be moved into destination path. Output Move successful after
	unlinking them.
	*/
	// NORMAL MOVE //
	// old path (source path) = res //
	// new path (destination path) = dir //
	if(jump_M && argc <= 5){		
		
		// for 8th & 9th example commands
		if(argc <=4){
			write(2, "Invalid argument, no destination to move the file!\n", strlen("Invalid argument, no destination to move the file!\n"));
			exit(2);
		}
		else{
			if(access(dir, F_OK) == 0){	 // check the destination path exist or not
				// exist [error]
				write(2, strerror(errno), strlen(strerror(errno)));
				exit(2);
			}
			else{
				link(res, dir);   // creates new link into destination path
				unlink(res);		// unlink to source path
				write(1, "Move successful\n", strlen("Move successful\n"));
			}
			
		}
	}
/*===================================================================*/	
	/* 
	*** Task 5 ***
	If -F option is used and user input -M and -F means FORCE move.
	As usual, check the file path whether exist or not exist. 
	If the file path exist.
	in order to FORCE move, new path(destination path) needs to be unlinked first
	then just link the old path (source path) and 
	new path (destination path) to create new link into destination path.
	After linking them, unlink the old path (source path). The source file in
	source path will be FORCE moved into destination path. 
	Output Force move successful after unlinking them.
	*/
	// FORCE MOVE //
	else if (jump_M && jump_F && jump_d){
		if(access(dir, F_OK) == 0){	 // check the destination path exist or not
			unlink(dir);	// unlink destination path
		}	
		link(res, dir);   // creates new link into destination path
		unlink(res);	// unlink source path
		write(1, "Force move successful\n", strlen("Force move successful\n"));
	}
	
/*===================================================================*/
	// to check for error	
	if (jump_F && argc <=3){
		write(2, "Invalid argument, -F is redundant as nothing to force here!\n", strlen("Invalid argument, -F is redundant as nothing to force here!\n"));
		exit(2);			
	}
	
	// close files
	close(infile);
	close(infile2);
	exit(0);
};
