#define BACKLOG 5
#define SERVPORT "22222"
#define WAIT_TO_START 'w'
#define GAME_START 'g'

#define CLIENT_TURN 0
#define CLIENT_WAIT 1
#define CLIENT_RESULT 2

#define M_WIN 0
#define M_LOSE 1
#define M_DRAW 2

#define M_PAPER 0
#define M_ROCK 1
#define M_SCIZOR 2

typedef struct move
{
  int8_t  id;
  int16_t flag;
  int16_t rslt;
  int32_t move;
} STD_MOVE;

typedef struct connection_std
{
  int16_t  flag;
  int32_t  tid;
  int32_t  sockfd;
  STD_MOVE attack;
} STD_CONN; 




/*Struct para uso com [m_prepar_socketopt()]*/
typedef struct sockopt
{
  int sockfd;
  int level;
  int optname;
  socklen_t optlen;
  void *optval;   
} m_sockopt;



char * structToString(STD_MOVE *s)
{
  char *c = (char *) malloc(sizeof(STD_MOVE));
  c = (char *) s;
  return c;
}




/****
 *  Funcao: int m_prepar_socketopt()
 *
 *  Parametros:
 *    int sockfd       >> Socket File Descriptor 
 *    int level        >> Secket level
 *    int optname      >> Socket flag option name
 *    void *optval     >> (CAST) Socket Option Value
 *
 *  Descricao:
 *    Realiza a funcao de settar os parametros de socket de uma forma
 *    mais limpa.
 * 
 *  Observacao:
 *    Nao realizado todas as baterias de testes necessarias e nem todas
 *    as opcoes de parametros de socket, podendo obter algum comporta-
 *    mento implicito principalmente nao (void * optval) se nao reali-
 *    do o CAST de forma devida.
 */
int
m_prepar_sockopt( m_sockopt *opt, int level, int optname, void *optval)
{
  //Limpando a memoria da struct sockopt
  memset(opt, 0, sizeof(struct sockopt));   

  if(opt == NULL)
    {
    printf("Erro de memória em prepar_sockopt\n");
    return(-1);
    } 

  //Socket opt apenas setta com valor positivel
  if( (level >= 0) && (optname >= 0) )
    {
    opt->level  = level;           
    opt->optname= optname;         
    opt->optlen = sizeof(optval);    

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
  return(0);
}
