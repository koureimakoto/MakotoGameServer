/* VERSION: PORTUGUESE
 *                                    ____
 *       ____    ____                /___ \ 
 *      |    |  /    |                 _/_/  
 *      |    | /    /                  \_\   
 *      |    |/    /           
 *      |         /            
 *      |        /             
 *      |        \        _____     _      _   _____    _____   __
 *      |         \      /  _  \   | |    | | |  _  \  |  ___| |  |
 *      |    |\    \    /  / \  \  | |    | | | | |  | | |___  |  |
 *      |    | \    \  |  |   |  | | |    | | | |_| _| |  ___| |  |
 *      |    |  \    \  \  \_/  /  |  \__/  | |  _  \  | |___  |  |
 *      |____|   \____|  \_____/    \______/  |_| \__| |_____| |__|
 *
 *
 *  =====================================================================
 *
 *  AUTOR: Talles Henrique Brolezzi Fagundes
 *  
 *  *
 *   Segue abaixo a versao inicial do nucleo do jogo. Foi decido chamar de 
 *  motor ( engine ), mesmo nao sendo exatamente um, mas tendo um propasi-
 *  to de ser com um controlador.
 *
 *   Nele contera as bibliotecas iniciais da linguagem C e da biblioteca 
 *  allegro, mas quando se tratar de bibliotecas do sistema operacional, 
 *  isto e, se e Linux, Windows, ou macOS, ficarao contidas dentro de ca-
 *  codico fonte especifico.
 *
 *   Exemplo: Threads e Signal 
 *   São expecificacoes muito peculiares de cada sistema operacional entao
 *  decidi manter os header de cada uma em seu proprorio corpo.
 *
 * 
 *  Sobre a Biblioteca Allegro:
 *   Esta biblioteca foi utilizada para realização do projeto, mas os co- 
 *  digos contidos nela nao sao de minha autoria,apenas a utilizacao dele;
 *  e toda sua documentacao pode ser encontrada no site da liballegro.
 *
 *  SITE ALLEGRO    : http://liballeg.org/
 *  LICENÇA ALLEGRO : http://liballeg.org/license.html
 */

//Standard C lib
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Allegro lib
#include <allegro5/allegro.h>
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_primitives.h>
#include "allegro5/allegro_native_dialog.h"


//Makoto Game lib
#include "m_game_initialization.h"
#include "m_game_main.c"
//#include "m_game_close.h"

//Makoto Client and Error lib
//#include "network_lib/m_client.h"
//#include "cliente_error_lib/m_game_error.h"

/*Main*/
int main()
{
  initOpt(); 
  gameLoop();
  //gameClose();

  return 0;
}
