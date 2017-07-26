#include "m_stderror.c"

//Não executa nenhuma tarefa à partir desta função, apenas identifica o erro
void m_std_error();

//Para fluxos de erros não padrões
void m_msg_error(char *msg);

//Identifica e trata o erro padrões do jogo
void m_error( int flags );
