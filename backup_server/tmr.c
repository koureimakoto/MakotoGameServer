#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
_act( int sig, siginfo_t *siginfo, void * context )
{
   printf("SIGNAL");
}

static void
_fooNULL( void * foo_t )
{
   memset( foo_t, '\0', sizeof *foo_t );
}


int
main(void)
{

   struct sigaction sigac;

   memset( &sigac, '\0', sizeof sigac );
   //_fooNULL( &sigac );
   sigac.sa_sigaction = &_act;
   sigac.sa_flags  = SA_SIGINFO;
   sigemptyset(&sigac.sa_mask);

   if(sigaction(SIGTERM, &sigac, NULL) == -1)
   {
      perror("sigaction");
      exit(EXIT_FAILURE);
   }

   alarm(10);
   
   if( 1 == 1 )
      perror("oi");

   return 0;
}
