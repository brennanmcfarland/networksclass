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
#include </usr/include/netinet/ip.h> //for u_int32_t
#include "IPHashtable.h"


Hashtable sourceiphashtable;
Hashtable destiphashtable;
int sourceiphashtablesize = INTINITIALIZER;
int destiphashtablesize = INTINITIALIZER;
int sourceiphashtablecapacity = DEFAULTIPHASHTABLESIZE;
int destiphashtablecapacity = DEFAULTIPHASHTABLESIZE;


//for debugging
void printSourceIPTable()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(sourceiphashtable.tableentrylists); i++)
  {
    ListNode *currentbucketnodeptr = (sourceiphashtable.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
        printf("%d|",currentbucketnodeptr->entry->value);
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    printf("\n");
  }
}

//for debugging
void printDestIPTable()
{
  int i;
  for(i=INTINITIALIZER; i<sizeof(destiphashtable.tableentrylists); i++)
  {
    ListNode *currentbucketnodeptr = (destiphashtable.tableentrylists[i].head);
    if(currentbucketnodeptr != NULL)
    {
      do
      {
        printf("%d|",currentbucketnodeptr->entry->value);
      }while((currentbucketnodeptr = currentbucketnodeptr->next) != NULL);
    }
    printf("\n");
  }
}


//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
ListNode *findInSourceIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue)
{
  //if the list DNE, it's not there
  if(sourceiphashtable.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentbucketnodeptr = (sourceiphashtable.tableentrylists[entrysearchkey].head);
  while(currentbucketnodeptr->next != NULL && currentbucketnodeptr->entry->value
    != entrysearchvalue)
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

ListNode *findInDestIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue)
{
  //if the list DNE, it's not there
  if(destiphashtable.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentbucketnodeptr = (destiphashtable.tableentrylists[entrysearchkey].head);
  while(currentbucketnodeptr->next != NULL && currentbucketnodeptr->entry->value
    != entrysearchvalue)
    currentbucketnodeptr = currentbucketnodeptr->next;
  return currentbucketnodeptr;
}

void initializeSourceIPTableList(u_int32_t listfirstentrykey, u_int32_t listfirstentryvalue)
{
  ListNode **headbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
  headbuffer = &(sourceiphashtable.tableentrylists[listfirstentrykey].head);
  initializeNewEntry(listfirstentrykey, listfirstentryvalue, headbuffer);
}

void initializeDestIPTableList(u_int32_t listfirstentrykey, u_int32_t listfirstentryvalue)
{
  ListNode **headbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
  headbuffer = &(destiphashtable.tableentrylists[listfirstentrykey].head);
  initializeNewEntry(listfirstentrykey, listfirstentryvalue, headbuffer);
}

void initializeNewEntry(u_int32_t newentrykey, u_int32_t newentryvalue, ListNode **newnode)
{
  *newnode = (ListNode *)hashTableSafeMalloc(sizeof(ListNode));
  (*newnode)->entry = (ListEntry *)hashTableSafeMalloc(sizeof(ListEntry *));
  *((*newnode)->entry) = *(ListEntry *)hashTableSafeMalloc(sizeof(ListEntry));
  (*newnode)->entry->key = newentrykey;
  (*newnode)->entry->value = newentryvalue;
  (*newnode)->entry->count = ENTRYCOUNTINITIALIZER;
  (*newnode)->next = NULL;
}

void insertSourceIP(u_int32_t newentryvalue)
{
  u_int32_t newentrykey = hashCode(newentryvalue)%sourceiphashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  if(sourceiphashtable.tableentrylists[newentrykey].head == NULL)
  {
    initializeSourceIPTableList(newentrykey, newentryvalue);
    sourceiphashtablesize++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ListNode *currentbucketnodeptr = (sourceiphashtable.tableentrylists[newentrykey].head);
    currentbucketnodeptr = findInSourceIPTable(newentrykey, newentryvalue);
    if(currentbucketnodeptr != NULL && currentbucketnodeptr->entry->value == newentryvalue)
      currentbucketnodeptr->entry->count++;
    else if(currentbucketnodeptr != NULL)
    {
      ListNode **currentbucketnodeptrbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewEntry(newentrykey, newentryvalue, currentbucketnodeptrbuffer);
      sourceiphashtablesize++;
    }
  }
}

void insertDestIP(u_int32_t newentryvalue)
{
  u_int32_t newentrykey = hashCode(newentryvalue)%destiphashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(destiphashtable.tableentrylists[newentrykey].head == NULL)
  {
    initializeDestIPTableList(newentrykey, newentryvalue);
    destiphashtablesize++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ListNode *currentbucketnodeptr = (destiphashtable.tableentrylists[newentrykey].head);
    currentbucketnodeptr = findInDestIPTable(newentrykey, newentryvalue);
    if(currentbucketnodeptr != NULL && currentbucketnodeptr->entry->value == newentryvalue)
      currentbucketnodeptr->entry->count++;
    else if(currentbucketnodeptr != NULL)
    {
      ListNode **currentbucketnodeptrbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentbucketnodeptrbuffer = &(currentbucketnodeptr->next);
      initializeNewEntry(newentrykey, newentryvalue, currentbucketnodeptrbuffer);
      destiphashtablesize++;
    }
  }
}

u_int32_t hashCode(u_int32_t counttohash)
{
  return counttohash;
}

void initializeTables()
{
  sourceiphashtable.tableentrylists = hashTableSafeMalloc(DEFAULTIPHASHTABLESIZE
    *sizeof(List)*POINTERSIZE);
  destiphashtable.tableentrylists = hashTableSafeMalloc(DEFAULTIPHASHTABLESIZE
    *sizeof(List)*POINTERSIZE);
  //clear the array
  int i;
  for(i=INTINITIALIZER; i<DEFAULTIPHASHTABLESIZE; i++)
  {
    sourceiphashtable.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    sourceiphashtable.tableentrylists[i].head = NULL;
    destiphashtable.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    destiphashtable.tableentrylists[i].head = NULL;
  }
  sourceiphashtablesize = INTINITIALIZER;
  destiphashtablesize = INTINITIALIZER;
  sourceiphashtablecapacity = DEFAULTIPHASHTABLESIZE;
  destiphashtablecapacity = DEFAULTIPHASHTABLESIZE;
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
