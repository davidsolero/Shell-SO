/**
 * Nombre: David Solero Chicano
 * 
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include "string.h"
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------
job * lista_trabajos;

void manejador() {
	block_SIGCHLD();
	for(int i = list_size(lista_trabajos); i >= 1 ; i--) {
		job *trabajo = get_item_bypos(lista_trabajos, i);
		if(trabajo != NULL) {
			int estado, info;
			int pid = trabajo -> pgid;
			enum status status_res;
			if(pid == waitpid(pid, &estado, WUNTRACED | WNOHANG)) {
				status_res = analyze_status(estado, &info);
				if(status_res == EXITED || status_res == SIGNALED ) {
					printf("Background process %s (%d) %s\n", trabajo ->command, trabajo -> pgid, status_strings[status_res]);
					printf("COMMAND->");
					fflush(stdout);
					delete_job(lista_trabajos, trabajo);
				} else if (status_res == SUSPENDED) {
					trabajo -> state = STOPPED;
				} else if (status_res == CONTINUED) {
					trabajo -> state == BACKGROUND;
				}
			}
		}
	}
	unblock_SIGCHLD();
}

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	ignore_terminal_signals();
	signal(SIGCHLD, manejador);
	
	lista_trabajos = new_list("Tareas");
	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("COMMAND->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue;   // if empty command

		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue 
			 (4) Shell shows a status message for processed command 
			 (5) loop returns to get_commnad() function
		*/
		if(strcmp(args[0], "cd") == 0) {
			chdir(args[1]);
		} else {
			pid_fork = fork();
			if(pid_fork == 0) { //Proceso hijo
				new_process_group(getpid());
				if(background == 0) {
					set_terminal(getpid());
				}
				restore_terminal_signals();
				execvp(args[0], args);
				printf("Error, command not found: %s\n", args[0]);
				exit(-1);
			} else {
				
				if(background == 0) {
					
					waitpid(pid_fork, &status, WUNTRACED);
					set_terminal(getpid());
					
					status_res = analyze_status(status, &info);
					if(status_res == SUSPENDED) {
						add_job(lista_trabajos, new_job(pid_fork, args[0], STOPPED));
					}
					printf("Foreground pid: %d, command: %s, %s, info: %d\n", pid_fork, args[0], status_strings[status_res] , info);
					
				} else {
					printf("Background job running... pid: %d, command: %s\n", pid_fork, args[0]);
					job * trabajo = new_job(pid_fork, args[0], BACKGROUND);
					add_job(lista_trabajos, trabajo);
				}

			}
		}
	} // end while
}

