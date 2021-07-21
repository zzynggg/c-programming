/* 
FIT 2100 Assignment 2
Author: Yong Zi Ying
Student ID: 30885027
Start date: 4/10/2020
Last modified date: 24/10/2020

Description: 
This task is to implement earliest deadline first scheduling algorithms.
It contains main function and Earliest_Deadline_First function.
The task will be explained more in details in in-line comments and function header comments.

User guideline on how to test with this task-3-30885027.c file: 
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
#include <limits.h>			// for INT_MAX

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

/* Funtion declaration */
void Earliest_Deadline_First(pcb_t p[], int num_process);


int main(int argc, char *argv[])
{
	/*
	The purpose of this function is to open either the user input valid filename/filepath
	or the default file which is "process-data.txt". Next, read the data from the file, 
	create some process control block instances and append the process control block into
	an array. After reading and creating, this function will pass process control block
	array and the total number of process into Earliest_Deadline_First function. 
	(explanation for Earliest_Deadline_First function will see soon). 
	After running the Earliest_Deadline_First function, the result that was 
	calculated in Earliest_Deadline_First function is outputted in "scheduler-result3.txt".  
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
		// if the argument res is an absolute path, use it instead of "process-data.txt"
		if (strcmp(dirname(res),".") != 0)
		{	
			if(access(argv[1], F_OK) == 0)  // check the file exists or not
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
		// if it's valid filename instead of absolute path
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
		proc_data.remainingTime = serviceTime;		//remaining service time
		proc_data.deadline = deadline;
		strcpy(proc_data.process_name,processName);
		 
		// append process control block into new array
		proc[num_process] = proc_data;	/* proc_data = [P1_data, P2_data, ...] */
		num_process ++; 
		data = fscanf(file,"%s %d %d %d", processName, &arrivalTime, &serviceTime, &deadline);
	}
	fclose(file);
	
	// passing the process control block array and total number of processes into Earliest_Deadline_First
	Earliest_Deadline_First(proc, num_process);
	
	/* after returning back from Earliest_Deadline_First function */
	// output result into "scheduler-result3.txt"
	FILE *outputfile = fopen("scheduler-result3.txt", "w");
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


void Earliest_Deadline_First(pcb_t proc[], int num_process)
{
	/* 
	The purpose of this functions is to calculate the result of earliest deadline
	first scheduling algorithm. The overall approach is if new process deadline 
	is smaller than the old process. The new process will prioritize and become running
	state. For the old process it will change from running state to ready state. 
	If the running process finished execution AND no new process entered then the 
	old process becomes running from ready state. To make it clear, smaller process 
	deadline will execute until the next smaller process deadline is found.
	Then the next smaller process will run.
	Process remainingTime is used to keep track of the remaining 
	running time of process, remainingTime will be decremented by 1 after every loop.
	After running process finished execution, we need to calculate turnaroundTime and
	waitTime. Deadline Met will be determined in the main function.
	More details for this function will be explained in in-line comments.  
	*/
	
	int duration = 0;	// capture the timer
	int process_done = 0;	// to count the number of process finished execution
	pcb_t *running;		// to update the pcb_t data after calculating 
	pcb_t *tem_proc[10];	// temporary array to store process data
	// assume min deadline is the MAX integer num in c (2147483647)
	int min_deadline = INT_MAX;
	int tem_proc_counter = 0;	// temporary array counter
	int head = 0;	// acts like pointer
	int process_enter = 0;	// to count the process entered the system
	
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
				process_enter ++;
				// output messages (file pointer is used to output on screen)
				fprintf(stdout, "Time %d: %s has entered the system.\n", duration, processes->process_name);
				processes->state = READY;
				
				/* 
				if new process deadline is smaller, current running process become ready
				state and append it into an array
				new process becomes running state from ready state
				*/
				if (process_enter > 1 && processes->deadline < min_deadline)
				{
					tem_proc[head] = running;	// append into an array(ready)
					head++;
					tem_proc_counter ++;
					running = processes;	// update the running process
					min_deadline = processes->deadline;	// update the min_deadline
					running->state = RUNNING;
					fprintf(stdout, "Time %d: %s is in the running state.\n", duration, running->process_name);
				}
				/* 
				if new process deadline is larger, append the it into tem_proc
				new process will be implemented if current running process finished
				termination or NO new process entered.
				*/
				else if (process_enter > 1 && processes->deadline > min_deadline)
				{
					tem_proc[head] = processes;
					head++;
					tem_proc_counter ++;
					processes->state = READY;
				}
				/* 
				first process deadline is assigned to min_deadline
				it becomes running state from ready state
				*/
				else if (processes->deadline < min_deadline)
				{		
					running = processes;
					min_deadline = processes->deadline;
					running->state = RUNNING;
					fprintf(stdout, "Time %d: %s is in the running state.\n", duration, running->process_name);
				}
			} 
			// increment loop
			i++;
		}
		
		
		// running process remaining time is 0, then exit the process
		if(running->remainingTime == 0)
		{
			// turnaround = completion - entryTime
			running->turnaroundTime = duration - running->entryTime;
			// WaitTime = turnaround - serviceTime
			running->waitTime = running->turnaroundTime - running->serviceTime;
			running->state = EXIT;
			fprintf(stdout, "Time %d: %s has finished execution.\n", duration, running->process_name);
			process_done++;
			min_deadline = INT_MAX;		// update the min_deadline
			
			// check the tem_proc process array 
			for (int i = 0; i < tem_proc_counter; i++)
			{
				/*
				the process that was appended in tem_proc previously, if its deadline
				is smaller than min_deadline. This process will prioritize and become
				running from ready state.
				and check if the process remaining time is not equal to zero just run it
				in order to prevent terminated process being in running state again
				*/
				if(tem_proc[i]->remainingTime != 0 && tem_proc[i]->deadline < min_deadline)
				{
					running = tem_proc[i];
					min_deadline = tem_proc[i]->deadline;
					running->state = RUNNING;
				}
			}
			/* 
			check if the running process remaining time is not equal to zero
			then just print the output in order to prevent terminated process 
			being outputted again
			*/
			if (running->remainingTime != 0){
				fprintf(stdout, "Time %d: %s is in the running state.\n", duration, running->process_name);
			}
			
		}
		
		duration++;	// increment the timer
		
		// decrement the remainingTime
		if(running)
		{
			running->remainingTime--;
		}

		sleep(1);
	}
}









