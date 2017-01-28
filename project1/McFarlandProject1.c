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
#define BYTESIZE 1

//prints the formatted IP address
char *dumpAddress(char *ipaddress)
{
  char formattedipaddress[IPADDRESSSIZE*(IPADDRESSSIZE-1)];
  int ipaddressnumberbyte = (int)(ipaddress[0]);
  char[0] = (unsigned char)ipaddressnumberbyte;
  for(int i=1; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
<<<<<<< HEAD
    char[2i-1] = '.';
    char[2i] = (unsigned char)ipaddressnumberbyte);
  }
  char *formattedipaddresspointer = &formattedipaddress;
  return formattedipaddresspointer;
=======
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
>>>>>>> parent of 18676ee... now -p works too!
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

//read a single byte from a file and error messages
int readbyte(int file, char *readbyte)
{
  void *readbuffer = malloc(BYTESIZE);
  int fileread;
  if((fileread = read(file,readbuffer,BYTESIZE))
  == 0)
  {
    return 1;
  }
  else if(fileread != BYTESIZE)
  {
    printf("Failed to read from file\n");
    exit(1);
  }
  else
  {
    *readbyte = *(char *)(readbuffer);
  }
  return 0;
}

void *safemalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (1);
    }
    memset (p,0x0,sz);
    return (p);
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

  //ensure an input file is given
  if(flags[3] == FALSE)
  {
    printf("Error: missing input file argument\n");
    return 1;
  }

  //open and read from input file
  int ipaddressfile;
  if((ipaddressfile = open(ipaddressfilename,O_RDONLY))<0)
  {
    printf("Failed to open file\n");
    return 1;
  }

  char ipaddress[IPADDRESSSIZE];
  char ipaddressfilechar = '0'; //initialized but always overwritten
  char *ipaddressfilebuffer;
  ipaddressfilebuffer = &ipaddressfilechar;
  int ipaddressnumberbyte = 0;

  while((readbyte(ipaddressfile, ipaddressfilebuffer)) != 1)
  {
    ipaddress[ipaddressnumberbyte] = ipaddressfilebuffer[0];
    ipaddressnumberbyte++;
    if(ipaddressnumberbyte == IPADDRESSSIZE)
    {
      ipaddressnumberbyte = 0;
      if(flags[0] == TRUE)
<<<<<<< HEAD
<<<<<<< HEAD
      {
        printf(dumpAddress(ipaddress));
        if(flags[1] == TRUE)
          printf(" ");
      }
      if(flags[1] == TRUE)
      {
        dumpPrefix(ipaddress);
      }
=======
        dumpAddress(ipaddress);
>>>>>>> parent of 18676ee... now -p works too!
=======
        dumpAddress(ipaddress);
>>>>>>> parent of 18676ee... now -p works too!
      if(flags[0] == TRUE || flags[1] == TRUE || flags[2] == TRUE)
        printf("\n");
    }
  }

  close(ipaddressfile);

  return 0;
}
