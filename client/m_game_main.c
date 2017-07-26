#include "scenes/loading/m_loading.c"
#include "network_lib/m_client.c"
#include "scenes/interface/interface.c"
#include "scenes/game/jankenpoo.c"

void gameLoop()
{
  ALLEGRO_COLOR
    hand_title_color = al_map_rgb(150,150,150);

  ALLEGRO_FONT
   *font        = NULL;

  ALLEGRO_THREAD
   *polling_thread;

  CONN_CLIENT
    conn_data;

  bool
    done     =  false,
    turn     =  true,
    redraw   =  true,
    m_scene[]= {true, false, false},
    m_key[]  = {false, false, false, false};

  int32_t 
    direction =  0;

  float
    alpha = 0.0,
    r = 1.0, g = 1.0, b = 1.0;

  printf("\n\n# %d \t>> Iniciando componentes dinamicos\n", pross_count++ );
  fflush(stdout);


  /*Carregando componentes de fonte de inicio*/
  if( !(font = al_load_font("assets/fonts/beefd.ttf", 20, 0) ) )
    {   
    m_msg_error("Falha no carregamento da fonte\n");
    al_destroy_display(display);
    al_destroy_timer(timer);
    exit(EXIT_FAILURE);
    }
  printf("# %d \t>> Carregado fonte "GREEN"Monoglyceride\n"RESET, pross_count++ );
  fflush(stdout);

  printf("# %d \t>> Iniciando "RED"GAME_LOOPING\n"RESET, pross_count++ );
  fflush(stdout);
  // Inicio do loop principal ==================================================
  al_start_timer(timer);  
  while( done == false ) 
    {
    /* Verifica se ha algum evento na FILA de eventos*/
    while(!al_is_event_queue_empty(event_queue))
      {
      ALLEGRO_EVENT event;
      al_wait_for_event(event_queue, &event);
     
      /*Capitura de eventos do de teclado por tempo*/
      if(event.type == ALLEGRO_EVENT_TIMER)//-----------------------------------
        {
        redraw = true;
        }
      else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
        // Se qualquer teclada for precionada dentro do evento
        switch(event.keyboard.keycode)
          {
          case ALLEGRO_KEY_LEFT:  //TECLA SETA ESQUERDA 
            {
            m_key[M_RIGHT] = !(m_key[M_LEFT]  = true);
            console_function_log(YELLOW"Precionou para ESQUERDA"RESET);
            break;	
            }
          case ALLEGRO_KEY_RIGHT: //TECLA SETA DIREITA
            {
            m_key[M_LEFT] = !(m_key[M_RIGHT]  = true);
            console_function_log(YELLOW"Precionou para DIREITA"RESET);
            break;
            }
          case ALLEGRO_KEY_ENTER:
            {
            /*Se a cena estiver no estado inicial, iniciara a preparacao da par-
              tida localizada em M_LOADING*/
            if(m_scene[M_START])
              {
              //Start setta como false e loading setta como true
              m_scene[M_START] = !(m_scene[M_LOADING] = true);
              }
            //Setta a tecla enter como true;
            m_key[M_ENTER] = true;
            console_function_log(YELLOW"Precionou ENTER"RESET);
            break;
            }
          case ALLEGRO_KEY_ESCAPE:
            {
            done = true;
            console_function_log(YELLOW"Precionou ESCAPE"RESET);
            break;
            }        
          }   
        }

      /*Calculo de eventos por scena -----------------------------------------*/
      if(m_scene[M_GAME])//-----------------------------------------------------
        {
        if(m_key[M_ENTER])//====================================================                    
          {
          //Limpa o evento do enter assim que a jogada ocorreu
          if(conn_data.conn.flag == CLIENT_TURN)
            {
            char msg_attack; //Variavel da jogada
            sprintf(&msg_attack,"%d",direction);
            
            //Enviado a jogada do cliente ao servidor
            if((send(conn_data.conn.sockfd, &msg_attack, sizeof(char), 0)) == -1)
              {
              m_msg_error("Erro no envio da jogada ao servidor\n");
              }
            else
              {
              printf(" ## %d \t>> Jogada enviada para o servidor com "GREEN"SUCESSO\n"RESET, pross_count++ );
              fflush(stdout);
              }

            printf("# %d \t>> Aguardadando o outro jogador: "YELLOW"CLIENT_WAIT\n"RESET, pross_count++ );
            fflush(stdout);
            conn_data.conn.flag = CLIENT_WAIT;
            }
          else if(conn_data.conn.flag == CLIENT_RESULT)
            {
            al_rest(2.0);
            conn_data.conn.flag = CLIENT_TURN;
            turn = true;
            }
          m_key[M_ENTER] = turn = false;
          }
        else if((m_key[M_LEFT] || m_key[M_RIGHT]) && turn )//===================
          {
          //Evento de percorrer a lista para direita ou esquerda
          if(m_key[M_RIGHT] == true )   //RIGHT
            {
            if(direction >= 2 )
              {
              direction = 0;
              }
            else
              {
              direction++;
              m_key[M_RIGHT] = false; 
              }
            }

          if(m_key[M_LEFT] == true )   //LEFT
            {
            if(direction <= 0 )
              {
              direction = 2;
              }
            else
              {
              direction--;
              m_key[M_LEFT] = false; 
              }          
            }
          }
        }
      else if(m_scene[M_LOADING])//---------------------------------------------
        {
        float
          control = 0, piece = 100.0f/ 5;

        //Realizando a conexao
        memset(&conn_data, 0, sizeof(CONN_CLIENT));
        waitConnection("Conectando-se ao Servidor"); 
        networkClient(&conn_data);

        waitConnection("Aguardando outro usuario conectar"); 
        initComm(conn_data.conn.sockfd);
        polling_thread  = al_create_thread(pollingThread, &conn_data);
        conn_data.mutex =  al_create_mutex();
        conn_data.cond  =  al_create_cond();
        al_start_thread(polling_thread);

        waitConnection("Encontrado outro jogador"); 

        //Inicia com zero
        gameLoading( control += piece );
        al_rest(0.5);        

   
        createHandObj( &hand[0], 100.0, 100.0, 128.0, 128.0 , "assets/sprites/hand_paper.png", "PAPEL");
        printHandObj( hand[0] );
        gameLoading( control += piece );   
        al_rest(0.5);

   
        createHandObj( &hand[1], 336.0, 200.0, 128.0, 128.0 , "assets/sprites/hand_rock.png", "PEDRA");
        printHandObj( hand[1] );
        gameLoading( control += piece );   
        al_rest(0.5);


        createHandObj( &hand[2], 572.0, 100.0, 128.0, 128.0 , "assets/sprites/hand_scizor.png", "TESOURA" );
        printHandObj( hand[2] );
        gameLoading( control += piece );   
        al_rest(0.5);

        
        m_key[M_ENTER] = m_scene[M_LOADING] = !(m_scene[M_GAME] = true );
        gameLoading( control += piece );   
        al_rest(1.0);
        }


      /*Renderizacao do jogo -------------------------------------------------*/
      if(redraw && al_is_event_queue_empty(event_queue)) //---------------------
        {
        redraw = false;
        al_clear_to_color(al_map_rgb( 10, 10, 50));
        
        if(m_scene[M_GAME])//===================================================
          {
          renderHandObjCursor( &hand[0] );
          renderHandObjCursor( &hand[1] );
          renderHandObjCursor( &hand[2] );

          if(conn_data.conn.flag == CLIENT_TURN)
            {
            renderSelectionHandObj( &hand[direction ] );
            al_draw_rectangle(100.0, 480, 700, 560, al_map_rgb(20, 20, 60 + ( 10 * direction )), 10.0);
            al_draw_filled_rectangle(100.0, 480, 700, 560, al_map_rgb(30, 30, 70));

            renderSelectionHandObjTitle( hand_title_color, font, hand[direction].title );
            }
          else if(conn_data.conn.flag == CLIENT_WAIT)
            {
            al_draw_rectangle(100.0, 480, 700, 560, al_map_rgb(10, 10, 10 + ( 10 * direction )), 10.0);
            al_draw_filled_rectangle(100.0, 480, 700, 560, al_map_rgb(10, 10, 10));
            renderWaitLabel(hand_title_color, font,"Aguardo o Resultado"); 
            }
          else if(conn_data.conn.flag == CLIENT_RESULT)
            {
            char *final = drawResult(conn_data.conn.attack.rslt);
            renderWaitLabel(hand_title_color, font, final);
            final = NULL;
            free(final);
            turn = true;
            }
          }
        else if(m_scene[M_START])//=============================================
          {
          alpha = blinkerFont(100, 20); 

          gameInterface(font, al_map_rgba_f(r * alpha, g * alpha, b * alpha, alpha));
          }

        al_flip_display();
        }
      }
    } 
  al_destroy_thread(polling_thread);
}


