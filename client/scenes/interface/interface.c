float blinkerFont( int speed, float delay)
{
  int
    half_speed = (speed / 2 );

  static int
    show_font  = 0;
  
  float static
    alpha     = 0.0f;
  

  /*Se maior que a velocidade, retorna ao comeco*/
  if(show_font >= speed )
    {
    show_font = -1 * delay;
    alpha = 0.0f;
    }
  else if(show_font > half_speed - 1 )
    {
    alpha = alpha - (1.0f / half_speed ) ;
    }
  else if(show_font >= 0)
    {
    alpha = alpha + (1.0f / half_speed ) ;
    }

  show_font++;

  return alpha;
}

void gameInterface(ALLEGRO_FONT *font, ALLEGRO_COLOR color)
{
  //Logo do jogo

  al_draw_text(font, color, 400, 400,
                        ALLEGRO_ALIGN_CENTRE, "PRESSIONE ENTER");
  al_draw_text(font, color, 400, 470,
                        ALLEGRO_ALIGN_CENTRE, "PARA JOGAR");
}
