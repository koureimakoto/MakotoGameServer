#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../../m_stdlib/stdnetwork_lib/m_stdnetwork.c"

#define SERVPORT "22222"



typedef struct connection
{
  STD_CONN      conn;
  ALLEGRO_MUTEX *mutex;
  ALLEGRO_COND  *cond; 
} CONN_CLIENT;

int
  pross_count = 0;

struct
  pollfd game_polling;



/***
 *
 *   Cria um cliente TCP ou UDP
 *
 */
static int openConnetion( struct addrinfo *result, m_sockopt *opt)
{
  /*VARIAVEIS-----------------------------------------------------------------*/
  int 
    sockfd,     //Arquivo descritor do socket, que será retornado 
    count = 0;  //contador no número de falhas nas tentativas de conexões
  
  struct addrinfo
   *p_rslt;     //Ponteiro para percorrer a lista de addrinfo
   
  /*CONEXAO-------------------------------------------------------------------*/ 
  //Alocação do ponteiro de addrinfo
  p_rslt = (struct  addrinfo *)malloc( sizeof(struct addrinfo ));
  if(!p_rslt)
    {
    m_error( M_FATAL_ERROR );
    }

  //Percorre a lista até realizar uma associação valida dentro da lista addrinfo
  for(p_rslt = result ; p_rslt != NULL; p_rslt = p_rslt->ai_next, count++)
    {
    //Formação do Socket
    sockfd = socket( p_rslt->ai_family, p_rslt->ai_socktype, p_rslt->ai_protocol );
    if( sockfd == -1 ) 
      {
      m_error( M_STD_ERROR ); 
      }
    else
      {
      console_function_log("Socket criado com "GREEN"SUCESSO"RESET);
      //Fixando a socket para reuso
      if( opt != NULL )
        {
        if( setsockopt(sockfd, opt->level, opt->optname, &opt->optval, opt->optlen ) == -1 )
          {
          close(sockfd);
          m_error( M_FATAL_ERROR );
          }
          console_function_log("Opcoes do socket setadas "GREEN"SUCESSO"RESET);
        }
      //Conectando com o servidor
      if( connect(sockfd, p_rslt->ai_addr, p_rslt->ai_addrlen) == -1 )
        {
        perror("Connect error");
        continue;
        }
      else
        {
        console_function_log("Conexao com servidor realizada com "GREEN"SUCESSO"RESET" porta associada ao IP");
            
        freeaddrinfo(result);
        if( p_rslt == NULL )
          {
          m_error( M_FATAL_ERROR );
          }
          return(sockfd);
        }
      }
    }
  return(-1);
}



bool
turnPlaySend( int32_t sockfd, const int32_t play )
{
  //Envia a jogada do usuario para o servidor
  if( send( sockfd, &play, sizeof(int32_t), 0 ) == -1 )
    {
    m_msg_error("Falha no envio da jogada\n");
    return false;
    }
  return true;
}

bool
turnPlayRecv( struct move *msg, const int32_t sockfd )
{
  //Envia a jogada do usuario para o servidor
  //if( recv(sockfd, (struct move *)msg, sizeof(struct move), 0 ) == -1 )
  //  {
  //  m_msg_error("Falha no recebimento da jogada\n");
  //  return false;
  //  }

  printf("FLAG: %d -- RSLT: %d\n", msg->flag, msg->rslt);

  return(true);
}

void *pollingThread(ALLEGRO_THREAD *thread, void *arg)
{
  CONN_CLIENT
   *client = (CONN_CLIENT *) arg;
 
  STD_MOVE
   move;

  memset(&move, 0, sizeof(STD_MOVE));
  memset(&game_polling, 0, sizeof(struct pollfd));
  game_polling.fd         = client->conn.sockfd;
  game_polling.events     = POLLIN | POLLPRI;

  bool
    done = true;

  int16_t
    count = 0;

  int32_t
    nbytes = 0,
    recv_bytes,
    poll_value;

  while( done )
    {
    poll_value = poll( &game_polling, 1 , 2000);  //5000ms = 5s 
    
    if(poll_value == -1 )
      {
      m_msg_error("Ocorreu algum erro no Polling do Cliente\n");
      }
    else if(poll_value == 0 )
      {
      m_msg_error("Tempo de poll esgotado\n");
      }
    else
      {
      if(game_polling.revents & POLLIN )
        {
        ioctl(game_polling.fd, FIONREAD, &nbytes );
        if( nbytes == 0 )
          {
          m_msg_error("Recebeu um fluxo de dados ineficiente:\n");
          tcflush(game_polling.fd, TCIFLUSH);
          count++;
          }
        else
          {
          printf("nbytes: %d\n", nbytes);
          fflush(stdout);
          recv_bytes = recv(client->conn.sockfd, &move, sizeof(STD_MOVE), 0);
          if(recv_bytes == -1)
            {
            m_msg_error("Erro no recebimento do resultado");
            }
          else
            {
            printf("Mensagem recebida com sucesso para o servidor\n");
            printf("Resultado: %s\n", move.rslt == M_DRAW ? "EMPATOU" : 
                                     (move.rslt == M_WIN  ? "SOU VITORIOSO" :
                                     (move.rslt == M_LOSE ? "FUI DERROTADO " : "ERROR_VALUE ") ));
            client->conn.attack = move;
            client->conn.flag   = CLIENT_RESULT;
            done = false;
            }
          }
        }
      }
    }
  return NULL;
}

void initComm(int32_t sockfd)
{
  char 
    msg_flag;

  int32_t
    recv_bytes;

  do{
    recv_bytes = recv(sockfd, &msg_flag, sizeof(char), 0);
    if(recv_bytes == -1)
      {
      m_msg_error("Erro no recebimento da mensagem inicial do Servidor\n");
      msg_flag = 0x00;
      }
  printf(" ## %d \t>> Servidor enviou: %s \n", pross_count, (msg_flag == GAME_START ? "GAME START" :
                                                            (msg_flag == WAIT_TO_START ? "WAIT TO START" : "ERROR_VALUE") ) );
  fflush(stdout);
  }while(msg_flag != GAME_START);
}


void networkClient( CONN_CLIENT *conn_client )
{
  //VARIAVEIS INICIAIS
  struct addrinfo
    client,
   *server;                

  int32_t
    addrinfo_value;	

  //Preparando socket
  memset(&client, 0, sizeof(client));
  client.ai_family   = AF_UNSPEC;
  client.ai_socktype = SOCK_STREAM;

  if( (addrinfo_value = getaddrinfo("127.0.0.1", SERVPORT, &client, &server) ) != 0 )
    {
    perror("Getaddrinfo error");
    m_error( M_FATAL_ERROR );
    }
  printf(" ## %d \t>> Preparada opcoes de conexao com servidor\n", pross_count++);
  fflush(stdout);


  conn_client->conn.sockfd = openConnetion( server, NULL );
  printf(" ## %d \t>> Conexao estabelecida com "GREEN"sucesso\n"RESET, pross_count++);
  printf("  ## %d \t>> Arquivo descritor do Socket: %d\n", pross_count, conn_client->conn.sockfd);
  fflush(stdout);
}

