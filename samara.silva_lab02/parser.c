#define _GNU_SOURCE
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


struct processo
{
	char ***g_tokens;
	int std_in;
	int std_out;
	int std_err;

};


void trocar_stdout(processo *p, char *token){
	p->std_out = open(token, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if(p->std_out < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
}

void trocar_stdin(processo *p, char *token){
	p->std_in = open(token, O_RDONLY);
    if(p->std_in < 0)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

}


//função para dividir a linha
processo *split_line(char *line)
{
	//declaração de variaveis para strtok
	char *str1, *str2, *str3, *str4, *str5, *token1, *token2, *token3, *token4, *token5;
    char *saveptr1, *saveptr2,*saveptr3, *saveptr4, *saveptr5;
    int j1,j2, j3, j4, j5,  a = 0,i = 0, cont = 0, istd = -1, auxstd = -1;

	//vetor para armazenar as partes da linha de comando
	char **tokens;
	int size_tokens = 64;
	int std[3]= {0, 1, 2};
	char ***g_tokens;


	processo *p;

	p = NULL;
	p = (processo*)malloc(sizeof(processo));
	p->std_in = 0;
	p->std_out = 1;
	p->std_err = 2;
	
	char *aux1 = "<";
	char *aux2 = ">";

	g_tokens = malloc(size_tokens*sizeof(char**));
        if(!g_tokens)
        {
           perror("Alocation Error :(");
 	       exit(EXIT_FAILURE);
        }

	for (j1 = 1, str1 = line; ; j1++, str1 = NULL)
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
        token1 = strtok_r(str1, &a, &saveptr1);
        if (token1 == NULL)
                break;

		//printf para dbug
                //printf("%d: %s\n", j1, token1);
		for (j2 = 1, str2 = token1; ; j2++,  str2 = NULL)
       	{
			//separar cada parte dividida por < na variavel aux1
			token2 = strtok_r(str2, aux1, &saveptr2);
			if (token2 == NULL)
				break;
			//printf("- %d: %s\n", j2, token2);
			for (j3 = 1, str3 = token2; ;j3++,  str3 = NULL)
        	{
            	//separar cada parte dividida por > na variavel aux2
            	token3 = strtok_r(str3, aux2, &saveptr3);
            	if (token3 == NULL)
            	        break;
				//printf("-- %d: %s\n", j3, token3);
				if(strncmp(token3, " \"",2) == 0)
				{
					//printf("entrou\n");
					token3++;
				}
				//printf("-- %d: %s\n", j3, token3);
				if(strncmp(token3, "\"",1) == 0 || strncmp(token3, " \"",2) == 0)
				{
					a = 0;
				}
				else
					a=1;
				for (j4 = 1, str4 = token3; ; j4++,  str4 = NULL)
               	{
                   	//separar cada parte dividida por \"
                   	token4 = strtok_r(str4, "\"", &saveptr4);
                   	if (token4 == NULL)
                           	break;
					
					//printf("---- %d: %s\n", j4, token4);
					if (j4%2 == a)
					{
						for (j5 = 1, str5 = token4; ; j5++, str5 = NULL)
                    	{
                       		//separar cada parte dividida por " \t\n"
                      		token5 = strtok_r(str5, " \t\n", &saveptr5);
                        	if (token5 == NULL)
								break;
							
						
							if (j3 == 2 && j5 == 1)
							{								
                                trocar_stdout(p, token5);                             
							}
							else if (j2 == 2 && j3 == 1)
							{
								trocar_stdin(p, token5);

							}
							else
							{
								tokens[i] = token5;
                                i++;
							}
						}
					}
					else
					{
                        if (j3 == 2 && j4 == 1)
                        {
                            trocar_stdout(p, token4);
                            
						}                                              
                        else if (j2 == 2 && j3 == 1)
						{
							trocar_stdin(p, token4);
						}
						else
                        {
                          	tokens[i] = token4;
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

        //printf("stdin %d\n ", std[0]);
        //
        //printf("j3 -- %d std[1] = %d\n", j3, std[1]);

		
	p->g_tokens = g_tokens;

	return p;
}