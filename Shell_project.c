/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)
Trabajo realizado por David Solero Chicano
**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include <string.h>
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

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

	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("COMMAND->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue; 
		
		if(strcmp (args[0], "cd") == 0) {	//es interno
			chdir(args[1]);					
			printf("Ejecutando el comando interno cd\n");
		}else{

			pid_fork=fork();

        if (pid_fork < 0) {
			perror("fork erroneo");
			exit(-1);
		}
			
			if (pid_fork==0){
				int gpid = new_process_group(getpid());		//le asignamos al proceso hijo un ide de grupo distinto al del padre
				if(background == 0){
					set_terminal(getpid());					//cuando un proc hijo se ejecuta en 1º plano, hacemos un set_terminal
				}
				restore_terminal_signals();					//es necesario de hacer antes de ejecutar un comando

				execvp(args[0], args);
				printf("Error, no se encuntra el comando o no es ejecutable\n");
				exit(-1);

			}else{
			
			if (background==0){
				waitpid(pid_fork, &status, WUNTRACED);
				set_terminal(getpid());	    //lo devuelve para no bloquearse
				status_res=analyze_status(status, &info);

				printf("Foreground Pid: %d, command: %s, %s, info: %d\n", pid_fork, args[0], status_strings[status_res], info);
			}else{
				printf("Background job running... pid: %d, command: %s\n", pid_fork, args[0]);
			}
			}
		}
		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue 
			 (4) Shell shows a status message for processed command 
			 (5) loop returns to get_commnad() function
		*/

	} // end while
}