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
 *   Este arquivo contem as funçoes de inicializacao do jogo. Tudo que  
 *  precisar ser inicializado ESTATICAMENTE devera ser linkado aqui.
 *  
 */

#include "../m_stdlib/stderror_libs/m_stderror.h"

#define FULLSCREEN 'y'
#define NO_FULLSCREEN 'n'

struct game_conf
{
  int8_t  fullscreen;   //'y' = fullscreen , 'n' or 0x00 = no fullscreen
  int32_t width;
  int32_t height;
} M_GAME_CONF;

enum obj
{
  M_LEFT,
  M_RIGHT,
  M_ESQ,
  M_ENTER 
} m_obj;

enum scene
{
  M_START,
  M_LOADING,
  M_GAME
} m_game;

const float
  FPS = 60;

const int
  MIN_SCREEN_WIDTH  = 640,
  MIN_SCREEN_HEIGHT = 480,
  BOUNCER_SIZE      = 32;

struct game_conf
  game_configuration;

static ALLEGRO_DISPLAY
 *display     = NULL; // parametros de display da tela

static ALLEGRO_TIMER      
 *timer       = NULL;

static ALLEGRO_EVENT_QUEUE 
 *event_queue = NULL;

//Pega as configuracoes do sistema

void
getConf(void)
{
}

//Settas as configuracoes do sistema
int  
setConf( struct game_conf conf )
{
  return 0;
}

//Setta configuracoes iniciais do jogo atraves de um arquivo de .conf
int 
readeConf(void)
{
  return 0;
}

int
writeConf(void)
{
  return 0;
} 



/*------------------------Funcao de Fade para background----------------------*/
  /****
   *  Funcao: static bool m_background_fadeout()
   *
   *  Parametros:
   *    ALLEGRO_BITMAP * image >> Objeto Allegro para imagens
   *    const int speed        >> Velocidade do efeito de opacidade
   *    int alfa               >> Valor da mascara de alfa 0 ~ 255
   *
   *  Descricacao: Realiza o efeito de transformar o background
   *               mais opaco ate desaparecer.
   *
   *  Obs: O alfa proporciona apenas intervalo entre branco e preto
   *       nao sendo possivel sobrepor uma imagens.
   *       Caso o obejeto nao esteja em fadein a trasicao sera brusca
   *
   ****/  
static bool m_background_fadeout( ALLEGRO_BITMAP *image, const int speed, int alfa ) 
{
  int channel; //Variavel dos canais de cores, nao manipulavel externamente

    /*Menor que 0 ocorreria erro, porem maior que 20 torna a transicao brusca*/
  if( (speed <= 0) || (speed > 20	) )
    {
    m_msg_error("Fade-out background recebeu valor invalido para speed");
    return false;
    }
    /*Se valor de alfa varia muito influenciara no alfa do al_map_rgba que e do
      tipo char podendo afetar no resulado*/
  if( (alfa < 0) || (alfa > 255) )
    {
    m_msg_error("Fade-out background recebeu valor invalido para alfa");
    return false;
    }
    /*Eleva o nivel dos canais proporcional a velocidade*/
  for( channel = 0 ; channel < 256 ; channel += speed )
    {
      //Setta as core da frente que vao sobropor a imagem
    al_clear_to_color(al_map_rgba(channel, channel, channel, channel));
      //Redesenha o objeto gradativamente com mais opacidade
    al_draw_tinted_bitmap
      (
      image,
      al_map_rgba(255 - channel, 255 - channel, 255 - channel, 255 - alfa ),
      0, 0, 0
      );
      //Atualiza o display
    al_flip_display();
    }
  /*Caso tudo ocorra bem ja caira aqui*/
  return true;
}

  /****
   *  Funcao: static bool m_background_fadein()
   *
   *  Parametros:
   *    ALLEGRO_BITMAP * image >> Objeto Allegro para imagens
   *    const int speed        >> Velocidade do efeito de opacidade
   *    int alfa               >> Valor da mascara de alfa 0 ~ 255
   *
   *  Descricacao: Realiza o efeito de transformar o background
   *               menos opaco caso o mesmo já esteja.
   *
   *  Obs: O alfa proporciona apenas intervalo entre branco e preto
   *       nao sendo possivel sobrepor uma imagens. 
   *
   *
   ****/ 
static bool m_background_fadein( ALLEGRO_BITMAP *image, const int speed, int alfa )
{
  int channel;
 
    /*Menor que 0 ocorreria erro, porem maior que 20 torna a transicao brusca*/
  if( (speed <= 0) || (speed > 20) )
    {
    m_msg_error("Fade-in background recebeu valor invalido para speed");
    return false;
    }
    /*Se valor de alfa varia muito influenciara no alfa do al_map_rgba que e do
      tipo char podendo afetar no resulado*/
  if( (alfa < 0) || (alfa > 255) )
    {
    m_msg_error("Fade-in background recebeu valor invalido para alfa");
    return false;
    }
    /*Eleva o nivel dos canais proporcional a velocidade*/
  for( channel = 0 ; channel < 256 ; channel += speed )
    {
      //Seta uma cor para o objeto, a frente da imagem com uso do alfa
    al_clear_to_color(al_map_rgba(alfa, alfa, alfa, 255 - channel));
      //Redesenha o objeto seguindo um canal de alfa
    al_draw_tinted_bitmap
      (
      image,
      al_map_rgba(channel, channel, channel, channel ),
      0, 0, 0
      );
      //Atualiza o display
    al_flip_display();
    }  
  return true;
}









/*--------------------Inicialização dos parametros do jogo--------------------*/
int initOpt(void)
{
  //Variaveis e estruturas de inicialização ====================================

  static ALLEGRO_DISPLAY_MODE 
    display_data;

  static ALLEGRO_BITMAP     
   *presentation[3] = { 
                      NULL, //Logo do studio Makoto
                      NULL, //Logo do IFMS
                      };

  // Inicializacao dos componentes Allegro =====================================
  /* Inicializacao do dos parametros da allegro */
  if( !al_init() )
    {
    m_msg_error("Falha na inicializacao dos parametros Allegro\n");
    return -1;
    }

  
  /* Instacao dos componetes de mouse via allegro*/
  if( !al_install_mouse() )
    {
    m_msg_error("Falha na instalacao dos componentes para uso do mouse\n");
    return -1;
    }

  if( !al_install_keyboard() )
    {
    m_msg_error("Falha na instalacao dos componentes de teclado\n");
    }
  else
    {
    m_msg_error((al_is_keyboard_installed()) == true ?
      "Teclado instalado com sucesso\n" : "Pode ter acontecido falhas na instalacao\n"); 
    }


  /* Inicializacao dos componetes de imagem*/
  if( !(al_init_image_addon()) )
    {
    m_msg_error("Falha na inicializacao dos parametros de imagem\n");
    return -1;
    }

  if( !al_init_primitives_addon() )
    {
    m_msg_error("Falha na inicializacao dos parametros de imagens primitivas\n");
    return -1;
    } 
  
  /* Inicializacao dos componetes de fontes Allegro*/
  al_init_font_addon();
  /* Inicializacao dos componenentes para uso de True Type Font*/
  if( !(al_init_ttf_addon()) )
    {
    m_msg_error("Falha na inicializacao dos componentes de TTF\n");
    return -1;
    }

  al_set_window_title(display, "JO-GUEN-PO");
  
  //Criacao dos objetos Allegro ================================================ 
  /*Cria o cronometro mas nao inicializa*/
  if(!( timer = al_create_timer( 1.0 / FPS ) ))
    {
    m_msg_error("Falha na criacao do cronometro\n");
    return -1;
    }




  /*Settado o primeiro parametro como zero, buscara a maior resolucao do compu-
    tador do usuario, armazenando no segundo campo como ALLEGRO_DISPLAY_MODE*/
  if(!al_get_display_mode( 0, &display_data))
    {
    m_msg_error("Falha ao encontrar parametros de fullscreen do usuario\n");
    return -1;
    }
  /*Verifica se o usuario iniciou fullscreen, se sim, setta fullscreen e cria o 
    novo display graficamente*/
  if( game_configuration.fullscreen == FULLSCREEN )
    {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN);
    display = al_create_display( game_configuration.width,
                                 game_configuration.height );
    if(!display )
      {
      m_msg_error("Falha na criacao de um display fullscreen\n");
      return -1;
      }
    }
  else
    {
    display = al_create_display( /*display_data.width*/   800, 
                                 /*display_data.height)*/ 600);
    if( !display )
      {
      m_msg_error("Falha na criacao do display\n");
      return -1;
      }
    }


  //Carregado e settando os objetos criados ====================================
  /* Carrega as opcoes de imagem da Allegro a partir de um caminho*/
  if( !(presentation[0] = al_load_bitmap("assets/credits/makoto_logo.jpg")) )
    {
    m_msg_error("Falha no carregamento da imagem");
    return -1;
    }

  if( !(presentation[1] = al_load_bitmap("assets/credits/ifms_logo.jpg")) )
    {
    m_msg_error("Falha no carregamento da imagem\n");
    return -1;
    }

  /*Cria uma fila de eventos tratados pela propria
    allegro, evitando ter que gerar handlers proprios*/
  if( !(event_queue = al_create_event_queue()) )
    {
    m_msg_error("Falha na criacao dos eventos");
    al_destroy_display(display);  // destroy_obj_display = 2
    al_destroy_timer(timer);      // destroy_obj_timer   = 1 
    return -1;
    }


   m_background_fadein( presentation[0], 2, 43);
   //al_rest(2.0);  
   m_background_fadeout(presentation[0], 5, 43);
   //al_rest(2.0); 
   m_background_fadein( presentation[1], 2, 43);
   //al_rest(2.0);  
   m_background_fadeout(presentation[1], 5, 255);
   //al_rest(2.0); 


  al_register_event_source(event_queue, al_get_display_event_source(display)); 
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_mouse_event_source());
  al_register_event_source(event_queue, al_get_keyboard_event_source());

  //SETTA todos os objetos em um getter
  //setAllegroDisplay( display );
  //setAllegroEventQueue( event_queue );
  //setAllegroTimer( timer );

  return 0;
}

