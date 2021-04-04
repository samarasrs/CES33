#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


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


void launch(char ***g_tokens,int infile, int outfile, int errfile)
{
        pid_t pid, wpid;
        int status;
        int mypipe[2];
        
	

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
			outfile = 1;/////////////
		pid = fork();
		if(pid == 0)
		{
			exec(g_tokens[i], infile, outfile, 2);
			
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
	char *str1, *str2, *str3, *token, *subtoken, *subtoken2;
        char *saveptr1, *saveptr2,*saveptr3;
        int j, k, i = 0, cont = 0;

	//vetor para armazenar as partes da linha de comando
	char **tokens;
	int size_tokens = 64;

	int std[3]= {0, 1, 2};

	char ***g_tokens;
	
		      	
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
					//char d = "<";
                  			subtoken2 = strtok_r(str3, " \t\n", &saveptr3);
                   			if (subtoken2 == NULL)
                       				break;
					
                   			//printf("     3-->  %s\n", subtoken2);
               				

					tokens[i] = subtoken2;
					i++;
			 	  }
		   	}
			else
			{
				tokens[i] = subtoken;
				i++;
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
		
		//launch(tokens);
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
		//printf("88888888888 %s\n",g_tokens[j-1][0]);
		/*for(int a = 0; tokens[a] != NULL; a++)
		{
			printf("tokens: %d - %s\n", a, tokens[a]);
		}*/	

		//launch(tokens);
   	
        }
	g_tokens[cont] = NULL;
	/*
	for(int i=0; g_tokens[i] != NULL; i++)
	{	
		printf("g_token %d\n", i);
		char **tokens = g_tokens[i];
		for(int k=0; tokens[k] !=NULL; k++)
		{
			printf("\t tokens: %d - %s\n", k, tokens[k]); 
		}
	}*/
	launch(g_tokens, 0, 1, 2);
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
	//
	nread = 1;
	int cont = 0;
	while(nread != -1)
	{
		printf("\ncmd> ");
		nread = getline(&line, &len, stdin);
		
		//printf para dbug
		printf("Linha lida: %s\n",line);
		split_line(line);
		//printf("\ncmd> ");
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
