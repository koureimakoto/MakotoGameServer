# MakotoGameServer - ALFA

APRESENTAÇÃO PESSOAL:

    Meu nome é [!FAILED], mas sinta-se livre para me chamar pelo meu nome espiritual Kourei. Nada mais.

APRESENTAÇÃO:

  Estes softwares correspondem ao meu trabalho de conclusão de curso, da Faculdade e como tivesse a permissão
desenvolver um jogo com diversas tecnologias livres decidi por manter este código disponível e publico.

  
SOBRE OS SOFTWARE:

  Neste projeto foi desenvolvido um jogo que possibilita jogar em lan com outro usuário o famoso jogo *Pedra, 
Papel e Tesoura*.

  Possui dois um cliente e um servidor, que se comunicam através de *Socket* em Linux; servidor desenvolvido
apenas apenas em modo texto, e o cliente apenas em modo gráfico programado com a bibliotéca Allegro 5.

  O sistema operacional utilizado foi a distribuição Debian GNU/Linux na sua versão 8(Jessie), não havendo 
até o momento nenhum teste realizado em outro distribuição.
  
  O jogo esta conseguindo realizar apenas uma unica apartida e seu servidor ainda apresenta instabilidades e
diversas implementações a serem realizados. Estas implemntações serão todas documentados a ponto de deixar
uma contribuição a quem se aventurar a desenvolver algo semelhante.


____________________________________________________________________________________________________________

PARA COMPILAÇÃO:

  Não estarei disponibilizando o código da liballegro neste projeto, apenas a versão utilizada para teste
  se desejar utilizar outras versões e obter bons tester, por favor informar ou subir um push alterando os
  as informações do README, por favor.
  
  VERSÃO ALLEGRO:
  
    >> 2:5.0.10-3+b1           | OK [[Não esqueça de baixar os addon compativeis a esta versão]]
  
  
  VERSÃO GCC:
  
    >> (Debian 4.9.2-10) 4.9.2 | OK
  
  
  COMANDO GCC (Enquanto não termino de gerar o make - os nomes dos arquivos vão ser substituídos):
  
    >>gcc -Wall -o [OUTPUT_FILE] m_game_engine.c `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0
      allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0
      allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`

    Obs: Está compilando com os parâmetros, como de áudio, que ainda não estão ainda não tem uso por falta de
         implementação. 
  
  
  ______________________________________________________________________________________________________________
    
    
  
 
