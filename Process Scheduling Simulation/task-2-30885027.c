/* 
Author: Yong Zi Ying
Start date: 4/10/2020
Last modified date: 24/10/2020
Process Shceduling Simulation

Description: 
This task is to implement round robin scheduling algorithms.
It contains main function and Round_Robin function.
The task will be explained more in details in in-line comments and function header comments.

User guideline on how to test with this task-2-30885027.c file: 
Refer to user_documentation.txt

The overall approach for this task are getting some online information 
and ask seniors for help.
*/

#include <stdio.h>
#include <unistd.h>			// for access ()
#include <stdlib.h>			// for exit ()
#include <string.h>			// for string operations
#include <errno.h>			// for system error numbers (errno)
#include <libgen.h>			// for dirname ()

/* Special enumerated data type for process state */
typedef enum {
	READY, RUNNING, EXIT
} process_state_t;

/* C data structure used as process control block. 
The scheduler should create one instance per running process in the system.*/
typedef struct {
	char process_name[11]; // a string that identifies the process
	
	/* Times measured in seconds...*/
	int entryTime; // time process entered system
	int serviceTime; // the total CPU time required by the process
	int remainingTime; // remaining service time until completion.
	int deadline;	// maximum time from the arrival to completion of a process
	int turnaroundTime;
	int waitTime;

	process_state_t state; // current process state(e.g. READY).
} pcb_t;

typedef enum {
	True, False
} boolean;

/* Funtion declaration */
void Round_Robin(pcb_t p[], int num_process);


int main(int argc, char *argv[])
{
	/*
	The purpose of this function is to open either the user input valid filename/filepath
	or the default file which is "process-data.txt". Next, read the data from the file, 
	create some process control block instances and append the process control block into
	an array. After reading and creating, this function will pass process control block
	array and the total number of process into Round_Robin function. 
	(explanation for Round_Robin function will see soon). 
	After running the Round_Robin function, the result that was 
	calculated in Round_Robin function is outputted in "scheduler-result2.txt".  
	Deadline Met is determined in this function. 
	More details for this function will be explained in in-line comments.
	*/

	FILE *file;	
	// ~~~ open file ~~~ //
	// open default file ("process-data.txt")
	file = fopen("process-data.txt", "r");
	
	if (argc > 1)
	{
		char *res = strdup(argv[1]); // duplicate argv[1] in order to prevent error
		// if the argument res is an absolute path, use it instead of process-data.txt
		if (strcmp(dirname(res),".") != 0)
		{	
			if(access(argv[1], F_OK) == 0) // check the file exists or not
			{	 
				// exists
				file = fopen(argv[1], "r");
			}
			else
			{
				// not exists [error]
				write(2, strerror(errno), strlen(strerror(errno)));
				exit(1);
			}
		}
		// if it's filename instead of absolute path
		else if(access(argv[1], F_OK) == 0)
		{
			file = fopen(argv[1], "r");
		}
		// if the argument res is not absolute path, show error
		else
		{
			// not exists [error]
			write(2, strerror(errno), strlen(strerror(errno)));
			exit(1);
		}
	}
	
	char processName[10];
	int data;	// data from each line in text file
	int arrivalTime;
	int serviceTime;
	int deadline;
	int num_process = 0;	// counter for total number of processes
	pcb_t proc[10];
	
	/* read each line from file */
	data = fscanf(file,"%s %d %d %d", processName, &arrivalTime, &serviceTime, &deadline);
	// exit the loop if End Of File(EOF) is reached 
	while (data != EOF)
	{
		pcb_t proc_data;
		proc_data.entryTime = arrivalTime;
		proc_data.serviceTime = serviceTime;
		proc_data.remainingTime = serviceTime;	//remaining service time
		proc_data.deadline = deadline;
		strcpy(proc_data.process_name,processName);
		 
		// append process control block into new array
		proc[num_process] = proc_data;	/* proc_data= [P1_data, P2_data, ...] */
		num_process ++; 
		data = fscanf(file,"%s %d %d %d", processName, &arrivalTime, &serviceTime, &deadline);
	}
	fclose(file);
	
	// passing the process control block array and total number of processes into Round_Robin
	Round_Robin(proc, num_process);
	
	/* after returning back from Round_Robin function */
	// output result into "scheduler-result2.txt"
	FILE *outputfile = fopen("scheduler-result2.txt", "w");
	if(outputfile == NULL)
	{
		printf("Invalid file");
		return 1;
	}
	// loop the process to write it into outputfile
	for (int i = 0; i < num_process; i++)
	{
		pcb_t processes = proc[i];
		// check the deadlineMet
		int deadlineMet;
		if (processes.turnaroundTime <= processes.deadline)
		{
			deadlineMet = 1;
		}
		else
		{
			deadlineMet = 0;
		}
		fprintf(outputfile, "%s %d %d %d\n", processes.process_name, processes.waitTime, processes.turnaroundTime, deadlineMet);
	}
	fclose(outputfile);
	
	return(0);
}


void Round_Robin(pcb_t proc[], int num_process)
{
	/*
	The purpose of this functions is to calculate the result of round robin scheduling
	algorithm. The time-slice quantum is 2 seconds. A new process comes and an old process
	becomes ready from running state, the new process will prioritize.
	(same concept as lecture). Process remainingTime is used to keep track of the remaining 
	running time of process, remainingTime will be decremented by 1 after every loop.
	used_time variable is used to keep track of the time-slice quantum count.
	After running process finished execution, we need to calculate turnaroundTime and
	waitTime. Deadline Met will be determined in the main function.
	More details for this function will be explained in in-line comments.  
	*/

	int duration = 0;	// capture the timer
	boolean bool1;
	bool1 = False;	
	int process_done = 0;	// to count the number of process finished execution
	pcb_t *running;		// to update the pcb_t data after calculating 
	pcb_t *tem_proc[10];	// temporary array to store process data
	int head = 0;	// acts like pointer
	int quantum = 2;
	int quantum_counter = 0;
	int used_time = 0;		// acts like quantum counter
	int tem_proc_counter=0;		// temporary array counter
	
	// exit when all processes are done 
	while(process_done < num_process)
	{
		int i = 0;
		while (i < num_process)
		{
			// retrieve each data of processes from proc_data by using pointer
			pcb_t* processes = &proc[i];
			
			// each processes will be in Ready state depend on their enterTime
			if(processes->entryTime == duration)
			{
				// output messages (file pointer is used to output on screen)
				fprintf(stdout, "Time %d: %s has entered the system.\n", duration, processes->process_name);
				tem_proc[head] = processes;	// append into an array(ready)
				head++;
				tem_proc_counter ++;
				processes->state = READY;
			} 
			// increment loop
			i++;
		}
		
		// process is running and its remaining time is 0, then exit the process
		if(bool1 == True && running->remainingTime == 0)
		{	
			// turnaround = completion - entryTime
			running->turnaroundTime = duration - running->entryTime;
			// WaitTime = turnaround - serviceTime
			running->waitTime = running->turnaroundTime - running->serviceTime;
			running->state = EXIT;
			fprintf(stdout, "Time %d: %s has finished execution.\n", duration, running->process_name);
			bool1 = False;
			process_done++;
			tem_proc_counter--;
			quantum_counter ++;
			used_time = 0;
			
		}
		/*
		append the uncompleted running process back into array when time slice 2 
		quantum has reached and if it's in tem_proc array 
		the uncompleted running process will change to ready state
		(to avoid the processes that's not appended into tem_proc to enter this code)
		*/
		else if(bool1 == True && used_time >= quantum && tem_proc_counter > 0)
		{
			tem_proc_counter++;
			running->state = READY;
			used_time = 0;
			tem_proc[head] = running;	// append into an array(ready)
			head++;
			quantum_counter ++;
			bool1 = False;
		}
		
		// no process is running, the next process will start running 
		if(bool1 == False && (running = tem_proc[quantum_counter]))
		{
			tem_proc_counter--;
			running->state = RUNNING;
			fprintf(stdout, "Time %d: %s is in the running state.\n", duration, running->process_name);
			bool1 = True;
		}
		
		if(running)
		{
			used_time++;	// increament the usedTime 
			running->remainingTime--;	// decrement the remainingTime
		}
		
		duration++;
		sleep(1);
	}
}









