/*
  Brennan McFarland
  bfm21
  McFarlandProject2.h
  3/3/17
  holds 2 hashtables for ip address comparison
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include "ConnectionHashtable.h"
#include "McFarlandNetworks.h"


Hashtable connections;
int numconnections = INTINITIALIZER;
int maxconnections = DEFAULTHASHTABLESIZE;


//for debugging
void printConnections()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(connections.tableentrylists); i++)
  {
    ListNode *currentbucketnodeptr = (connections.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
        printf("%d|",currentbucketnodeptr->entry->value);
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    free(currentbucketnodeptr);
    printf("\n");
  }
}

//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
ListNode *findConnection(int entrysearchkey, int entrysearchvalue)
{
  //if the list DNE, it's not there
  if(connections.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentbucketnodeptr = (connections.tableentrylists[entrysearchkey].head);
  while(currentbucketnodeptr->next != NULL && currentbucketnodeptr->entry->value
    != entrysearchvalue)
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

void initializeConnectionsList(int listfirstentrykey, int listfirstentryvalue)
{
  ListNode **headbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
  headbuffer = &(connections.tableentrylists[listfirstentrykey].head);
  initializeNewEntry(listfirstentrykey, listfirstentryvalue, headbuffer);
}

void initializeNewEntry(int newentrykey, int newentryvalue, ListNode **newnode)
{
  *newnode = (ListNode *)hashTableSafeMalloc(sizeof(ListNode));
  (*newnode)->entry = (ListEntry *)hashTableSafeMalloc(sizeof(ListEntry *));
  *((*newnode)->entry) = *(ListEntry *)hashTableSafeMalloc(sizeof(ListEntry));
  (*newnode)->entry->key = newentrykey;
  (*newnode)->entry->value = newentryvalue;
  (*newnode)->entry->count = ENTRYCOUNTINITIALIZER;
  (*newnode)->next = NULL;
}

void insertConnection(int newentryvalue)
{
  int newentrykey = hashCode(newentryvalue)%maxconnections;
  //if it already exists in the list, increment the # occurences, if not create
  if(connections.tableentrylists[newentrykey].head == NULL)
  {
    initializeConnectionsList(newentrykey, newentryvalue);
    numconnections++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ListNode *currentbucketnodeptr = (connections.tableentrylists[newentrykey].head);
    currentbucketnodeptr = findConnection(newentrykey, newentryvalue);
    if(currentbucketnodeptr != NULL && currentbucketnodeptr->entry->value == newentryvalue)
      currentbucketnodeptr->entry->count++;
    else if(currentbucketnodeptr != NULL)
    {
      ListNode **currentbucketnodeptrbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewEntry(newentrykey, newentryvalue, currentbucketnodeptrbuffer);
      numconnections++;
    }
  }
}

int hashCode(int counttohash)
{
  return counttohash;
}

void initializeConnections()
{
  connections.tableentrylists = hashTableSafeMalloc(DEFAULTHASHTABLESIZE
    *sizeof(List)*POINTERSIZE);
  //clear the array
  int i;
  for(i=INTINITIALIZER; i<DEFAULTHASHTABLESIZE; i++)
  {
    connections.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    connections.tableentrylists[i].head = NULL;
  }
  numconnections = INTINITIALIZER;
  maxconnections = DEFAULTHASHTABLESIZE;
}

void *hashTableSafeMalloc (unsigned int sz)
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
