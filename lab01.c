#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


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


void launch(char ***g_tokens,int in, int out, int err)
{
        pid_t pid, wpid;
        int status;
        int mypipe[2], infile, outfile, errfile;

	infile = in;
	errfile = err;       
	

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
			outfile = out;
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


//função para dividir a linha
char *split_line(char *line)
{
	//declaração de variaveis para strtok
	char *str1, *str2, *str3, *str4, *str5, *str6, *token, *subtoken, *subtoken2, *subtoken3, *subtoken4;
        char *saveptr1, *saveptr2,*saveptr3, *saveptr4, *saveptr5, *saveptr6;
        int j, k, i = 0, cont = 0, istd = -1, auxstd = -1;

	//vetor para armazenar as partes da linha de comando
	char **tokens;
	int size_tokens = 64;
	int std[3]= {0, 1, 2};
	char ***g_tokens;
	
	char aux1 = '<';
	char aux2 = '>';
	char *aux3 = "2>";


	g_tokens = malloc(size_tokens*sizeof(char**));
        if(!g_tokens)
        {
               perror("Alocation Error :(");
 	       exit(EXIT_FAILURE);
        }


	for (j = 1, str1 = line; ; j++, str1 = NULL)
        {
		//g_tokens = malloc(size_tokens*sizeof(char**));		
		tokens = malloc(size_tokens*sizeof(char*));

		//verificando se a alocação OK
		if(!tokens)
        	{
                	perror("Alocation Error :(");
                	exit(EXIT_FAILURE);
        	}
		i = 0;
		
		// a = "|"
		char a = 124;

		

		//separar a linha em partes dividida por "|"	
                token = strtok_r(str1, &a, &saveptr1);
                if (token == NULL)
                        break;

		//printf para dbug
                //printf("%d: %s\n", j, token);
		for (k = 1, str2 = token; ; k++,  str2 = NULL)
	       	{
			//separar cada parte dividida por "
			subtoken = strtok_r(str2, "\"", &saveptr2);
			if (subtoken == NULL)
				break;
			//printf para debug
			//printf("  2--> %d  %s\n", k, subtoken);
			
			//considerando que palavras entre aspas vao ser separadas por espaço	
		  	if(k%2 == 1)
		   	{
				
				for (str3 = subtoken; ; str3 = NULL)
				{
				    	subtoken2 = strtok_r(str3, " \a\r\t\n", &saveptr3);
                   			if (subtoken2 == NULL)
                       				break;
					
                   			//printf("     3-->  %s\n", subtoken2);
					
					if (strncmp(subtoken2, &aux1, 1) == 0)
					{
						istd = i;
						auxstd = 0;
					}
					else if (strncmp(subtoken2, &aux2, 1) == 0)
					{
						istd = i;
						auxstd = 1;

					}
					else if (strncmp(subtoken2, aux3, 1) == 0)
					{
						istd = i;
						auxstd = 2;

					}
					else
					{
						if (istd == i)
						{
							if(auxstd == 0)
							{
								std[0] = open(subtoken2, O_RDONLY);
								if(std[0] < 0)
								{
									perror("open");
									exit(EXIT_FAILURE);
								}
								auxstd = -1;
								istd = -1;
							}
							else 
							{
								std[auxstd] = open(subtoken2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
								if(std[auxstd] < 0)
                                                                {
                                                                        perror("open");
                                                                        exit(EXIT_FAILURE);
                                                                }
								auxstd = -1;
                                                                istd = -1;
							}
						}
						else
						{	
							
							
							tokens[i] = subtoken2;
                                        		i++;
						}
					}


			 	  }
		   	}
			else
			{
				if (istd == i)
                                {
        	                        if(auxstd == 0)
                                        {
                                        	std[0] = open(subtoken2, O_RDONLY);
						if(std[0] < 0)
                                                {
                                                	perror("open");
                                                        exit(EXIT_FAILURE);
                                                }
						auxstd = -1;
                                                istd = -1;
                                         }
                                         else
                                         {
                                         	std[auxstd] = open(subtoken2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                                if(std[auxstd] < 0)
                                                {
                                                	perror("open");
                                                        exit(EXIT_FAILURE);
                                                }
						auxstd = -1;
                                                istd = -1;
                                         }
				}
				else
				{
					tokens[i] = subtoken;
					i++;
				}
			}	
			if(i >= size_tokens)
			{
				tokens = realloc(tokens, size_tokens * sizeof(char*));
				if(!tokens)
				{
					perror("Alocation Error :(");
					exit(EXIT_FAILURE);
				}
			}
       		}
		tokens[i] = NULL;
		g_tokens[cont] = tokens;
		cont++;
		if(cont >= size_tokens)
		{
			g_tokens = realloc(g_tokens, size_tokens* sizeof(char**));
			if(!g_tokens)
			{
				perror("Alocation Error :(");
				exit(EXIT_FAILURE);
			}
		}
   	
        }
	g_tokens[cont] = NULL;
/*
	printf("********************************\n");
        for(int i=0; g_tokens[i] != NULL; i++)
        {
                printf("g_token %d\n", i);
                char **tokens = g_tokens[i];
                for(int k=0; tokens[k] !=NULL; k++)
                {
                        printf("\t tokens: %d - %s\n", k, tokens[k]);
                }
        }
        printf("********************************\n");
*/

        //printf("stdin %d\n\n\n", std[0]);
	
	
	launch(g_tokens, std[0], std[1], std[2]);
	free(tokens);
	free(g_tokens);
}	


//função para ler a linha de comando
char *read_line()
{
	//declarando as variaveis conforme man getline()
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	
	
	//se nao conseguir ler a linha pq ja chegou no fim do arquivo/comando retorna sucesso
	//se nao conseguir ler por outro motivo printa o erro e retorna falha
	printf("\ncmd> ");
	nread = getline(&line, &len, stdin);
	int cont = 0;
	while(nread != -1)
	{
		//printf para dbug
		//printf("nread : %ld  -- Linha lida: %s\n", nread, line);
		split_line(line);
		printf("\ncmd> ");
		nread = getline(&line, &len, stdin);
		
	}
			

	free(line);
	exit(EXIT_SUCCESS);
}




int main()
{
	printf("\nWelcome to the Samara's miniature-shell\n");	
	//printf("\ncmd> ");
	read_line();

	return 0;
}
