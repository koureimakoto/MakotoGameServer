#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <netdb.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SERVPORT "22222"
#define B_SIZE 255
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

#define P_WARN -1
#define P_WAIT 2
#define P_FAIL 3
#define P_CORR 4
#define P_WIN  5
#define P_LOS  6
#define P1 0
#define P2 1

#define M_MAX_THREADS 2
#define B_SIZE 255

typedef struct mat_data {
   float value[M_MAX_THREADS];//Valores do jogo
   float rslt;    //Resultado
} M_DATA_T;

typedef struct mat_players {
   int     plynum;
   int     game_flag;
   int32_t game_flag_conv;
   char   *game_flag_char;
} M_PLAYER_T;

M_PLAYER_T player;

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

int 
main(void)
{
   //VARIAVEIS INICIAIS
   struct addrinfo  client;
   struct addrinfo *server;
   struct addrinfo *server_p;                
   struct pollfd    poll_fd[2];
   M_DATA_T         game;

   int     sockfd;
   int     ai_value;
   int     poll_value;
   int     count;
   size_t  send_bytes;
   size_t  recv_bytes;
   int32_t sinal;

   char s[INET6_ADDRSTRLEN];
   char buffer[B_SIZE];

   //Preparando socket
   memset(&client, 0, sizeof(client));
   client.ai_family   = AF_UNSPEC;
   client.ai_socktype = SOCK_STREAM;

   if(( ai_value = getaddrinfo("127.0.0.1", SERVPORT, &client, &server )) != 0 )
     {
     perror("Getaddrinfo error");
     exit(EXIT_FAILURE);
     }

   //Iniciando o socket e requisitando connect
   for( server_p = server ; server_p != NULL; server_p->ai_next )
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
   
   inet_ntop( 
     server_p->ai_family,
     get_in_addr((struct sockaddr *)server_p->ai_addr),
     s,
     sizeof(s)
            );

   freeaddrinfo(server);
   /*--Abertura do socket com sucesso, e conexão realizada--*/

   //Recebe a primeira mensagem de inicio
   if( (recv_bytes = recv( sockfd,  &buffer, B_SIZE -1, 0)) == -1)
     {
     perror("Nao foi possivel receber com sucesso a mensagem do servidor com sucesso");
     exit(1);
     }
   else
     {
     buffer[B_SIZE] = 0x00;
     printf("Mensagem do servidor: " GREEN "%s\n" RESET, buffer);
     fflush(stdout);
     setbuf(stdin, NULL);
     }
   
   //Preparando a estrutura para receber polling de poll
   memset( &poll_fd, 0, sizeof(struct pollfd) );
   poll_fd[0].fd = sockfd;
   poll_fd[0].events = POLLIN;



   while("happy")
     {
     //Prepara o poll
     poll_value = poll( poll_fd, 2, 2000); 
       

     printf("fd : %d - ev : %hd - rev : %hd\n", poll_fd[0].fd, poll_fd[0].events, poll_fd[0].revents);

     printf("CONTADOR POLLING: %d - POLLING VALUE: %d\n", poll_value);

     if( poll_value == -1 )
       {
       perror("Erro na poll\n");
       exit(EXIT_FAILURE);
       } 
     else if( poll_value == 0 )
       {
       perror("Tempo Esgotado, reiniciando cronometro");
       continue;
       }
     else
       {
       if( poll_fd[0].revents & POLLIN )
         {
         //bzero( &player.game_flag_conv, 4);
         if((recv_bytes = recv( sockfd, &player.game_flag_conv, sizeof(player.game_flag_conv), 0)) == -1 )
           {
           printf("Erro ao tentar receber SINAL do servidor\n");
           exit(1);
           }
         else
           {
           memcpy( &player.game_flag_conv, &player.game_flag_char, sizeof(player.game_flag_conv));
           sinal = ntohl(player.game_flag_conv);
           printf("SINAL: %ld - CONV: %ld - BYTES: %d\n ", sinal, player.game_flag_conv, recv_bytes);
           }
          if( sinal == P_WARN )
           {
           printf("\nJOGO SERA ENCERRADO");
           exit(0);
           }
         else if( sinal == P_WAIT)
           {
           printf(RED"\nAguardando outro jogador\n"RESET);
           }
         else
           {
           printf("Voce e o jogador:%d \n", sinal);
           break;
           }
         }         
       }
     }

   printf("\n\n\nPartida esta sendo inciada\n");
   sleep(3);

   while( count < 3 )
   {
      if( sinal == P2 )
      {   
         printf("Adversario preparando pergunta\n\n");
         
         if((recv_bytes = recv( sockfd, &game , sizeof(M_DATA_T), 0)) == -1 )
         {
            printf("Erro ao tentar receber SINAL do servidor\n");
            exit(1);
         }
         const float rslt = game.rslt;
         const float resp;
         
         printf("Seu adversario perguntou, quanto e %.2f + %.2f ??\n", game.value[0], game.value[1]);
         scanf("%f", &resp);
         
         if( resp == rslt )
            printf("Você acertou\n");
         else
            printf("Que pena, voce errou\n");
          
 
         sinal = P_FAIL; 
         if( (send_bytes = send( sockfd, &sinal, sizeof(int) , 0 ) ) == -1 )
         {
            printf("Erro ao tentar enviar SINAL ao servidor\n");
            exit(1);
         }
         sinal = P1;
         memset(&game, 0, sizeof(M_DATA_T));
      }   
      if( sinal == P1 )
      {
         printf("Faça sua jogada, escolha dois valores para o seu adversario tentar somar\n");
         scanf("%d", &game.value[0]);
         scanf("%d", &game.value[1]);
         
         if( (send_bytes = send( sockfd, &game, sizeof(M_DATA_T) , 0 ) ) == -1 )
         {
            printf("Erro ao tentar enviar a jogada do servidor\n");
            exit(1);
         }
         printf("Enviado com sucesso\n");
         printf("Aguardando resposta do adversario\n");
         
         if((recv_bytes = recv( sockfd, &sinal , sizeof(int), 0)) == -1 )
         {
            printf("Erro ao tentar receber SINAL do servidor\n");
            exit(1);
         }
         if( sinal == P_FAIL )
            printf("Seu oponente errou\n");
         else if( sinal == P_CORR )
            printf("Seu oponente acertou\n");
            
         sinal == P2;
         memset(&game, 0, sizeof(M_DATA_T));
      }
      count++;
   }
   

   return(0);
}
