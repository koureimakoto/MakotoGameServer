//#include "../interface/interface.c"

enum direction
{
  NEXT
};

struct hand
{
  float x_axis;
  float y_axis;
  float width;
  float height;
  char  path[100];
  char  title[21];
  ALLEGRO_BITMAP *image;
};

ALLEGRO_BITMAP
  *hand_image[3];

struct hand
  hand[3];

void createHandObj( struct hand *hand, float x, float y, float x2, float y2, char *path, char *title)
{
  hand->x_axis = x;
  hand->y_axis = y;
  hand->width  = x2;
  hand->height = y2;
  strcpy(hand->path, path);
  strcpy(hand->title, title);
  hand->image = al_load_bitmap(path); 
  al_convert_mask_to_alpha(hand->image, al_map_rgb(255,0,0));
}

void printHandObj(struct hand hand )
{
  printf("Eixo x : %.2f Eixo y : %.2f \n"
         "Eixo x2: %.2f Eixo y2: %.2f \n"
         "Caminho: %s\n",
         hand.x_axis , hand.y_axis,
         hand.width, hand.height,
         hand.path);
  fflush(stdout);
}

void renderSelectionHandObj( struct hand *hand )
{
  al_draw_circle( hand->x_axis + 64 , hand->y_axis + 64 , (hand->width - 20), al_map_rgb(200,200,200), 5.0 ); 
}

void renderSelectionHandObjTitle(ALLEGRO_COLOR color, ALLEGRO_FONT *font, char *title )
{
  al_draw_textf(font, color, 400, 480, ALLEGRO_ALIGN_CENTRE, "%s", title);
}

void renderWaitLabel(ALLEGRO_COLOR color, ALLEGRO_FONT *font,char *text)
{
  al_draw_textf(font, color, 400, 480, ALLEGRO_ALIGN_CENTRE, "%s", text);
}

void renderHandObjCursor( struct hand *hand )
{
  al_draw_bitmap( hand->image, hand->x_axis, hand->y_axis , 0 );
}


char *drawResult(int result)
{
  char
   *final = (char *) malloc(sizeof(char )*15);

  ALLEGRO_COLOR
    line_color,
    fill_color;  

  memset(final, 0, 15);

  switch(result)
    {
    case M_DRAW:
      {
      strcpy(final, "EMPATOU");
      line_color = al_map_rgb(50,50,50);
      fill_color = al_map_rgb(150,150,150);
      break;
      }
    case M_WIN:
      {
      strcpy(final, "SOU VENCEDOR");
      line_color = al_map_rgb(10,50,10);
      fill_color = al_map_rgb(50,150,50);
      break;
      }
    case M_LOSE:
      {
      strcpy(final, "FUI DERROTADO");
      line_color = al_map_rgb(50,10,10);
      fill_color = al_map_rgb(150,50,50);
      break;
      }
    default:
      {
      strcpy(final, "ERROR_VALUE");
      line_color = al_map_rgb(10,10,10);
      fill_color = al_map_rgb(20,20,20);
      break;
      }
    }
  al_draw_rectangle(100.0, 480, 700, 560, line_color, 10.0);
  al_draw_filled_rectangle(100.0, 480, 700, 560, fill_color);

  return(final);
}


void waitConnection(char *msg)
{
  int i;
  float x, y, width, border = 5.0f, a;
  float r, b, g;
  ALLEGRO_FONT *font;
  
  if( !(font = al_load_font("assets/fonts/beefd.ttf", 10, 0) ) )
    {   
    m_msg_error("Falha no carregamento da fonte\n");
    exit(EXIT_FAILURE);
    }

  x = y = 400.0f;
  width = 50.0f;
  r = g = b = a = 1.0f;
  for( i = 0 ; i < 100  ; i ++)
    {
      al_clear_to_color(al_map_rgb( 10, 10, 50));
      al_draw_circle(x, y, width, al_map_rgba_f(r,g,b, 1.0), border);
      al_draw_textf(font, al_map_rgba_f(r, g, b, 1.0), 400, 480, ALLEGRO_ALIGN_CENTRE, "Carregando [%s]", msg);
    if(i < 50)
      {
      al_draw_circle(x, y, width - i, al_map_rgba_f(r * a, g * a, b * a, a), border - a * 2 ); 
      al_flip_display();
      al_clear_to_color(al_map_rgb( 10, 10, 50));
      a = a - (1.0/80);
      }
    else if( i < 81)
      {
      al_draw_circle(x, y, (width - 50 )+ i, al_map_rgba_f(r * a, g * a, b * a, a), border - a * 2 ); 
      al_flip_display();
      al_clear_to_color(al_map_rgb( 10, 10, 50));
      a = a - (1.0/80);
      }
    al_rest(0.05);
    }
}
