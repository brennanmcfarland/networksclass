/*
  Brennan McFarland
  bfm21
  TrafficMatrix.c
  3/3/17
  a hashtable for an ip trace file traffic matrix
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include </usr/include/netinet/ip.h>
#include "TrafficMatrix.h"


TrafficMatrix trafficmatrix;
int trafficmatrixsize = INTINITIALIZER;
int trafficmatrixcapacity = DEFAULTTRAFFICMATRIXSIZE;


//for debugging
void printTrafficMatrixDebug()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(trafficmatrix.tableentrylists); i++)
  {
    MatrixListNode *currentbucketnodeptr = (trafficmatrix.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    printf("\n");
  }
}

//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
MatrixListNode *findInTrafficMatrix(int entryipaddresssearchkey,
  char **sourceipaddresssearchvalue, char **destipaddresssearchvalue)
{
  //if the list DNE, it's not there
  if(trafficmatrix.tableentrylists[entryipaddresssearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  MatrixListNode *currentbucketnodeptr = (trafficmatrix.tableentrylists[entryipaddresssearchkey]
    .head);
  while(currentbucketnodeptr->next != NULL &&
    ((trafficMatrixTestStringEquality(currentbucketnodeptr->entry->sourceaddressvalue,
      *sourceipaddresssearchvalue) == FALSE)
    || (trafficMatrixTestStringEquality(currentbucketnodeptr->entry->destinationaddressvalue,
      *destipaddresssearchvalue) == FALSE)))
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

//returns whether 2 strings are equal
int trafficMatrixTestStringEquality(char *string1, char *string2)
{
  if(sizeof(string1) != sizeof(string2))
  {
    return FALSE;
  }
  int i;
  for(i=INTINITIALIZER; i<sizeof(string1)-OFFSETVALUE; i++)
  {
    if(string1[i] != string2[i])
    {
      return FALSE;
    }
  }
  return TRUE;
}

void initializeTrafficMatrixList(int listfirstentryipaddresskey,
  char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress,
  int listfirstentrydatavol)
{
  MatrixListNode **headbuffer = (MatrixListNode **)trafficMatrixSafeMalloc(POINTERSIZE);
  headbuffer = &(trafficmatrix.tableentrylists[listfirstentryipaddresskey].head);
  initializeNewTrafficMatrixEntry(listfirstentryipaddresskey,
    listfirstentrysourceipaddress, listfirstentrydestipaddress,
    listfirstentrydatavol, headbuffer);
}

void initializeNewTrafficMatrixEntry(int newentryipaddresskey,
  char **newentrysourceipaddress, char **newentrydestipaddress,
  int newentrydatavol, MatrixListNode **newnode)
{
  *newnode = (MatrixListNode *)trafficMatrixSafeMalloc(sizeof(MatrixListNode));
  (*newnode)->entry = (MatrixListEntry *)trafficMatrixSafeMalloc(sizeof(MatrixListEntry *));
  *((*newnode)->entry) = *(MatrixListEntry *)trafficMatrixSafeMalloc(sizeof(MatrixListEntry));
  (*newnode)->entry->ipaddresskey = newentryipaddresskey;
  (*newnode)->entry->sourceaddressvalue = *newentrysourceipaddress;
  (*newnode)->entry->destinationaddressvalue = *newentrydestipaddress;
  (*newnode)->entry->count = ENTRYCOUNTINITIALIZER;
  (*newnode)->entry->datavol = newentrydatavol;
  (*newnode)->next = NULL;
}

void insertInTrafficMatrix(char **newentrysourceipaddress,
  char **newentrydestipaddress, int newentrydatavol)
{
  unsigned int newentryipaddresskey = trafficMatrixHashCode(newentrysourceipaddress,
    newentrydestipaddress)%(unsigned int)trafficmatrixcapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(trafficmatrix.tableentrylists[newentryipaddresskey].head == NULL)
  {
    initializeTrafficMatrixList(newentryipaddresskey, newentrysourceipaddress,
      newentrydestipaddress, newentrydatavol);
    trafficmatrixsize++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    MatrixListNode *currentbucketnodeptr = (trafficmatrix.tableentrylists[newentryipaddresskey]
      .head);
    currentbucketnodeptr = findInTrafficMatrix(newentryipaddresskey, newentrysourceipaddress,
      newentrydestipaddress);
    if(currentbucketnodeptr != NULL && (trafficMatrixTestStringEquality(currentbucketnodeptr
      ->entry->sourceaddressvalue,
      *newentrysourceipaddress) == TRUE) && trafficMatrixTestStringEquality(
        currentbucketnodeptr->entry->destinationaddressvalue, *newentrydestipaddress) == TRUE)
    {
      currentbucketnodeptr->entry->count++;
      currentbucketnodeptr->entry->datavol += newentrydatavol;
    }
    else if(currentbucketnodeptr != NULL)
    {
      MatrixListNode **currentbucketnodeptrbuffer = (MatrixListNode **)trafficMatrixSafeMalloc(
        POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewTrafficMatrixEntry(newentryipaddresskey, newentrysourceipaddress,
        newentrydestipaddress, newentrydatavol, currentbucketnodeptrbuffer);
      trafficmatrixsize++;
    }
  }
}

unsigned int trafficMatrixHashCode(char **sourceipaddresstohash, char **destipaddresstohash)
{
  unsigned int hashcode = (unsigned int)(*sourceipaddresstohash[INTINITIALIZER])
    +(unsigned int)(*sourceipaddresstohash[OFFSETVALUE])
    +(unsigned int)(*destipaddresstohash[INTINITIALIZER])
    +(unsigned int)(*destipaddresstohash[OFFSETVALUE]);
  return hashcode;
}

void initializeTrafficMatrix()
{
  trafficmatrix.tableentrylists = trafficMatrixSafeMalloc(DEFAULTTRAFFICMATRIXSIZE*sizeof(
    MatrixList)*POINTERSIZE);
  //clear the array
  int i;
  for(i=INTINITIALIZER; i<DEFAULTTRAFFICMATRIXSIZE; i++)
  {
    trafficmatrix.tableentrylists[i] = *(MatrixList *)trafficMatrixSafeMalloc(sizeof(
      MatrixList));
    trafficmatrix.tableentrylists[i].head = NULL;
  }
  trafficmatrixsize = INTINITIALIZER;
  trafficmatrixcapacity = DEFAULTTRAFFICMATRIXSIZE;
}

void *trafficMatrixSafeMalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (EXIT_ERRORCODE);
    }
    memset (p,FALSE,sz);
    return (p);
}
