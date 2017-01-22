#include <unistd.h> //for getopt
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset

#define TRUE 1
#define FALSE 0

#define NUMFLAGS 3

void parseInput(int input, int (*flags)[NUMFLAGS])
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
      break;
    case '?':
      if(optopt == 'c')
      {
        exit(1);
      }
      else
      {
        exit(2);
      }
    }
}

int main(int argc, char *argv[])
{

  int flags[NUMFLAGS];
  int (*flgs)[NUMFLAGS] = &flags;
  memset(flags, 0x0, NUMFLAGS);
  printf("%d", flags[2]);
  int input;

  while((input = getopt(argc, argv, "ipo:")) != -1)
  {
    parseInput(input, flgs);
  }

  printf("%d",(int)sizeof(flags)/(int)sizeof(int));
  printf("flag_addresses = %d \n", flags[0]);
  printf("flag_prefixes = %d \n", flags[1]);
  printf("flag_orgs = %d \n", flags[2]);


  return 0;
}
