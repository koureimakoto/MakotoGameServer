#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SERVPORT "12345"
#define B_SIZE 255
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

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

int main(void)
{
   struct addrinfo client,
                  *server,
                  *server_p;

   int sockfd,
       ai_value;

   size_t send_bytes,
          recv_bytes;

   char s[INET6_ADDRSTRLEN],
        buffer[B_SIZE],
        msg[B_SIZE] = "PROCESSO CLIENTE PARABENIZA A TURMA DO FLISOL";

   memset(&client, 0, sizeof(client));
   client.ai_family   = AF_UNSPEC;
   client.ai_socktype = SOCK_STREAM;

   if(( ai_value = getaddrinfo("127.0.0.1", SERVPORT, &client, &server )) != 0 )
   {
      perror("Getaddrinfo error");
      exit(EXIT_FAILURE);
   }

   for( server_p = server; server_p != NULL; server_p->ai_next )
   {
      sockfd = socket(server_p->ai_family, server_p->ai_socktype, server_p->ai_protocol);
      if( sockfd == -1 )
      {
         perror("Socket error");
         continue;
      }
      if( connect(sockfd, server_p->ai_addr, server_p->ai_addrlen) == -1 )
      {
         perror("Connect error");
         continue;
      }
      break;
   }   
   

   inet_ntop( server_p->ai_family,
              get_in_addr((struct sockaddr *)server_p->ai_addr),
              s, sizeof(s)
             );

   printf("Cliente conectou-se atraves do endereco " RED " %s\n" RESET, s);

   freeaddrinfo(server);

   if((recv_bytes = recv( sockfd,  buffer, sizeof(buffer) -1, 0)) == -1)
   {
      perror("Nao foi possivel receber com sucesso a mensagem do servidor com sucesso");
      exit(1);
   }
   
   printf("Resposta enviada com" GREEN " SUCESSO\n" RESET);
  
   int x = 0;
   printf("Agora vou dormir um pouco para que o servidor processo bem minhas mensagens antes de encerrar\n");
   while( 7 > (x++))
   {
       sleep(1);
       fflush(stdout);
       printf(".");
   }

   printf("Pronto, acordei e vou enviar a mensagens final para que outros clientes possam entrar\n");
  
   printf("%s\n", buffer);

   if( (send_bytes = send( sockfd , &msg, sizeof(msg), 0)) == -1 )
   {
      perror( RED "Nao foi possivel enviar a mensagem final ao cliente" RESET );
      exit(1);
   }

   printf("Programa encerrando com" GREEN " SUCESSO\n" RESET );

   return(0);
}
