#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include </usr/include/netinet/ip.h> //for u_int32_t
#include "IPHashtable.h"


Hashtable sourceiphashtable;
Hashtable destiphashtable;
int sourceiphashtablesize = 0;
int destiphashtablesize = 0;
int sourceiphashtablecapacity = DEFAULTTABLESIZE;
int destiphashtablecapacity = DEFAULTTABLESIZE;

//notes commented out, delete this function
void tempNotes()
{
/*
  IMPLEMENT GENERIC HASHTABLE HERE TO USE IN THIS AND FUTURE PROJECTS
  JUST include
  FOLLOW TEXTBOOK EXAMPLE
*/
/*
#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE];

SEPARATE CHAINING
struct nlist {
struct nlist *next;
char *name;
char *defn;
};

FORM HASH count FOR STRING
unsigned hash(char *s)
{
unsigned hashval;
for (hashval = 0; *s != '\0'; s++)
hashval = *s + 31 * hashval;129
return hashval % HASHSIZE;
}

struct nlist *lookup(char *);
char *strdup(char *);
struct nlist *install(char *name, char *defn)
{
struct nlist *np;
unsigned hashval;
if ((np = lookup(name)) == NULL) {
np = (struct nlist *) malloc(sizeof(*np));
if (np == NULL || (np->name = strdup(name)) == NULL)
return NULL;
hashval = hash(name);
np->next = hashtab[hashval];
hashtab[hashval] = np;
} else
free((void *) np->defn);
if ((np->defn = strdup(defn)) == NULL)
return NULL;
return np;
}

*/
}

//for debugging
void printSourceIPTable()
{
  int i;
  for(i=0; i<sizeof(sourceiphashtable.tableentrylists); i++)
  {
    ListNode *currentnode = (sourceiphashtable.tableentrylists[i].head);
    if(currentnode != NULL)
    {
      do
      {
        printf("%d|",currentnode->entry->value);
      }while((currentnode = currentnode->next) != NULL);
    }
    printf("\n");
  }
}

//for debugging
void printDestIPTable()
{
  int i;
  for(i=0; i<sizeof(destiphashtable.tableentrylists); i++)
  {
    ListNode *currentnode = (destiphashtable.tableentrylists[i].head);
    if(currentnode != NULL)
    {
      do
      {
        printf("%d|",currentnode->entry->value);
      }while((currentnode = currentnode->next) != NULL);
    }
    printf("\n");
  }
}

/*
void growTable()
{
    //copy the old table and make a new one to put the rehashed entries in
    List *oldhashtablearray = hashtable.tableentrylists;
    List newhashtablearray[sizeof(hashtable.tableentrylists)*HASHTABLEGROWTHFACTOR];
    //clear the array
    int i;
    for(i=0; i<sizeof(*oldhashtablearray)*POINTERSIZE; i++)
    {
      newhashtablearray[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    }
    for(i=sizeof(*oldhashtablearray)*POINTERSIZE; i<sizeof(newhashtablearray)*POINTERSIZE; i++)
    {
      List templist = newhashtablearray[i];
      templist = *(List *)hashTableSafeMalloc(sizeof(List));
      templist.head = NULL;
    }
    hashtable.tableentrylists = newhashtablearray;
    hashtablecapacity = hashtablecapacity*HASHTABLEGROWTHFACTOR;
    //and copy the elements over, rehashing as we go
    for(i=0; i<sizeof(hashtable.tableentrylists); i++)
    {
      //does this work if no head?
      ListNode *currentnode = (oldhashtablearray[i].head);
      while(currentnode->next != NULL)
      {
        insert(currentnode->entry->value);
        currentnode = currentnode->next;
      }
    }
}
*/

//if found, return pointer to node, if not and list exists, return pointer to
//last element in list, if list dne, return null
ListNode *findInSourceIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue)
{
  //if the list DNE, it's not there
  if(sourceiphashtable.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentnode = (sourceiphashtable.tableentrylists[entrysearchkey].head);
  while(currentnode->next != NULL && currentnode->entry->value != entrysearchvalue)
    currentnode = currentnode->next;
  return currentnode;
}

ListNode *findInDestIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue)
{
  //if the list DNE, it's not there
  if(destiphashtable.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentnode = (destiphashtable.tableentrylists[entrysearchkey].head);
  while(currentnode->next != NULL && currentnode->entry->value != entrysearchvalue)
    currentnode = currentnode->next;
  return currentnode;
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

//it's returning the memory as blank since it's in the function
void initializeNewEntry(u_int32_t newentrykey, u_int32_t newentryvalue, ListNode **newnode)
{
  *newnode = (ListNode *)hashTableSafeMalloc(sizeof(ListNode));
  //IT'S OVERWRITING THE LIST POINTER DATA BEFORE IT WITH THE KEY AND VALUE
  //changed entry to pointer, but that didn't seem to fix it, investigate
  (*newnode)->entry = (ListEntry *)hashTableSafeMalloc(sizeof(ListEntry *));
  *((*newnode)->entry) = *(ListEntry *)hashTableSafeMalloc(sizeof(ListEntry));
  (*newnode)->entry->key = newentrykey;
  (*newnode)->entry->value = newentryvalue;
  (*newnode)->entry->count = 1;
  (*newnode)->next = NULL;
}

void insertSourceIP(u_int32_t newentryvalue)
{
  u_int32_t newentrykey = hashCode(newentryvalue)%sourceiphashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(sourceiphashtable.tableentrylists[newentrykey].head == NULL)
  {
    initializeSourceIPTableList(newentrykey, newentryvalue);
    sourceiphashtablesize++;
  }
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ListNode *currentnode = (sourceiphashtable.tableentrylists[newentrykey].head);
    currentnode = findInSourceIPTable(newentrykey, newentryvalue);
    if(currentnode != NULL && currentnode->entry->value == newentryvalue)
      currentnode->entry->count++;
    else if(currentnode != NULL) //it never gets to this point, why?  find must be coming back as null
    {
      ListNode **currentnodebuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentnodebuffer = &(currentnode->next);
      initializeNewEntry(newentrykey, newentryvalue, currentnodebuffer); //if it's null, should already be handled above
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
    ListNode *currentnode = (destiphashtable.tableentrylists[newentrykey].head);
    currentnode = findInDestIPTable(newentrykey, newentryvalue);
    if(currentnode != NULL && currentnode->entry->value == newentryvalue)
      currentnode->entry->count++;
    else if(currentnode != NULL) //it never gets to this point, why?  find must be coming back as null
    {
      ListNode **currentnodebuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentnodebuffer = &(currentnode->next);
      initializeNewEntry(newentrykey, newentryvalue, currentnodebuffer); //if it's null, should already be handled above
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
  sourceiphashtable.tableentrylists = hashTableSafeMalloc(DEFAULTTABLESIZE*sizeof(List)*POINTERSIZE);
  destiphashtable.tableentrylists = hashTableSafeMalloc(DEFAULTTABLESIZE*sizeof(List)*POINTERSIZE);
  //clear the array
  int i;
  for(i=0; i<DEFAULTTABLESIZE; i++)
  {
    sourceiphashtable.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    sourceiphashtable.tableentrylists[i].head = NULL;
    destiphashtable.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    destiphashtable.tableentrylists[i].head = NULL;
  }
  sourceiphashtablesize = 0;
  destiphashtablesize = 0;
  sourceiphashtablecapacity = DEFAULTTABLESIZE;
  destiphashtablecapacity = DEFAULTTABLESIZE;
}

void *hashTableSafeMalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (1);
    }
    memset (p,FALSE,sz);
    return (p);
}

/*int main()
{
  initializeTables();
  insertSourceIP(3);
  insertDestIP(1);
  insertSourceIP(4);
  insertSourceIP(0);
  insertDestIP(5);
  insertDestIP(6);
  printSourceIPTable();
  printDestIPTable();

  return 0;
}*/
