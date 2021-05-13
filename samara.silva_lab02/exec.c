#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>




void exec(char **args, int infile, int outfile, int errfile)
{
	char *envp[] = {"", NULL};

	if (infile != STDIN_FILENO)
	{
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if (outfile != STDOUT_FILENO)
        {
                dup2(outfile, STDOUT_FILENO);
                close(outfile);
        }
	if (errfile != STDERR_FILENO)
        {
                dup2(errfile, STDERR_FILENO);
                close(errfile);
        }

	execve(args[0], args, envp);
	perror("execve");
	exit(EXIT_FAILURE);

}


void launch(processo *p)
{
    pid_t pid, wpid;
    int status;
    int mypipe[2], infile, outfile, errfile;

	infile = p->std_in;
	errfile = p->std_err;
	char ***g_tokens;

	g_tokens = p->g_tokens;     
	

	for( int i = 0; g_tokens[i]!=NULL ; i++)
	{
		if(g_tokens[i+1]!=NULL)
		{
			if(pipe(mypipe) < 0)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			outfile = mypipe[1]; 
		}
		else
			outfile = p->std_out;
		pid = fork();
		if(pid == 0)
		{
			exec(g_tokens[i], infile, outfile, errfile);
			
		}
		else if (pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else 
		{

			do {
      				wpid = waitpid(pid, &status, WUNTRACED);
    			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
  		}
		if (infile != 0)
			close (infile);
      		if (outfile != 1)
        		close (outfile);
     		infile = mypipe[0];
        }
}