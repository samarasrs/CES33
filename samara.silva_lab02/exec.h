#include "parser.h"

/*
	Na função exec ocorre a execução do comando
*/
void exec(char **args, int infile, int outfile, int errfile);

/*
	Na função launch ocorre o looping necessário para o pipe
*/

void launch(processo *p);