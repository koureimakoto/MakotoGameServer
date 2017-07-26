#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>

void confirma();

main() {
   int i;

   signal(SIGINT, confirma);

   for (i=1; i<20; i++) {
      printf("Estamos no ciclo numero press ctrl-c %d.\n", i);
      sleep(1);
   }
}
void confirma() {
   char sim_ou_nao, enter;
   printf("\n Quer mesmo terminar? (S/N)");
   scanf("%c%c", &sim_ou_nao, &enter);
   printf("%c\n", sim_ou_nao);
   if ((sim_ou_nao == 'S') || (sim_ou_nao == 's')) 
      exit(-1);
}
