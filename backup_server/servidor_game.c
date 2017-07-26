#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

//m_error.h_____________________________________________________________________
#define M_ERR_EXIT 0
#define M_ERR_LOG 1
void m_error_log();
void m_error_fatal();
//game_tcplib.h_________________________________________________________________
#define M_ACCP_MAX 128
#define M_ACCP_DFN 20
int m_socket( int domain, int type, int protocol, short error_type );
int m_setsockopt( int sock_fd, int lvl, int opt_name, const void * opt_value, socklen_t opt_len, short error_type );
int m_bind( int sock_fd, const struct sockaddr_in *addr, socklen_t addrlen, short error_type );
int m_listen( int sock_fd, int backlog, short error_type );
int m_accept( int sock_fd, struct sockaddr_in *addr, socklen_t *addrlen, short error_type );
struct sockaddr_in set_addrsock_in( short family, unsigned short port, unsigned long s_addr );



int
main(void)
{
  //Variable
  int sock_fd,  //File Descriptor do Socket criado 
      sock_opt, //Retorno de erro do Socket Options
      bind_rt,  //Retorno de erro do Bind
      lstn_rt,  //Retorno de erro do Listen
      accp_fd[M_ACCP_DFN - 1],  //File Descriptor do Accept gerado
      on = 1 ,  // 1 = on - 0 = off
      i  = 0,
      size_s;
  struct sockaddr_in server,     //estrutura do servidor
                    *server_ptr, //ponteiro da estrutura
                     client[2];
                     server_ptr = &server;
 

  char buffer[256] = "Teste de ";

  //criando socket TCP----------------------------------------------------------
  sock_fd = m_socket( AF_INET, SOCK_STREAM, 0, M_ERR_EXIT );
  //setando os dependencias da comunicação--------------------------------------
  server  = set_addrsock_in( AF_INET, htons(12345), INADDR_ANY ); 
  ////setando opções de socket para reuso das opções à cima-----------------------
  sock_opt= m_setsockopt( sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on), M_ERR_EXIT );
  //realizado o bind com o socket tcp
  bind_rt = m_bind( sock_fd, &server , sizeof(server), M_ERR_EXIT );
  //setta os parametros de listen do servidor para o socket tcp
  lstn_rt = m_listen( sock_fd, M_ACCP_DFN, M_ERR_EXIT);

  /***************************************************/
  /* Apesar de listen receber um número x de requisi-*/
  /*coes na pilha, */
  
  //While que recebe as requisões, sequindo os valores do LISTEN
  while( i < M_ACCP_DFN +1 )
  {
    size_s = sizeof(client[i]);
    accp_fd[i] = m_accept( sock_fd, &client[i], &size_s, M_ERR_EXIT );
    printf("%d -  %s\n", accp_fd[i]);
    memset(&buffer, 0, sizeof(buffer));
    write( accp_fd[i], &buffer, sizeof(buffer+1));
    i++;
  }





  printf("socket = %ld\n", sock_fd);
  printf("addr   = family %d, port %d, s_addr %ld\n",
         server.sin_family, server.sin_port, server.sin_addr.s_addr );
  printf("bind   = %d\n", bind_rt);
  printf("listne = %d\n", lstn_rt);


  close(sock_fd);
  return 0;
}








//FUNCTION m_error.h============================================================
void m_error_fatal()
{
  fprintf( stderr, "Critical Error: %d Type: %s\n", errno, strerror(errno));
  exit(EXIT_FAILURE); // Pode ser return(-1) caso não haja a função, mas  
}

void m_error_log()
{

}
//FUNCTION game_tcplib.h========================================================

//M_SOCKET()--------------------------------------------------------------------
/*
 *Esta função é semelhante ao socket(), apenas já realiza as verificações de er-
 *rros para retornar dar seguimento no programa. Por este motivo leva o parame-
 *tro <int error_type>, serve para sinalizar qual forma de erro quer trabalhar.
 */
int m_socket( int domain, int type, int protocol, short error_type )
{
  //Variable
  int socket_file_descriptor; //Get socket file descriptor from socket()

  //Operations
  socket_file_descriptor = socket( domain, type, protocol );
  if( socket_file_descriptor == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
  else
    return( socket_file_descriptor );	
}

//M_SETSOCKETOPT()--------------------------------------------------------------
/*
 *Esta função, setsockopt(), tem o poder de settar opções de utilização do sock
 *utilizado. Podendo ser usado parametros como SO_REUSEADDR que permite o reuso 
 *das especificações de socket que já venham a ter sido iniciadas fazendo voltar
 *escutar mesmo que já tenha sido dado um TIME_WAIT no processo do socket.
*/
int m_setsockopt( int sock_fd, int lvl, int opt_name, const void * opt_value, socklen_t opt_len, short error_type )
{
  //Variable
  int sock_option;

  //Operations
  sock_option = setsockopt( sock_fd, lvl, opt_name, opt_value, opt_len );
  if( sock_option == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
  else	
    return( sock_option );
}


//M_BIND()----------------------------------------------------------------------
/*
 *Esta função leva a lógica semelhante do socket(), busca fazer a verificação de
 *erro dentro da função, esta é uma versão semelhante a bind() da <sys/socket.h>.
 */
int m_bind( int sock_fd, const struct sockaddr_in *addr, socklen_t addrlen, short error_type )
{
  //Variable
  int bind_return;

  //Operations
  bind_return = bind( sock_fd, ( struct sockaddr *) addr, addrlen);
  if( bind_return == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
  else	
    return( bind_return );
}

int m_listen( int sock_fd, int backlog, short error_type )
{
  //Variable
  int listen_return;

  //Operations
  listen_return = listen( sock_fd, backlog );
  if( listen_return == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
  else
    return( listen_return );
}

int m_accept( int sock_fd, struct sockaddr_in *addr, socklen_t *addrlen, short error_type )
{
  int accept_file_descriptor;

  accept_file_descriptor = accept( sock_fd, ( struct sockaddr *) addr, addrlen);
  if( accept_file_descriptor == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
}
/*
 *Função com o papel de popular mais facilmente a estrutura de sockaddr_in
*/
struct sockaddr_in set_addrsock_in( short family, unsigned short port, unsigned long s_addr )
{
  struct sockaddr_in addr;
  memset( &addr, 0, sizeof(addr) );
  addr.sin_family      = family;
  addr.sin_port        = port;
  addr.sin_addr.s_addr = s_addr;
  return( addr );
}


