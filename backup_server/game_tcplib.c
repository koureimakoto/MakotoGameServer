#include <sys/socket.h>
#include "m_error.h"


/*
 *Esta função é semelhante ao socket(), apenas já realiza as verificações de er-
 *rros para retornar dar seguimento no programa. Por este motivo leva o parame-
 *tro <int error_type> que serve para sinalizar qual forma de erro quer trabalhar.
 */
int socket_tcp_m( int domain, int type, int protocol, int error_type )
{
  //Variable
  int socket_file_descriptor; //Get socket file descriptor from socket()

  //Operations
  socket_file_descriptor = socket( domain, type, protocol )
  if( socket_file_descriptor == -1)
  {
    if( error_type == M_ERR_EXIT)
      critical_error_exit();
    if( error_type == M_ERR_LOG )
      critical_error_log();
  }
  else
    return( socket_file_descriptor );	
}
/*
 *Esta função leva a lógica semelhante do socket(), busca fazer a verificação de
 *erro dentro da função, esta é uma versão semelhante a bind() da <sys/socket.h>.
 */
int bind_tcp_m( int sock_fd, const struct sockaddr *addr, socklen_t addrlen, int error_type )
{
  //Variable
  int bind_return;

  //Operations
  bind_return = bind( sock_fd, ( struct sockaddr *) addr, addrlen);
  {
    if( error_type == M_ERR_EXIT)
      critical_error_exit();
    if( error_type == M_ERR_LOG )
      critical_error_log();
  }
  else	
    return( bind_return );
}
