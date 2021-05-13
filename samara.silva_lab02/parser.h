typedef struct processo processo;


/*
	A função trocar_stdout trocar a saída padrão para o arquivo do comando recebido
*/

void trocar_stdout(processo *p, char *token);

/*
	A função trocar_stdin trocar a entrada padrão para o arquivo do comando recebido
*/
void trocar_stdin(processo *p, char *token);


/*
	A função split_line divide a linha de comando recebida levando em consideração 
	| " " "\"" < >
	a função retorna uma struct processo onde tem o g_tokens que possui todos os comandos separados 
	e cada coluna da matriz corresponde a um pipe
	e possui também as entradas e saídas padrão
*/
processo *split_line(char *line);


