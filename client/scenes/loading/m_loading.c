void gameLoading( float width )
{
  ALLEGRO_COLOR
    rect_color      = al_map_rgb( 100, 100, 50 ),
    rect_line_color = al_map_rgb( 50, 100, 100 );

  al_clear_to_color(al_map_rgb( 10, 10, 50));

  al_draw_rectangle( 30.0, 490.0, 770.0, 540.0, rect_color , 6.0f);

  al_draw_rectangle( 25.0, 496.0, 775.0, 534.0, rect_color , 6.0f);

  al_draw_filled_rectangle( 33 , 493 , 33 + ( 737 * ( width * 0.01 )) , 537 ,  rect_line_color );

  al_draw_line( 28, 515, 33 + ( 740 * ( width * 0.01 )) , 515 ,  rect_line_color , 40.0 );

  al_flip_display();
}


/**
 


*/
