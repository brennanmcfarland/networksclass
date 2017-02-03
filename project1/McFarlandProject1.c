#include <unistd.h> //for getopt and file operations
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include <fcntl.h> //for file operations
#include "McFarlandProject1.h"


//prints the formatted IP address
void dumpAddress(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[0]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  for(int i=1; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//prints the formatted /24 prefix of the IP address
void dumpPrefix(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[0]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  for(int i=1; i<IPADDRESSPREFIXSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//prints the organization associated with the given IP address
void dumpOrg(char *ipaddress, OrgPrefix orgprefixlist[], int listsize)
{
  for(int i=0; i<listsize; i++)
  {
    if(memcmp(ipaddress, orgprefixlist[i].ipprefix, IPADDRESSPREFIXSIZE) == 0)
    {
      printf("%s", orgprefixlist[i].orgname);
      return;
    }
  }
  printf("Error: organization not found");
  exit(10);
}

//returns the number of lines and max linelength of a file
int getFileLines(char *filename, int *maxlinelength)
{
  //run through the file once to determine the number of lines
  int filenum;
  if((filenum = open(filename,O_RDONLY))<0)
  {
    printf("Failed to determine file size\n");
    exit(-4);
  }

  char filechar = '0'; //initialized but always overwritten
  char *filebuffer;
  filebuffer = &filechar;
  unsigned long filenumlines = 0;
  int linelength = 0;
  int maxlinelengthvalue = 0;
  maxlinelength = &maxlinelengthvalue;

  while((readbyte(filenum, filebuffer)) != 1)
  {
    filechar = *(char *)(filebuffer);
    linelength++;
    //read until line ending reached to determine number of lines and max length
    if(filechar == '\n' || filechar == '\r')
    {
      filenumlines++;
      if(linelength > *maxlinelength)
      {
        *maxlinelength = linelength;
        linelength = 0;
      }
    }
  }
  close(filenum);
  return filenumlines;
}
//compiles the list of organizations' prefixes from the given file
void compileOrgList(OrgPrefix orgprefixlist[], char *orgprefixfilename, int orglistfilenumlines, int *maxlinelength)
{
  int orglistfile;
  char orglistfilechar = '0'; //initialized but always overwritten
  char *orglistfilebuffer;
  orglistfilebuffer = &orglistfilechar;

  //compile the list into the array
  if((orglistfile = open(orgprefixfilename,O_RDONLY))<0)
  {
    printf("Failed to open file\n");
    exit(-4);
  }

  char ipaddressdecimalbuffer[IPADDRESSDECIMALBUFFERSIZE];
  int ipaddressdecimalbufferindex;
  char ipprefix[IPADDRESSPREFIXSIZE];

  for(int i=0; i<orglistfilenumlines; i++)
  {
    //read ip address byte by byte
    for(int j=0; j<IPADDRESSPREFIXSIZE; j++)
    {
      ipaddressdecimalbufferindex = 0;
      do {
        if(readbyte(orglistfile, orglistfilebuffer) == 1)
        {
          if(i < orglistfilenumlines-1)
            return;
          printf("Error: bad input data in organization list file");
          exit(-5);
        }
        orglistfilechar = *(char *)(orglistfilebuffer);
        ipaddressdecimalbuffer[ipaddressdecimalbufferindex] = orglistfilechar;
        ipaddressdecimalbufferindex++;
      } while(orglistfilechar != '.' && orglistfilechar != ' ');
      //and reset unused array elements
      while(ipaddressdecimalbufferindex < 3)
      {
        ipaddressdecimalbuffer[ipaddressdecimalbufferindex] = '\n';
        ipaddressdecimalbufferindex++;
      }
      ipprefix[j] = atoi(ipaddressdecimalbuffer);
      printf("String: %s, Integer: %d", ipprefix, (unsigned char)ipprefix[j]);
    }
    //read organization name
    char orglistfilelinebuffer[*maxlinelength];
    int orglistfilelinecharindex = 0;
    memset((void *)orglistfilelinebuffer,0x0,*maxlinelength);
    do {
      if(readbyte(orglistfile, orglistfilebuffer) == 1)
      {
        if(i < orglistfilenumlines-1)
          return;
        printf("Error: bad input data in organization list file");
        exit(-5);
      }
      orglistfilechar = *(char *)(orglistfilebuffer);
      orglistfilelinebuffer[orglistfilelinecharindex] = orglistfilechar;
      printf("| %c ", orglistfilechar);
      orglistfilelinecharindex++;
    }while(orglistfilechar != '\n' && orglistfilechar != '\r');
    //and dump them both in the struct
    for(int k=0; k<IPADDRESSPREFIXSIZE; k++)
      (*orgprefixlist)[i].ipprefix[k] = ipprefix[k];
    (*orgprefixlist)[i].orgname = orglistfilelinebuffer;
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
  char *orgprefixfilename;

  //read input args into variables
  while((input = getopt(argc, argv, "ipo:L:")) != -1)
  {
    int prevLset = flags[3];
    int prevoset = flags[2];
    char *option = parseInput(input, flags);

    //if the given argument is L, copy the input file name
    if(prevLset == FALSE && flags[3] == TRUE)
    {
      //no need to check if null; already handled by getopt
      ipaddressfilename = option;
    } //if it's o, copy the organization list file name
    else if(prevoset == FALSE && flags[2] == TRUE)
    {
      orgprefixfilename = option;
    }
  }

  int orglistfilelines = 0;
  OrgPrefix orgprefixlistarray = NULL;
  //if -o flag set, compile the list of organizations and their prefixes
  if(flags[2] == TRUE)
  {
    int maxlinelengthvalue = 0;
    int *maxlinelength;
    maxlinelength = &maxlinelengthvalue;
    orglistfilelines = getFileLines(orgprefixfilename, maxlinelength);
    OrgPrefix orgprefixlistarray[orglistfilelines];
    //OrgPrefix **orgprefixlist;

    //allocate memory for the array
    for(int i=0; i<*maxlinelength; i++)
    {
      for(int j=0; j<IPADDRESSPREFIXSIZE; j++)
      {
        orgprefixlistarray[i].ipprefix[j] = '0';
      }
      orgprefixlistarray[i].orgname = (char *)(safemalloc(*maxlinelength));
    }
    //orgprefixlist = &orgprefixlistarray;
    compileOrgList((OrgPrefix [])orgprefixlistarray, orgprefixfilename, orglistfilelines, maxlinelength);
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
      {
        dumpAddress(ipaddress);
        if(flags[1] == TRUE)
          printf(" ");
      }
      if(flags[1] == TRUE)
      {
        dumpPrefix(ipaddress);
      }
      if(flags[2] == TRUE)
      {
        dumpOrg(ipaddress, (OrgPrefix [])orgprefixlistarray, orglistfilelines);
      }
      //if(flags[0] == TRUE || flags[1] == TRUE || flags[2] == TRUE)
      //  printf("\n");
    }
  }

  close(ipaddressfile);

  return 0;
}
