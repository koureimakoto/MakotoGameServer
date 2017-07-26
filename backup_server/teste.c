#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct _msg
{
  int8_t flag1;
  int8_t flag2;
  int32_t msg;
};



int main()
{
  struct _msg *imsg;

  char cmsg[6] = "abcde";

  imsg = ( struct _msg *)&cmsg;

  printf("%s\n", (char *) imsg);

  return 0;
}
