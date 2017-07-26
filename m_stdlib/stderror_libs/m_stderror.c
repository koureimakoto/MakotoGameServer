#define M_SUCESS_SERVER 0
#define M_SUCESS_SEND 1
#define M_SUCESS_RECV 2
#define M_TIMER 3
#define M_TIMER_OUT 4

#define M_STD_ERROR -1
#define M_ERROR_SEND
#define M_ERROR_RECV
#define M_TIMEOUT

#define M_FATAL_ERROR -2
#define M_FATAL_ERROR_CHLD -3
#define M_FATAL_ERROR_THRD -4

//THREADS
#define M_MAX_THREADS 2
#define B_SIZE 255

//POLLING
#define M_POLL_NFDS 3

//DEFINE DE CORES
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


int ext_count = 0;


void
console_function_log(char *msg)
{
  fprintf(stdout, " ##%d \t>> %s\n\n", ext_count++, msg);
  fflush(stdout);
}


//Não executa nenhuma tarefa à partir desta função, apenas identifica o erro
void m_std_error()
{
  fprintf( 
    stderr,                     //Saída padrão de erro
    "Error number: %d - %s\n",  
    errno, strerror(errno)      //errno + mensagem do tipo do errno
         );

  //Para liberar o fluxo quase o mesmo esteja preso
  fflush(stdout);
}

//Para fluxos de erros não padrões
void m_msg_error(char *msg)
{
  fprintf( 
    stderr,                   //Saída padrão de erro
    "Error number: %d - %s\n"
    "MSG>> %s",  
    errno, strerror(errno),   //errno + mensagem do tipo do errno
    msg                      //Mensagem não padrão do desenvolvedor  
         );

  //Para liberar o fluxo quase o mesmo esteja preso
  fflush(stdout);
}

//Identifica e trata o erro padrões do jogo
void m_error( int flags )
{
   m_std_error();     
          
   if( flags == M_FATAL_ERROR )  //Erro no processo pai
   {
      exit(EXIT_FAILURE);
   }   
   if( flags == M_FATAL_ERROR_CHLD )  //Erro no processo filho
   {
      _exit(EXIT_FAILURE);
   }   
   if( flags == M_FATAL_ERROR_THRD )  //Erro na posix threads
   {
      //pthread_exit(NULL);
   }
}
