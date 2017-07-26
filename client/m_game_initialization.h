/* VERSION: PORTUGUESE
 *                         ____
 *                        /___ \ 
 *                          _/_/  STUDIO MAKOTO 
 *                         \_\   
 *
 *  By: Talles Henrique Brolezzi Fagundes
 *
 *  =====================================================================
 *
 *   Este arquivo contem os protótipos das funções de inicialização do  
 *  jogo. Tudo que precisar ser inicializado ESTATICAMENTE deverá ser
 *  linkado aqui.
 *
 */

#include "m_game_initialization.c"


//Pega as configurações do sistema
void getConf(void);

 //Settas as configurações do sistema
int  setConf( struct game_conf conf );

//Setta configurações iniciais do jogo através de um arquivo de .conf
int  readeConf(void);

//Escreve no arquivo as opções definas pelo usuário dentro da partida
int  writeConf(void);

//Funcao para transicao de fade-out
static bool m_background_fadeout( ALLEGRO_BITMAP *image, const int speed, int alfa ); 

//Funcao para transicao de fade-in
static bool m_background_fadein( ALLEGRO_BITMAP *image, const int speed, int alfa ); 


//Inicialização dos parametros do jogo
int initOpt(void);
