/*
  Brennan McFarland
  bfm21
  McFarlandProject1.c
  02/04/2017
  Given an input file (-L [input file]) of IP addresses, outputs formatted
  addresses (-i), formatted prefixes (-p), and/or associated organization names
  (-o [org list file]) to the terminal
*/

#include <unistd.h> //for getopt and file operations
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include <fcntl.h> //for file operations
#include "McFarlandProject1.h"


//prints the formatted IP address
void dumpAddress(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[INDEXINITIALIZER]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  for(int i=SKIPINDEXINITIALIZER; i<IPADDRESSSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//prints the formatted /24 prefix of the IP address
void dumpPrefix(char *ipaddress)
{
  int ipaddressnumberbyte = (int)(ipaddress[INDEXINITIALIZER]);
  printf("%d",(unsigned char)ipaddressnumberbyte);
  for(int i=SKIPINDEXINITIALIZER; i<IPADDRESSPREFIXSIZE; i++)
  {
    ipaddressnumberbyte = (int)(ipaddress[i]);
    printf(".%d",(unsigned char)ipaddressnumberbyte);
  }
}

//prints the organization associated with the given IP address
void dumpOrg(char *ipaddress, OrgPrefix orgprefixlist[], int listsize)
{
  for(int i=INDEXINITIALIZER; i<listsize; i++)
  {
    if(memcmp(ipaddress, orgprefixlist[i].ipprefix, IPADDRESSPREFIXSIZE)
      == RETURNSUCCESS)
    {
      printf("%s", orgprefixlist[i].orgname);
      return;
    }
  }
  printf("Error: organization not found");
  exit(RETURNERROR);
}

//returns the number of lines and max linelength of a file
int getFileLines(char *filename, int *maxlinelength)
{
  //run through the file once to determine the number of lines
  int filenum;
  if((filenum = open(filename,O_RDONLY))<RETURNSUCCESS)
  {
    printf("Failed to determine file size\n");
    exit(ERRORCODE4);
  }

  char filechar = '0'; //initialized but always overwritten
  char *filebuffer;
  filebuffer = &filechar;
  unsigned long filenumlines = TALLYINITIALIZER;
  int linelength = TALLYINITIALIZER;
  int maxlinelengthvalue = TALLYINITIALIZER;
  maxlinelength = &maxlinelengthvalue;

  while((readbyte(filenum, filebuffer)) != RETURNFAILURE)
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
        linelength = TALLYINITIALIZER;
      }
    }
  }
  close(filenum);
  return filenumlines;
}
//compiles the list of organizations' prefixes from the given file
void compileOrgList(OrgPrefix *orgprefixlist, char *orgprefixfilename,
  int orglistfilenumlines, int maxlinelengthvalue)
{
  int orglistfile;
  char orglistfilechar = '0'; //initialized but always overwritten
  char *orglistfilebuffer;
  orglistfilebuffer = &orglistfilechar;

  //compile the list into the array
  if((orglistfile = open(orgprefixfilename,O_RDONLY))<RETURNSUCCESS)
  {
    printf("Failed to open file\n");
    exit(ERRORCODE4);
  }

  char ipaddressdecimalbuffer[IPADDRESSDECIMALBUFFERSIZE];
  int ipaddressdecimalbufferindex;
  char ipprefix[IPADDRESSPREFIXSIZE];

  for(int i=INDEXINITIALIZER; i<orglistfilenumlines; i++)
  {
    //read ip address byte by byte
    for(int j=INDEXINITIALIZER; j<IPADDRESSPREFIXSIZE; j++)
    {
      ipaddressdecimalbufferindex = INDEXINITIALIZER;
      do {
        if(readbyte(orglistfile, orglistfilebuffer) == RETURNFAILURE)
        {
          if(i > orglistfilenumlines-SKIPINDEXINITIALIZER)
            return;
          printf("Error: bad input data in organization list file");
          exit(ERRORCODE5);
        }
        ipaddressdecimalbuffer[ipaddressdecimalbufferindex] = orglistfilechar;
        ipaddressdecimalbufferindex++;
      } while(orglistfilechar != '.' && orglistfilechar != ' ');
      //and reset unused array elements
      while(ipaddressdecimalbufferindex < IPADDRESSDECIMALBUFFERSIZE)
      {
        ipaddressdecimalbuffer[ipaddressdecimalbufferindex] = '\n';
        ipaddressdecimalbufferindex++;
      }
      ipprefix[j] = (char)(atoi(ipaddressdecimalbuffer));
    }

    //read organization name
    char orglistfilelinebuffer[maxlinelengthvalue];
    int orglistfilelinecharindex = INDEXINITIALIZER;
    memset((void *)orglistfilelinebuffer,FALSE,maxlinelengthvalue);
    do {
      if(readbyte(orglistfile, orglistfilebuffer) == RETURNFAILURE)
      {
        if(i < orglistfilenumlines-SKIPINDEXINITIALIZER)
          return;
        printf("Error: bad input data in organization list file");
        exit(ERRORCODE5);
      }
      orglistfilechar = *(char *)(orglistfilebuffer);
      orglistfilelinebuffer[orglistfilelinecharindex] = orglistfilechar;
      orglistfilelinecharindex++;
    }while(orglistfilechar != '\n' && orglistfilechar != '\r');

    //and dump them both in the struct
    for(int k=INDEXINITIALIZER; k<IPADDRESSPREFIXSIZE; k++)
      orgprefixlist[i].ipprefix[k] = ipprefix[k]; //the segfault happens here
    orgprefixlist[i].orgname = orglistfilelinebuffer;
  }
  close(orglistfile);
}

//sets argument flag and returns any options given
char *parseInput(int input, int flags[])
{
    switch(input)
    {
    case 'i':
      flags[FLAGA] = TRUE;
      break;
    case 'p':
      flags[FLAGB] = TRUE;
      break;
    case 'o':
      flags[FLAGC] = TRUE;
      return optarg;
      break;
    case 'L':
      flags[FLAGD] = TRUE;
      return optarg;
      break;
    case '?':
      if(optopt == 'c')
      {
        exit(ERRORCODE2);
      }
      else
      {
        exit(ERRORCODE3);
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
  == RETURNSUCCESS)
  {
    return RETURNFAILURE;
  }
  else if(fileread != BYTESIZE)
  {
    printf("Failed to read from file\n");
    exit(RETURNFAILURE);
  }
  else
  {
    *readbyte = *(char *)(readbuffer);
  }
  return RETURNSUCCESS;
}

void *safemalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (RETURNFAILURE);
    }
    memset (p,FALSE,sz);
    return (p);
}

int main(int argc, char *argv[])
{
  int flags[NUMFLAGS];
  for(int i=INDEXINITIALIZER; i<NUMFLAGS; i++)
    flags[i] = FALSE;
  int input;
  char *ipaddressfilename;
  char *orgprefixfilename;

  //read input args into variables
  while((input = getopt(argc, argv, "ipo:L:")) != -RETURNFAILURE)
  {
    int prevLset = flags[FLAGD];
    int prevoset = flags[FLAGC];
    char *option = parseInput(input, flags);

    //if the given argument is L, copy the input file name
    if(prevLset == FALSE && flags[FLAGD] == TRUE)
    {
      //no need to check if null; already handled by getopt
      ipaddressfilename = option;
    } //if it's o, copy the organization list file name
    else if(prevoset == FALSE && flags[FLAGC] == TRUE)
    {
      orgprefixfilename = option;
    }
  }

  int orglistfilelines = TALLYINITIALIZER;
  int *maxlinelength;
  int maxlinelengthvalue = TALLYINITIALIZER;
  maxlinelength = &maxlinelengthvalue;
  //if -o flag set, compile the list of organizations and their prefixes
  if(flags[FLAGC] == TRUE)
  {
    orglistfilelines = getFileLines(orgprefixfilename, maxlinelength);
  }
  OrgPrefix orgprefixlistarray[orglistfilelines];

  if(flags[FLAGC] == TRUE)
  {
    //allocate memory for the array
    for(int i=INDEXINITIALIZER; i<*maxlinelength; i++)
    {
      OrgPrefix orgprefixblank;
      orgprefixlistarray[i] = orgprefixblank;
      for(int j=INDEXINITIALIZER; j<IPADDRESSPREFIXSIZE; j++)
      {
        orgprefixlistarray[i].ipprefix[j] = '0';
      }
      orgprefixlistarray[i].orgname = (char *)(safemalloc(maxlinelengthvalue));
    }
    compileOrgList(orgprefixlistarray, orgprefixfilename, orglistfilelines, maxlinelengthvalue);
  }

  //ensure an input file is given
  if(flags[FLAGD] == FALSE)
  {
    printf("Error: missing input file argument\n");
    return RETURNFAILURE;
  }

  //open and read from input file
  int ipaddressfile;
  if((ipaddressfile = open(ipaddressfilename,O_RDONLY))<RETURNSUCCESS)
  {
    printf("Failed to open file\n");
    return RETURNFAILURE;
  }

  char ipaddress[IPADDRESSSIZE];
  char ipaddressfilechar = '0'; //initialized but always overwritten
  char *ipaddressfilebuffer;
  ipaddressfilebuffer = &ipaddressfilechar;
  int ipaddressnumberbyte = INDEXINITIALIZER;

  while((readbyte(ipaddressfile, ipaddressfilebuffer)) != RETURNFAILURE)
  {
    ipaddress[ipaddressnumberbyte] = ipaddressfilebuffer[INDEXINITIALIZER];
    ipaddressnumberbyte++;
    if(ipaddressnumberbyte == IPADDRESSSIZE)
    {
      ipaddressnumberbyte = INDEXINITIALIZER;
      if(flags[FLAGA] == TRUE)
      {
        dumpAddress(ipaddress);
        if(flags[FLAGB] == TRUE)
          printf(" ");
      }
      if(flags[FLAGB] == TRUE)
      {
        dumpPrefix(ipaddress);
      }
      if(flags[FLAGC] == TRUE)
      {
        dumpOrg(ipaddress, orgprefixlistarray, orglistfilelines);
      }
      if(flags[FLAGA] == TRUE || flags[FLAGB] == TRUE || flags[FLAGC] == TRUE)
        printf("\n");
    }
  }

  close(ipaddressfile);

  return RETURNSUCCESS;
}
