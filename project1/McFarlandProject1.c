#include <unistd.h> //for getopt and file operations
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include <fcntl.h> //for file operations

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
  char *ipaddressfilename;

  //read input args into variables
  while((input = getopt(argc, argv, "ipo:L:")) != -1)
  {
    int prevLset = flags[3];
    char *option = parseInput(input, flags);

    //if the given argument is L, copy the input file name
    if(prevLset == FALSE && flags[3] == TRUE)
    {
      //no need to check if null; already handled by getopt
      ipaddressfilename = option;
    }
  }

  char *testaddress = "abcd"; //delete later
  dumpAddress(testaddress);

  //ensure an input file is given
  if(flags[3] == FALSE)
  {
    printf("Error: missing input file argument\n");
    return 1;
  }

  //open and read from input file
  int ipaddressfile;
  int ipaddressfileread;
  void *filereadbuffer;
  if((ipaddressfile = open(ipaddressfilename,O_RDONLY))<0)
  {
    printf("Failed to open file\n");
    return 1;
  }
  //may not be proper use of arguments
  ipaddressfileread = read(ipaddressfile,filereadbuffer,IPADDRESSSIZE);
  close(ipaddressfile);

  return 0;
}
