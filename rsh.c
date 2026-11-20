/* Alexandra Sandoval Bobadilla
12 commands:
cp, touch, mkdir, ls, pwd, cat, grep, chmod, diff
-> run as seperate processes using posix_spawnp()
cd, exit, help
-> handled directly by shell

GOALS:
display rsh> prompt
check whether or not the command is allowed
print NOT ALLOWED! for commands that are not allowed -> go to stdout
support command arguments such as mkdir testFolder
print an error when cd receives too many arguments
accept commands until the user enters exit


pseudocode
user enters mkdir testFolder
read the command using fgets()
split using strtok()
-> argv[0] = "mkdir"
-> argv[1] = "testFolder"
check isAllowed("mkdir")
-> if allowed: run posix_spawnp()
-> if not allowed: print NOT ALLOWED!
return

*/

#include <stdio.h>
#include <stdlib.h>
#include <spawn.h> // the file is called myspawn
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12
#define MAX_ARGUMENTS 21

extern char **environ;

char *allowed[N] = {"cp", "touch", "mkdir", "ls", "pwd", "cat", "grep", "chmod", "diff", "cd", "exit", "help"};

int isAllowed(const char *cmd)
{
	// TODO
	// return 1 if cmd is one of the allowed commands
	for (int i = 0; i < N; i++)
	{
		if (strcmp(cmd, allowed[i]) == 0)
		{
			return 1;
		}
	}

	return 0; // return 0 otherwise
}

int main()
{

	// TODO
	// Add variables as needed
	char line[256];
	char *argv[MAX_ARGUMENTS + 1]; // xtra slot for null
	pid_t pid;
	int status;

	while (1)
	{

		fprintf(stderr, "rsh>");

		if (fgets(line, 256, stdin) == NULL)
			break; // replaced continue with break because we weant the shell to stop running when there is no more input
		if (strcmp(line, "\n") == 0)
			continue;

		line[strlen(line) - 1] = '\0';

		// TODO
		// Add code to spawn processes for the first 9 commands
		// And add code to execute cd, exit, help commands
		// Use the example provided in myspawn.c

		// split the line into the command and args.
		int argc = 0;
		char *token = strtok(line, " \t");

		while (token != NULL && argc < MAX_ARGUMENTS)
		{
			argv[argc] = token;
			argc++;
			token = strtok(NULL, " \t");
		}

		argv[argc] = NULL; // posix_spawnp requires null
						   // ignore empty or whitespace-only lines
		if (argc == 0)
		{
			continue;
		}

		// reject commands that are not allowed
		if (!isAllowed(argv[0]))
		{
			printf("NOT ALLOWED!\n");
			continue;
		}

		// exit shell
		if (strcmp(argv[0], "exit") == 0)
		{
			return 0;
		}

		// list the commands in required order
		if (strcmp(argv[0], "help") == 0)
		{
			printf("The allowed commands are:\n");
			for (int i = 0; i < N; i++)
			{
				printf("%d: %s\n", i + 1, allowed[i]);
			}
			continue;
		}

		// change this shell's working directory
		if (strcmp(argv[0], "cd") == 0)
		{
			if (argc > 2)
			{
				printf("-rsh: cd: too many arguments\n");
			}
			else if (argc == 2)
			{
				if (chdir(argv[1]) != 0)
				{
					// print error to stdout
					printf("-rsh: cd: cannot change directory to %s\n", argv[1]);
				}
			}
			else
			{
				chdir(getenv("HOME"));
			}
			continue;
		}

		// remaining commands run
		fflush(stdout);
		int result = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
		if (result != 0)
		{
			printf("-rsh: could not execute %s\n", argv[0]);
			continue;
		}

		// wait till finish for new command
		if (waitpid(pid, &status, 0) == -1)
		{
			printf("-rsh: could not wait for %s\n", argv[0]);
		}
	}
	return 0;
}
