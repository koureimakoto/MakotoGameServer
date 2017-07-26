#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../m_stdlib/stdnetwork_lib/m_stdnetwork.c"
#include "../m_stdlib/stderror_libs/m_stderror.c"





/***
 *
 *   Cria um cliente TCP ou UDP
 *
 */
static int openBind( struct addrinfo *result, m_sockopt *opt)
{
    /*VARIAVEIS----------------------------------------------------------------*/
  int 
    sockfd,     //Arquivo descritor do socket, que será retornado 
    count = 0;  //contador no número de falhas nas tentativas de conexões
  
  struct addrinfo
   *p_rslt;     //Ponteiro para percorrer a lista de addrinfo
   
  /*CONEXAO-----------------------------------------------------------------*/ 
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
      if( (bind(sockfd, p_rslt->ai_addr, p_rslt->ai_addrlen)) == -1 )
        {
        close(sockfd);
        m_error( M_STD_ERROR );
        }
      else
        {
        console_function_log("Bind realizado com "GREEN"SUCESSO"RESET" porta associada ao IP");
            
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
{//Funcao de GJE
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}






int
main(void)
{
  /*VARIAVEIS----------------------------------------------------------------*/
  struct addrinfo
    server,              //Arma informações do servidor
   *p_server,            //ponteiro da struct do servido
   *p_serverinfo;        //Informaçãoes do servidor

  struct sigaction
    siga;                //signal action

  struct sockopt
    sopt;                //socket option

  struct sockaddr_storage
    useraddr; 

  struct pollfd
    polling[2];
   
  STD_CONN
    game_data[2];

  STD_MOVE
    std_move;

  socklen_t
    useraddr_size;             //Tamanho da estrutura do socket

  char
    msg_flag;                  //Mensagens de flag do estado de jogo

  bool
    ply_flag[] = {false, false}; //

  int32_t 
    serverfd,                  //File Descriptor do socket
    accptfd[2],                //File Descriptor do accept
    poll_value,                //Retunr dos eventos por poll()
    addrinfo_value,            //Return do getaddrinfo()
    pross_count = 0,           //Contador de processos 
    ply_num     = 0,           //Contador de players
    ply_count   = 0,           //Contador para contador de player
    recv_bytes  = 0,           //Numero de bytes recebidos via recv()
    send_bytes  = 0;           //Numero de bytes enviados via send()

  const unsigned int 
    yes = 1,           //Uma constante positiva = SIM
    no  = 0;           //Uma constante positiva = NÃO
             
  char
    s[INET6_ADDRSTRLEN];


  /*FIM_VARIAVEIS-------------------------------------------------------------*/
  printf("\n\n\n\n"
         "################################################################################\n"
         "                               "GREEN"MAKOTO GAME SERVER"RESET"\n"
         "                                   "YELLOW"VERSION 1.0"RESET"\n"
         "################################################################################\n"
         "\n\n");
  fflush(stdout);


  /*Inicialização dos parametros para realizar o Socket*/
  memset(&server, 0, sizeof(server));
  server.ai_family   = AF_UNSPEC;    //Aceitar IPv4 ou v6
  server.ai_socktype = SOCK_STREAM;  //TCP
  server.ai_flags    = AI_PASSIVE;   //Localhost 

  /*Iniciando servidor--------------------------------------------------------*/
    //Inicializnado os parametros para a criação do socket
  if( (addrinfo_value = getaddrinfo(NULL, SERVPORT, &server, &p_serverinfo )) != 0 )
    { 
    m_error(M_FATAL_ERROR);
    }
  printf("# %d \t>> Iniciado parametros de criacao do socket\n", pross_count++);
  fflush(stdout);

    //Inicializa os parametros que serão usados no [int setsockopt()]
  m_prepar_sockopt( &sopt, SOL_SOCKET, SO_REUSEADDR, (int *)yes);
  printf("# %d \t>> Iniciando parametros de opicoes do socket\n", pross_count++);
  fflush(stdout);

    //Criando socket principal do servidor
  serverfd = openBind(p_serverinfo, &sopt);
  printf("# %d \t>> Servidor iniciado com sucesso\n", pross_count++);
  fflush(stdout);

    //Código de BJEE------------------------------------------------------------
    //Limpando processos zumbis
  siga.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&siga.sa_mask);
  siga.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &siga, NULL) == -1)
    {
    perror("sigaction");
    exit(EXIT_FAILURE);
    }//-------------------------------------------------------------------------
  printf("# %d \t>> Limpo os processos Zumbis\n", pross_count++);
  fflush(stdout);


    //Iniciando listen, preparando a fila de requisicoes # BACKLOG == 5
  if( (listen(serverfd, BACKLOG)) == -1 )
    {
    m_error( M_FATAL_ERROR );
    }
  printf("# %d \t>> Preparado backlog do socket : %d\n", pross_count++, serverfd);
  fflush(stdout);


  memset(game_data, 0, sizeof(STD_CONN) * 2);
  /*Looping do jogo-----------------------------------------------------------*/
  while(ply_num < 2)
    {
    printf("# %d \t>> Aguardando conexao do cliente atraves do socket : %d\n", pross_count++, serverfd);
    fflush(stdout);

      //Accept
    useraddr_size = sizeof(useraddr);
    accptfd[ply_num] = accept( serverfd, ( struct sockaddr *)&useraddr, &useraddr_size);
    if( accptfd[ply_num] == -1 )
      {
      m_msg_error("Falha na tentativa de comunicacao com cliente\n");
      m_error(M_STD_ERROR);
      continue;
      }
    
    inet_ntop( useraddr.ss_family, get_in_addr((struct sockaddr *) &useraddr ), s, sizeof(s));
    printf("# %d \t>> Conectado atraves do ip:"RED" %s" RESET "!!\n", pross_count++, s);
    fflush(stdout);

    //Parametros do jogadores
    game_data[ply_num].tid    = ply_num;
    game_data[ply_num].sockfd = accptfd[ply_num];
    game_data[ply_num].flag   = 0;
      
    ply_num++;
    }


  /*Preparando inicio da partida----------------------------------------------*/
  printf("\n\n# %d \t>> Enviando Parametros: "RED"ESPERA"RESET "!!\n", pross_count++ );
  fflush(stdout);
  msg_flag = WAIT_TO_START;
  while( ply_count < ply_num )
    {
    send_bytes = send(game_data[ply_count].sockfd, &msg_flag, sizeof(char), 0);
    if( send_bytes == -1)
      {
      m_msg_error("Erro no envio da mensagem do jogador: ");
      fprintf(stderr, RED"## %s !!\n"RESET, (ply_count == 0 ? "UM" : "DOIS") );
      continue;
      }
    printf("# %d \t>> Mensagem enviada com sucesso para jogador: "GREEN"## %s" RESET "!!\n",
           pross_count, (ply_count == 0 ? "UM" : "DOIS") );
    fflush(stdout);

    polling[ply_count].fd = game_data[ply_count].sockfd;
    polling[ply_count].events = POLLIN;

    printf("## %d \t>> Preparado parametros de: "GREEN"POLLING"RESET "!!\n", pross_count );
    fflush(stdout);

    ply_count++;
    }

  ply_count = 0;
  printf("\n\n# %d \t>> Enviando Parametros: "GREEN"INICIO"RESET "!!\n", pross_count++ );
  fflush(stdout);
  msg_flag = GAME_START;
  while( ply_count < ply_num )
    {
    send_bytes = send(game_data[ply_count].sockfd, &msg_flag, sizeof(char), 0);
    if( send_bytes == -1)
      {
      m_msg_error("Erro no envio da mensagem do jogador: ");
      fprintf(stderr, RED"## %s !!\n"RESET, (ply_count == 0 ? "UM" : "DOIS") );
      continue;
      }
    printf("## %d \t>> Mensagem enviada com sucesso para jogador: "GREEN"## %s"RESET"!!\n",
           pross_count, (ply_count == 0 ? "UM" : "DOIS") );
    fflush(stdout);

    ply_count++;
    }

  printf("\n\n# %d \t>> Aguardando jogadas\n", pross_count++ );
  fflush(stdout);
  /*Se os dois jogadores estiverem conectados corretamente, será realizado o */
  while(!(ply_flag[0] && ply_flag[1]))
    {
    poll_value = poll(polling, 2, 3000);
    if(poll_value == -1)
      {
      m_msg_error("Erro no recebimento do evento\n");
      }
    else if(poll_value == 0)
      {
      m_msg_error("Tempo de evento esgotado\n");
      }
    else
      {
      //Percorre cada evento do usuario=========================================
      ply_count = 0;
      while(ply_count < ply_num )
        {
        if((polling[ply_count].revents & POLLIN) && !ply_flag[ply_count] )
          {
          memset(&std_move, 0, sizeof(STD_MOVE));
          char c;
          recv_bytes = recv(game_data[ply_count].sockfd, &c, sizeof(char) , 0);
          if(recv_bytes == -1)
            {
            m_msg_error("Erro no recebimento da mensagem: ");
            fprintf(stderr, RED"## %s !!\n"RESET, (ply_count == 0 ? "UM" : "DOIS") );
            }   
          else
            {
            game_data[ply_count].attack.move = (int) c - '0';
            ply_flag[ply_count] = true; 

            printf("## %d \t>> Mensagem recebida com: "GREEN"SUCESSO"RESET" >>  usuario: %s !!\n",
                   pross_count , (ply_count == 0 ? "UM" : "DOIS") );
            fflush(stdout);

            printf("## %d \t>> >> Jogou :" GREEN " %s\n" RESET, pross_count, (c == '0' ? "PAPEL" :
                                                           (c == '1' ? "PEDRA" :"TESOURA")) );
 

            printf("&&&&&&&&&&&&&&&&&&&  MOVE: %d\n",game_data[ply_count].attack.move );

            fflush(stdout); 
            }
          }
        //Percorre cada evento de usuario=======================================
        ply_count++;
        }
      }
    }
  printf("# %d \t>> Realizadas todas as jogadas jogadas\n", pross_count++ );
  fflush(stdout);


  printf("\n\n# %d \t>> Calculando vencedor do "GREEN"JANKENPOO\n"RESET, pross_count++ );
  fflush(stdout);    
  if( ply_flag[0] && ply_flag[1] )
    {
    int result_final = (game_data[0].attack.move - game_data[1].attack.move);
    printf("\n\n\n RESULTADO FINAL : %d\n\n", result_final);

    if( game_data[0].attack.move == game_data[1].attack.move )
      {
      game_data[0].attack.rslt = game_data[1].attack.rslt = 2;
      ply_flag[0] = ply_flag[1] = false;
      }
    else if( (result_final == -1) || (result_final == 2) )
      {
      game_data[0].attack.rslt =(game_data[1].attack.rslt = 1) - 1;
      ply_flag[0] = ply_flag[1] = false;
      printf("\n\n\n P1 : %d\n P2 : %d\n", game_data[0].attack.rslt, game_data[1].attack.rslt);
      }
    else if( (result_final == 1) || (result_final == -2) )
      {
      game_data[0].attack.rslt =(game_data[1].attack.rslt = 0) + 1;
      ply_flag[0] = ply_flag[1] = false;
      }

    ply_count = 0;
    while(ply_count < ply_num)
      {
      memset(&std_move, 0, sizeof(STD_MOVE));
      std_move = game_data[ply_count].attack;
      send_bytes = send( game_data[ply_count].sockfd, &std_move, sizeof(STD_MOVE), 0);
      if( send_bytes == -1 )
        {
        m_msg_error("Erro no envio da mensagem para o player 1\n");
        continue;
        }
      printf("## %d \t>> Mensagem enviada com: "GREEN"SUCESSO"RESET" >>  usuario: %s !!\n",
             pross_count , (ply_count == 0 ? "UM" : "DOIS") );
      fflush(stdout);
      ply_count++;
      }
    } 

  printf("################ %d \t>> "RED"JOGO ENCERRADO\n"RESET, pross_count++ );
  fflush(stdout);
    
  return 0;
}

