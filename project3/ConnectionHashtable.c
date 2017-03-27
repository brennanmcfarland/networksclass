/*
  Brennan McFarland
  bfm21
  ConnectionHashtable.c
  3/3/17
  a hashtable for an ip trace file traffic matrix
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include </usr/include/netinet/ip.h>
#include </usr/include/netinet/udp.h> //for the udphdr struct
#include </usr/include/netinet/tcp.h> //for the tcphdr struct
#include "ConnectionHashtable.h"


ConnectionHashtable connectionhashtable;
int connectionhashtablesize = INTINITIALIZER;
int connectionhashtablecapacity = DEFAULTCONNECTIONHASHTABLESIZE;


//for debugging
void printConnectionHashtableDebug()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(connectionhashtable.tableentrylists); i++)
  {
    ConnectionHashtableListNode *currentbucketnodeptr = (connectionhashtable.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    free(currentbucketnodeptr);
    printf("\n");
  }
}

//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
ConnectionHashtableListNode *findInConnectionHashtable(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  unsigned int connectionhashtablesearchkey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //if the list DNE, it's not there
  if(connectionhashtable.tableentrylists[connectionhashtablesearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ConnectionHashtableListNode *currentbucketnodeptr = (
    connectionhashtable.tableentrylists[connectionhashtablesearchkey].head);
  while(currentbucketnodeptr->next != NULL &&
    ((ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->orig_ip,
      *originatoripaddress) == FALSE)
    || (ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->resp_ip,
      *responderipaddress) == FALSE)
    || (originatorport != currentbucketnodeptr->entry->orig_port)
    || (responderport != currentbucketnodeptr->entry->resp_port)
    ))
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

//returns whether 2 strings are equal
int ConnectionHashtableTestStringEquality(char *string1, char *string2)
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

void initializeConnectionHashtableList(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  /*
  THE LIST ISN'T BEING INITIALIZED FOR SOME REASON
  */
  unsigned int connectionlistfirstkey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //printf("creating new list with key %u\n", connectionlistfirstkey);
  ConnectionHashtableListNode **headbuffer = (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(POINTERSIZE);
  headbuffer = &(connectionhashtable.tableentrylists[connectionlistfirstkey].head);
  initializeNewConnectionHashtableEntry(originatoripaddress, responderipaddress,
    originatorport, responderport, headbuffer);
}
/*
void initializeTrafficMatrixList(int listfirstentryipaddresskey,
  char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress,
  int listfirstentrydatavol)
{
  MatrixListNode **headbuffer = (MatrixListNode **)trafficMatrixSafeMalloc(POINTERSIZE);
  headbuffer = &(trafficmatrix.tableentrylists[listfirstentryipaddresskey].head);
  initializeNewTrafficMatrixEntry(listfirstentryipaddresskey,
    listfirstentrysourceipaddress, listfirstentrydestipaddress,
    listfirstentrydatavol, headbuffer);
}*/

void initializeNewConnectionHashtableEntry(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  ConnectionHashtableListNode **newnode)
{
  *newnode = (ConnectionHashtableListNode *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListNode));
  (*newnode)->entry = (ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry *));
  *((*newnode)->entry) = *(ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry));
  (*newnode)->entry->orig_ip = *originatoripaddress;
  (*newnode)->entry->resp_ip = *responderipaddress;
  (*newnode)->entry->orig_port = originatorport;
  (*newnode)->entry->resp_port = responderport;
  (*newnode)->entry->count = 1;
  (*newnode)->next = NULL;
}

void insertInConnectionHashtable(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport)
{
  unsigned int newconnectiontableentrykey = ConnectionHashtableHashCode(
    originatoripaddress, responderipaddress, originatorport, responderport)
    %(unsigned int)connectionhashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(connectionhashtable.tableentrylists[newconnectiontableentrykey].head == NULL)
  {
    initializeConnectionHashtableList(originatoripaddress, responderipaddress,
      originatorport, responderport);
    connectionhashtablesize++;
    //printf("inserting in new bucket %d\n", newconnectiontableentrykey);
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ConnectionHashtableListNode *currentbucketnodeptr = (
      connectionhashtable.tableentrylists[newconnectiontableentrykey].head);
    currentbucketnodeptr = findInConnectionHashtable(originatoripaddress,
      responderipaddress, originatorport, responderport);
    if(currentbucketnodeptr != NULL && (ConnectionHashtableTestStringEquality(currentbucketnodeptr
      ->entry->orig_ip,*originatoripaddress) == TRUE)
      && (ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->resp_ip,
        *responderipaddress) == TRUE)
      && (currentbucketnodeptr->entry->orig_port == originatorport)
      && (currentbucketnodeptr->entry->resp_port == responderport))
    {
      currentbucketnodeptr->entry->count++;
      //printf("incrementing count of existing node in bucket %d\n", newconnectiontableentrykey);
    }
    else if(currentbucketnodeptr != NULL)
    {
      ConnectionHashtableListNode **currentbucketnodeptrbuffer = (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(
        POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewConnectionHashtableEntry(originatoripaddress, responderipaddress,
        originatorport, responderport, currentbucketnodeptrbuffer);
      connectionhashtablesize++;
      //printf("inserting in new node in bucket %d\n", newconnectiontableentrykey);
    }
  }
}

unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  unsigned int hashcode = (unsigned int)(*originatoripaddress)[0]+originatorport+
    (unsigned int)(*responderipaddress)[0]+responderport;
  return hashcode;
}

void initializeConnectionHashtable()
{
  connectionhashtable.tableentrylists = ConnectionHashtableSafeMalloc(DEFAULTCONNECTIONHASHTABLESIZE*sizeof(
    ConnectionHashtableList)*POINTERSIZE);
  //clear the array
  int i;
  for(i=INTINITIALIZER; i<DEFAULTCONNECTIONHASHTABLESIZE; i++)
  {
    connectionhashtable.tableentrylists[i] = *(ConnectionHashtableList *)ConnectionHashtableSafeMalloc(sizeof(
      ConnectionHashtableList));
    connectionhashtable.tableentrylists[i].head = NULL;
  }
  connectionhashtablesize = INTINITIALIZER;
  connectionhashtablecapacity = DEFAULTCONNECTIONHASHTABLESIZE;
}

void *ConnectionHashtableSafeMalloc (unsigned int sz)
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
