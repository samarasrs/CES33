#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "parser.c"

#include "exec.h"
#include "exec.c"


//função para ler a linha de comando
char *read_line()
{
	//declarando as variaveis conforme man getline()
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	processo *proc;
	
	
	//se nao conseguir ler a linha pq ja chegou no fim do arquivo/comando retorna sucesso
	//se nao conseguir ler por outro motivo printa o erro e retorna falha
	printf("\ncmd> ");
	nread = getline(&line, &len, stdin);
	int cont = 0;
	while(nread != -1)
	{
		//printf para dbug
		//printf("nread : %ld  -- Linha lida: %s\n", nread, line);
		proc = split_line(line);
		launch(proc);

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