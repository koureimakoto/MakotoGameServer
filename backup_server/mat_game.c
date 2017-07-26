#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BACKLOG 10
#define SERVPORT "22222"

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


/*STRUCTS---------------------------------------------------------------------*/
typedef struct sockopt
{
   int sockfd;
   int level;
   int optname;
   socklen_t optlen;
   void *optval;   
} m_sockopt;

typedef struct gameplay
{
   int game_id;
   short game_players[2];
   clock_t game_start;
   clock_t game_current;
   const clock_t game_end;
} GAMEPLAY;

typedef struct cliente_thread_data_t {
  int tid;
  int fd;
  char msg[B_SIZE]; 
} c_thread_data;


#define FIRST_PLY 0
#define SECON_PLY 1

typedef struct mat_calc
{
   float value[2];//Valores do jogo
   float rslt;    //Resultado
} M_DATA_T;

typedef struct mat_data {
   int  tid;
   int  ply_fd;    //Flag de controle
   int  ply_flag;
   M_DATA_T data_t;
} M_GAME_T;

typedef struct mat_players {
   int     plynum;
   int     game_flag;
   int32_t game_flag_conv;
   size_t  game_flag_size;
   char   *game_flag_char;
} M_PLAYER_T;

/*VARIAVEIS_GLOBAIS-----------------------------------------------------------*/
pthread_mutex_t MUTEX_mat_data;
M_PLAYER_T players;

/*THREADS---------------------------------------------------------------------*/

//Jogo Matematica*****************************************************

void handler(int signal)
{
   perror("han");
   exit(-1);
}


/*
 * Thread implementada com conceito de polling em cima de poll()
 * teste realizado para correação de falha de comunicação no recebimento
 * de mais de uma mensagem send(), em situação de recv( WAITMSG )
 */

void *mat_thread( M_GAME_T *m_game )
{
   //Variaveis
   int  byte_send,                         //Bytes enviados da comunicação 
        byte_recv,                         //Bytes recebidos da comunicação
        poll_value; 

   char buff_answer[B_SIZE] = "CONECTADO"; //Resposta ao cliente
 
   struct pollfd server_poll[M_POLL_NFDS];              //Estrutura para usar POLL
   
	
   //Print da criação com sucesso da Thread
   printf("Thread %d criada com sucesso | Arquivo descritor %d \n",m_game->tid, m_game->ply_fd);
   
   //Primeira mensagem enviada ao cliente
   printf("Thread %d envia mensagem ao cliente\n", m_game->tid );
   if(( byte_send = send(m_game->ply_fd, &buff_answer, strlen(buff_answer), 0) ) == -1)
     {
     //Abortar a Thread por erro
     perror("Problemas no envio da mensagem de inicio");
     pthread_exit(NULL);
     }
   else
     {
     //fflush(stdout) forçará o flush de saida a imprimir do buffer para não segurar a aplicação
     fflush(stdout);
     //Mensagem enviada com sucesso
     printf("-----Enviado a mensagem com" GREEN " sucesso\n" RESET); 
     }


   server_poll[0].fd = m_game->ply_fd;
   server_poll[0].events = POLLIN ;
   //server_poll[1].events = POLLIN | POLLPRI;
   //server_poll[2].events = POLLOUT;

   signal(SO_NOSIGPIPE, handler);

   int x = 0;
   while("happy")
     {
     //Prepara o poll
     poll_value = poll( server_poll, M_POLL_NFDS , 2000);  //5000ms = 5s   
   
     printf("DENTRO DO POLL : %d - COUNT : %d\n",  poll_value, x++ );
     
     if( poll_value == -1 )
       {
       perror("Ocorreu falha com o polling POLL\n");
       pthread_exit(NULL);
       }
     else if( poll_value == 0 )
       {
       perror("Tempo esgotado\n");
       continue;
       }   
     else
       {
       if( server_poll[0].revents & POLLIN )
         {
         byte_recv = recv( m_game->ply_fd, &x, sizeof(int), 0 );
         if( byte_recv == 0 )
            perror("Deu zero");
         if( byte_recv == -1 )
           {
             perror("Erro no recebimento da mensagem");
             //Acordar minha thread de envio referente a este erro
           }
         else
           { 
             byte_send = send(m_game->ply_fd, "OI", 
                              sizeof(players.game_flag_conv), 0 );
 
             printf("GAME FLAG: %d - ByTES: %d\n",players.game_flag, byte_send);
             pthread_exit(NULL);
             }
         }
       }
     }
}


void m_outputlog( int flags )
{
   switch(flags)
   {
      case M_SUCESS_SERVER  :
      {
         printf("Sucesso na criação do servidor"); 
         break;
      }
      case M_SUCESS_SEND    :
      {
         printf("Sucesso no envio do mensagem");
         break;
      }
      case M_SUCESS_RECV    :
      {
         printf("Sucesso ao receber ao mensagem");
         break;
      }
      case M_TIMER   :
      {
         printf("Cronometrado esgotado");
         break;
      }
      case M_TIMER_OUT:
      {
         printf("Tempo esgotado da partida");
         break;
      }
   }
}

void m_std_error()
{
   fprintf( stderr,
            "Error number: %d - %s\n",
            errno, strerror(errno)
          );
}

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
      pthread_exit(NULL);
   }
}
int close_server( int sockfd )
{
   if( close( sockfd ) == 0 )
   {
      puts("Socket Fechado com Sucesso");
      return(0);
   }
   return(-1);
}

//Função para setar os parametros da setsockopt com de forma a minimizar o 
//impacto visual no main e validar alguns parametros antes
int m_prepar_sockopt( m_sockopt *opt, int level, int optname, void *optval)
{
   //Limpando a memoria da struct sockopt
   memset(opt, 0, sizeof(struct sockopt));   
   if(opt == NULL)
   {
      printf("Erro de memória em prepar_sockopt\n");
      return(-1);
   } 

   if( (level > 0) && (optname > 0) )
   {
      opt->level  = level;        //VARIAVEL DA STRUCT SOCKOPT
      opt->optname= optname;      //VARIAVEL DA STRUCT SOCKOPT
      opt->optlen = sizeof(optval);  //VARIAVEL DA STRUCT SOCKOPT  

      opt->optval = (void *) malloc( opt->optlen );
      if( opt->optval == NULL )
      {
         printf("Erro de alocação de memória em prepar_sockopt");
         return(-1);
      }
      opt->optval = optval;          //VARIAVEL DA STRUCT SOCKOPT 
   }
   else
   {
      printf("ERRO ao tentar receber valores negativos em prepar_sockopt\n");
      printf("Valores para:\n   option level = %d\n   option name = %d\n", level, optname);
      return(-1);
   }
}



//Cria um servidor TCP ou UDP
int create_server( struct addrinfo *result, m_sockopt *opt)
{
   /*VARIAVEIS----------------------------------------------------------------*/
   int sockfd,     //Arquivo descritor do socket, que será retornado 
       on    = 1,  //on or off 
       count = 0;  //contador no número de falhas nas tentativas de conexões
   struct addrinfo *p_rslt; //Ponteiro para percorrer a lista de addrinfo
   
   /*FIM_VARIAVEIS------------------------------------------------------------*/
   
   //Alocação do ponteiro de addrinfo
   p_rslt = (struct  addrinfo *)malloc( sizeof(struct addrinfo ));
   if( p_rslt == NULL)
   {
      m_error( M_FATAL_ERROR );
   }

   //Percorre a lista até realizar uma associação valida dentro da lista
   for(*p_rslt = *result ; p_rslt->ai_next != NULL; *p_rslt = *p_rslt->ai_next, count++)
   {
      //Formação do Socket
      sockfd = socket( p_rslt->ai_family, p_rslt->ai_socktype, p_rslt->ai_protocol );
      if( sockfd == -1 ) 
      {
         m_error( M_STD_ERROR ); 
      }
      else
      {
         puts("Socket criado com sucesso \n");
         //Fixando a socket para reuso
         if( opt != NULL )
         {
            if( setsockopt(sockfd, opt->level, opt->optname, &opt->optval, opt->optlen ) == -1 )
            {
               close(sockfd);
               m_error( M_FATAL_ERROR );
            }
            puts("Socket opt realizado com sucesso\n");
         }
         //Associando o bind
         if( (bind(sockfd, p_rslt->ai_addr, p_rslt->ai_addrlen)) == -1 )
         {
            close(sockfd);
printf("\n\noi\n\n");
            m_error( M_STD_ERROR );
         }
         else
         {
            puts("Porta associada com sucesso ao ip\n");
            
            freeaddrinfo(result);
            if( p_rslt == NULL )
            {
               m_error( M_FATAL_ERROR );
            }
            return(sockfd);
         }
      }
   }


}

/*Função de BEEJ.US, apenas comentanda - Ela tem o objetivo de recuperar o tipo
de ip iniciado na struct */
void *get_in_addr(struct sockaddr *servaddr)
{
   /*Valor de AF_INET e PF_INET são os mesmo*/
   if( servaddr->sa_family == AF_INET ) 
   {
       return( &(((struct sockaddr_in*)servaddr)->sin_addr) );
   }
   /*Caso o não seja IPv4 retorna IPv6*/ 
   return( &(((struct sockaddr_in6*)servaddr)->sin6_addr) );
}

void sigchld_handler()
{//Funcao de GJEE
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}





int main( int argc, char *argv )
{
   /*VARIAVEIS----------------------------------------------------------------*/
   struct addrinfo  server;              //Arma informações do servidor
   struct addrinfo *p_server;            //ponteiro da struct do servido
   struct addrinfo *p_serverinfo;        //Informaçãoes do servidor

   struct sigaction siga;                //signal action
   struct sockopt   sopt;                //socket option
   struct sockaddr_storage useraddr; 
   
   //struct poolfd poll_fd;

   socklen_t useraddr_size;              //Tamanho da estrutura do socket

   int serverfd;                         //File Descriptor do socket
   int timerfd;                          //File Descriptor do cronometro em UDP
   int accptfd[10];                      //File Descriptor do accept
   int ai_value;                         //Return do getaddrinfo()
   int plynum = 0;                       //Contador de players
   int thread_r;                         //Retorna ao criar threads

   const unsigned int yes = 1;           //Uma constante positiva = SIM
   const unsigned int no  = 0;           //Uma constante positiva = NÃO
             
   char s[INET6_ADDRSTRLEN];

   pthread_t userthr[M_MAX_THREADS]; //Número máximo de threads
   M_GAME_T  game_data[M_MAX_THREADS];

   /*FIM_VARIAVEIS------------------------------------------------------------*/

   /*Inicialização dos parametros para realizar o Socket*/
   memset(&server, 0, sizeof(server));
   server.ai_family   = AF_UNSPEC;    //Aceitar IPv4 ou v6
   server.ai_socktype = SOCK_STREAM;  //TCP
   server.ai_flags    = AI_PASSIVE;   //Localhost 

   //Inicializnado os parametros para a criação do socket
   if( (ai_value = getaddrinfo(NULL, SERVPORT, &server, &p_serverinfo )) != 0 )
   { 
      //Caso de erro o servidor encerra
      m_error(M_FATAL_ERROR);
   }
   
   //Inicializa os parametros que serão usados no [int setsockopt()]
   m_prepar_sockopt( &sopt, SOL_SOCKET, SO_REUSEADDR, (int *)yes);

   printf("1 - OPTION LEVEL:%d \n \
           2 - OPTION NAME :%d \n \
           3 - OPTION VALUE:%d \n", sopt.level, sopt.optname, sopt.optval);
   
   /*Criando socket principal do servidor*/

   serverfd = create_server(p_serverinfo, &sopt);


   //Código de BJEE-------------------------------------------------------------
   siga.sa_handler = sigchld_handler; // reap all dead processes
   sigemptyset(&siga.sa_mask);
   siga.sa_flags = SA_RESTART;
   if (sigaction(SIGCHLD, &siga, NULL) == -1)
   {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }
   //FIM Código de BJEE---------------------------------------------------------

   //Inicializando mutex par lock_msg
   pthread_mutex_init( &MUTEX_mat_data, NULL );

   //Listen
   if( (listen(serverfd, BACKLOG)) == -1 )
   {
      m_error( M_FATAL_ERROR );
   }
   while(plynum < 2)
   {
      //Accept
      useraddr_size = sizeof(useraddr);
      accptfd[plynum] = accept( serverfd, ( struct sockaddr *)&useraddr, &useraddr_size);
      if( accptfd[plynum] == -1 )
      {
         m_error(M_STD_ERROR);
         continue;
      }
      
      inet_ntop(useraddr.ss_family,
                get_in_addr((struct sockaddr *) &useraddr ),
                s, sizeof(s));
      printf("Servidor: Conectou-se atraves " RED " %s\n" RESET ,s);

      //Threads
      game_data[plynum].tid    = plynum;
      game_data[plynum].ply_fd = accptfd[plynum];
      if( thread_r = pthread_create( &userthr[plynum], NULL, (void *)mat_thread, &game_data[plynum] ))
      {
			perror("Criação da thread");
         m_error( M_FATAL_ERROR );
		}
      
      if(plynum < 1)
      {
         players.game_flag = 0;
         players.game_flag_conv = htonl(players.game_flag);
         players.game_flag_size = sizeof(players.game_flag_conv);
      }
      else
      {
         players.game_flag = 1;
         players.game_flag_conv = htonl(players.game_flag);
         players.game_flag_size = sizeof(players.game_flag_conv);
      }
      
      printf("Teste\n");
      
      plynum++;
   }
	
	sleep(10);

   return 0;
}
