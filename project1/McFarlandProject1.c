#include <unistd.h> //for getopt
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset

#define TRUE 1
#define FALSE 0
#define NUMFLAGS 4
#define FLAGSALLOCSIZE NUMFLAGS*sizeof(int)
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
char *parseInput(int input, int flags[])
{
    switch(input)
    {
    case 'i':
      flags[0] = TRUE;
      break;
    case 'p':
      flags[1] = TRUE;
      break;
    case 'o':
      flags[2] = TRUE;
      return optarg;
      break;
    case 'L':
      flags[3] = TRUE;
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
  for(int i=0; i<NUMFLAGS; i++)
    flags[i] = 0;
  int input;

  char *testaddress = "abcd"; //delete later
  dumpAddress(testaddress);

  //read input
  while((input = getopt(argc, argv, "ipo:L:")) != -1)
  {
    char *option = parseInput(input, flags);
    if(option != NULL)
    {
      printf("%d",(int)sizeof(*option));
      printf("%s",option);
    }
  }
  //ensure an input file is given
  if(flags[3] == FALSE)
  {
    printf("Error: missing input file argument\n");
    return 1;
  }

  //don't forget to check for L

  return 0;
}
