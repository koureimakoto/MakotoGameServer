//DEFINES for GAME TCP LIB

#define M_ERR_EXIT 0
#define M_ERR_LOG 1

int criticalerror()
{
  fprintf( stderr, "Critical Error: %d Type: %s\n", errno, strerror(errno));
  exit(EXIT_FAILURE); // Pode ser return(-1) caso não haja a função, mas  
}
