#include <unistd.h> //for getopt
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset

#define TRUE 1
#define FALSE 0
#define NUMFLAGS 3
#define FLAGSALLOCSIZE NUMFLAGS*4

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

  while((input = getopt(argc, argv, "ipo:")) != -1)
  {
    //keeps reporting stack smashing
    char *option = parseInput(input, flgs);
    printf("%d",(int)sizeof(*option));
    printf("%s",option);
  }

  //printf("flag_addresses = %d \n", flags[0]);
  //printf("flag_prefixes = %d \n", flags[1]);
  //printf("flag_orgs = %d \n", flags[2]);


  return 0;
}
