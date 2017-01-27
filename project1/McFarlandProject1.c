#include <unistd.h> //for getopt
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset

#define TRUE 1
#define FALSE 0
#define NUMFLAGS 3
#define FLAGSALLOCSIZE NUMFLAGS*4
#define IPADDRESSSIZE 4

//prints the formatted IP address
void dumpAddress(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[0]);
  printf("%d",ipaddressnumberbyte);
  for(int i=0; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",ipaddressnumberbyte);
  }
}

//sets argument flag and returns any options given
char *parseInput(int input, int (*flags)[NUMFLAGS])
{
    switch(input)
    {
    case 'i':
      *flags[0] = TRUE;
      break;
    case 'p':
      *flags[1] = TRUE;
      break;
    case 'o':
      *flags[2] = TRUE;
      return optarg;
      break;
    case 'L':
      return optarg;
      break;
    case '?':
      if(optopt == 'c')
      {
        exit(-2);
      }
      else
      {
        exit(-3);
      }
    }
    return NULL;
}

int main(int argc, char *argv[])
{

  int flags[NUMFLAGS];
  int (*flgs)[NUMFLAGS] = &flags;
  memset(flags, 0x0, FLAGSALLOCSIZE);
  int input;

  char *testaddress = "abcd"; //delete later
  dumpAddress(testaddress);
  while((input = getopt(argc, argv, "ipo:")) != -1)
  {
    //keeps reporting stack smashing
    char *option = parseInput(input, flgs);
    printf("%d",(int)sizeof(*option));
    printf("%s",option);
  }

  return 0;
}
