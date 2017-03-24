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
    originatoripaddress, responderipaddress, originatorport, responderport);
  //if the list DNE, it's not there
  if(connectionhashtable.tableentrylists[connectionhashtablesearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ConnectionHashtableListNode *currentbucketnodeptr = (
    connectionhashtable.tableentrylists[connectionhashtablesearchkey].head);
  while(currentbucketnodeptr->next != NULL &&
    ((ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->orig_ip,
      *sourceipaddresssearchvalue) == FALSE)
    || (ConnectionHashtableTestStringEquality(currentbucketnodeptr->entry->resp_ip,
      *destipaddresssearchvalue) == FALSE)
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

//START AGAIN HERE, EVERYTHING ABOVE SHOULD THEORETICALLY BE GOOD
void initializeConnectionHashtableList(int listfirstentryipaddresskey,
  char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress,
  int listfirstentrydatavol)
{
  ConnectionHashtableListNode **headbuffer = (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(POINTERSIZE);
  headbuffer = &(connectionhashtable.tableentrylists[listfirstentryipaddresskey].head);
  initializeNewConnectionHashtableEntry(listfirstentryipaddresskey,
    listfirstentrysourceipaddress, listfirstentrydestipaddress,
    listfirstentrydatavol, headbuffer);
}

void initializeNewConnectionHashtableEntry(int newentryipaddresskey,
  char **newentrysourceipaddress, char **newentrydestipaddress,
  int newentrydatavol, ConnectionHashtableListNode **newnode)
{
  *newnode = (ConnectionHashtableListNode *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListNode));
  (*newnode)->entry = (ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry *));
  *((*newnode)->entry) = *(ConnectionHashtableListEntry *)ConnectionHashtableSafeMalloc(sizeof(ConnectionHashtableListEntry));
  (*newnode)->entry->ipaddresskey = newentryipaddresskey;
  (*newnode)->entry->orig_key = *newentrysourceipaddress;
  (*newnode)->entry->resp_key = *newentrydestipaddress;
  (*newnode)->entry->count = ENTRYCOUNTINITIALIZER;
  (*newnode)->entry->datavol = newentrydatavol;
  (*newnode)->next = NULL;
}

void insertInConnectionHashtable(char **newentrysourceipaddress,
  char **newentrydestipaddress, int newentrydatavol)
{
  unsigned int newentryipaddresskey = ConnectionHashtableHashCode(newentrysourceipaddress,
    newentrydestipaddress)%(unsigned int)connectionhashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(connectionhashtable.tableentrylists[newentryipaddresskey].head == NULL)
  {
    initializeConnectionHashtableList(newentryipaddresskey, newentrysourceipaddress,
      newentrydestipaddress, newentrydatavol);
    connectionhashtablesize++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ConnectionHashtableListNode *currentbucketnodeptr = (connectionhashtable.tableentrylists[newentryipaddresskey]
      .head);
    currentbucketnodeptr = findInConnectionHashtable(newentryipaddresskey, newentrysourceipaddress,
      newentrydestipaddress);
    if(currentbucketnodeptr != NULL && (ConnectionHashtableTestStringEquality(currentbucketnodeptr
      ->entry->orig_key,
      *newentrysourceipaddress) == TRUE) && ConnectionHashtableTestStringEquality(
        currentbucketnodeptr->entry->resp_key, *newentrydestipaddress) == TRUE)
    {
      currentbucketnodeptr->entry->count++;
      currentbucketnodeptr->entry->datavol += newentrydatavol;
    }
    else if(currentbucketnodeptr != NULL)
    {
      ConnectionHashtableListNode **currentbucketnodeptrbuffer = (ConnectionHashtableListNode **)ConnectionHashtableSafeMalloc(
        POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewConnectionHashtableEntry(newentryipaddresskey, newentrysourceipaddress,
        newentrydestipaddress, newentrydatavol, currentbucketnodeptrbuffer);
      connectionhashtablesize++;
    }
  }
}

unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport)
{
  unsigned int hashcode = (unsigned int)originatoripaddress[0]+originatorport+
    (unsigned int)responderipaddress+responderport;
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
