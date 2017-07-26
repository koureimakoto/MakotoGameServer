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
#define M_ACCP_DFN 2
int m_socket( int domain, int type, int protocol, short error_type );
int m_setsockopt( int sock_fd, int lvl, int opt_name, const void * opt_value, socklen_t opt_len, short error_type );
int m_bind( int sock_fd, const struct sockaddr_in *addr, socklen_t addrlen, short error_type );
int m_listen( int sock_fd, int backlog, short error_type );
int m_accept( int sock_fd, struct sockaddr_in *addr, socklen_t *addrlen, short error_type );
int m_connect( int sock_fd, const struct sockaddr_in *addr, socklen_t addrlen, short error_type );

struct sockaddr_in set_addrsock_in( short family, unsigned short port, unsigned long s_addr );



int
main(void)
{
  //Variable
  int sock_fd,  //File Descriptor do Socket criado 
      sock_opt, //Retorno de erro do Socket Options
      on = 1,
      size_s;
  struct sockaddr_in host,     //estrutura do servidor
                    *host_ptr; //ponteiro da estrutura
                     host_ptr = &host;

  char buffer[256];

  //criando socket TCP----------------------------------------------------------
  sock_fd = m_socket( AF_INET, SOCK_STREAM, 0, M_ERR_EXIT );
  //setando os dependencias da comunicação--------------------------------------
  host = set_addrsock_in( AF_INET, htons(12345), inet_addr("127.0.0.1") ); size_s = sizeof(host);
  //setando opções de socket para reuso das opções à cima-----------------------
  m_connect( sock_fd, host_ptr , size_s, M_ERR_EXIT);

  memset( buffer, 0, sizeof(buffer));
  read( sock_fd, &buffer, sizeof(buffer+1));

  printf("socket = %ld\n - %d", sock_fd, getpid());
  printf("addr   = family %d, port %d, s_addr %ld\n",
         host.sin_family, host.sin_port, host.sin_addr.s_addr );
  printf("%s\noi", buffer);

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

int m_connect( int sock_fd, const struct sockaddr_in *addr, socklen_t addrlen, short error_type )
{
  //Variable
  int connect_return;

  //Operations
  connect_return = connect( sock_fd, ( struct sockaddr *) addr, addrlen);
  if( connect_return == -1 )
  {
    if( error_type == M_ERR_EXIT)
      m_error_fatal();
    if( error_type == M_ERR_LOG )
      m_error_log();
  }
  else	
    return( connect_return );
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


