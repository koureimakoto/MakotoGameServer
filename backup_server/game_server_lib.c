#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define BACKLOG 10
#define SERVPORT "12345"

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


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */


/*VARIAVEIS_GLOBAIS-----------------------------------------------------------*/
char msg[B_SIZE], buffer[B_SIZE];
pthread_mutex_t lock_msg;


/*STRUCTS---------------------------------------------------------------------*/
typedef struct sockopt
{
   int sockfd;
   int level;
   int optname;
   socklen_t optlen;
   void *optval;   
} m_sockopt;
//cardinfo.flags
/* 0 - 0 - 0 - 0 - 0 - 0 - 0 - 0 | 0 - 0 - 0 - 0 - 0 - 0 - 0 - 0
   15- 14- 13- 12- 11- 10- 9 - 8   7   6   5   4   3   2   1   0

   8 - 0 : card type 0x0100 : type
                     0x0001 : Neutral
                     0x0002 : Drarkness    
*/



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


typedef struct mat_data {
   int   ply_flag;    //Flag de controle
   int   tid[2];  //Ida da Thread
   float value[2];//Valores do jogo
   float rslt;    //Resultado
	
	
} MAT_DATA_T;



/*THREADS---------------------------------------------------------------------*/

//Thread Jogo Matematica
void *mat_thread( MAT_DATA_T *m_data )
{
	printf("Thread %d criada com sucesso\n",m_data->tid[m_data->flag])
	pthread_exit(NULL);
}



//Thread Test do servidor.
void *client_thread( c_thread_data * c_data )
{
  /*******************************/
  /*VARIAVEIS                    */
  /*******************************/
  size_t send_bytes;
  size_t recv_bytes;
  /*FIM_VARIAVEIS                */

  /*******************************/
  /*INICIALIZAÇÃO DA THREAD      */
  /*******************************/

  //AVISA QUE O THREAD FOI CRIADO COM SUCESSO
  printf( GREEN "GERADO THREAD CLIENTE: " RED " %d\n\n " RESET , c_data->tid);

  sprintf( msg, "Thread %d\n", c_data->tid );
   

  /*******************************/
  /*COMUNICAÇÃO DA THREAD        */
  /*******************************/

  //Envia para o cliente uma pequena mensagem e recebe o tamanho dos bytes em 
  //send_bytes. Se igual a -1, erro de envio.
  if( (send_bytes = send( c_data->fd , &msg, sizeof(msg), 0)) == -1 )
    {
    perror("Thread Error Send\n");
    }
  puts("Enviado com sucesso para o Cliente contendo o ID da Thread\n");
   
  /*INICIA MUTEX-------------------------------------------------------------*/ 
  pthread_mutex_lock( &lock_msg );
    {
  //Recebe a mensagem do cliente confirmando o recebimento da mensagem armaze-
  //a o tamanho de bytes em recv_bytes. Se igual a -1, erro ao receber
    if( (recv_bytes = recv( c_data->fd, &buffer, sizeof( buffer ) -1, 0 )) == -1 )
      {
      perror("Thread Error Recv");
      }

    puts("Recebido com" GREEN " SUCESSO" RESET " confirmação de mensagem do cliente");

    buffer[recv_bytes] = '\0';
  
    flockfile(stdout);
      {
      printf("MENSAGEM FINAL DO CLIENTE:" GREEN " %s\n\n" RESET ,buffer);
      fflush(stdout);
      }
    funlockfile(stdout);
    }
  pthread_mutex_unlock( &lock_msg );
  /*FINALIZA_MUTEX----------------------------------------------------------*/
  //Fecha o Arquivo descritor escrito para o [int accept()]
  close(c_data->fd);
  //Encerra a thread
  pthread_exit(NULL);
}

/*FUNCOES---------------------------------------------------------------------*/


/*void m_whitelog( int flags )
{
   FILE * serverlog;

   if( ( serverlog = fopen("game_log.txt", "r+") == NULL )
   {
      perror("Erro na abertura do arquivo\n");
      fclose(serverlog);
   }
   else
   {
      fprintf( stdout, "aberto com sucesso\n");
      fseek( serverlog, 0L, SEEK_END);
      //ERROR LOG
      //SUCESS LOG   
   }
}
//Devolve um lista do arquivo
void m_readlog(){}
*/

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

   pthread_t     userthr[M_MAX_THREADS]; //Número máximo de threads
   c_thread_data c_data[M_MAX_THREADS];  //Dados passados à thread

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
   //Inicializando variavel global das threads
   memset(&msg[0], 0, sizeof(msg));

   //Inicializando mutex par lock_msg
   pthread_mutex_init( &lock_msg, NULL );

   //Listen
   if( (listen(serverfd, BACKLOG)) == -1 )
   {
      m_error( M_FATAL_ERROR );
   }
   while(1)
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
      c_data[plynum].tid = plynum;
      c_data[plynum].fd = accptfd[plynum];
      if( thread_r = pthread_create( &userthr[plynum], NULL, (void *)client_thread, &c_data[plynum] ) )
      {
         perror("Criação da thread");
         m_error( M_FATAL_ERROR );
      }


      //Threads enviam as msg para os usuarios--
      //Usuario retorna msg--
      //threds escrevem as msg na struct de inicio deo jogo
      //servidor vai estar em while(infinito) aguardando estas variavies serem setadas
      //assim que settadas ele envia msg as threads para o elas avisarem os clientes que esta sendo preparada a partida
      //Usuario envia as cartas que ele irá usar

      //Servidor embaralha as cartas dos dois usuarios, isto ocorrerá dentro de cada thread

      //servidor cria um servidor de tempo UDP para enviar msg via multicast
      //servidor envia msg aos usuarios pedindo apra eles abrirem o servidor de tempo
      //servidor entra em um novo while com struct de tempo real
      //recebida confirmação do usario as threads settam o struct de tempo
      //servidor envia as cartas vi TCP com os dados de quem será o primeiro jogador
      //servidor settas as informaçãos da partida para que as threads saibam quem deve escutar primeiro
      
      //Clientes avisam que começarão
      //servidor envia TEMPO da jogada em multicast


      /*Enquanto x1 nao perder, x2 nao perder ou tempo total nao acaba, continua*/
      //Se tempo acabar thread 
         //de escuta fecha x1 fecha
         //Servidor prepara novo turno
      //Se nao
         //jogador x1 faz sua primeira jogadas
         //Servidor prepara novo turno

      //Finalizada partida servidor verifca quem ganhou e setta struct vitoria_derrota de cada jogador e as envia 

      //Jogar avisa ter recebido o fim da partida
      //Threads fecham a partida
      //Servidor limpa os buffers
      //Servidor return 0 e encerra


      plynum++;
            
      
   }

   return 0;
}
