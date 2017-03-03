#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include "Hashtable.h"

#define POINTERSIZE sizeof(List *)

Hashtable hashtable;
int hashtablesize = 0;
int hashtablecapacity = DEFAULTTABLESIZE;

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
void printTable()
{
  int i;
  for(i=0; i<sizeof(hashtable.tableentrylists); i++)
  {
    ListNode *currentnode = (hashtable.tableentrylists[i].head);
    if(currentnode != NULL)
    {
      do
      {
        printf("%d",currentnode->entry->value);
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
ListNode *find(int entrysearchkey, int entrysearchvalue)
{
  //if the list DNE, it's not there
  if(hashtable.tableentrylists[entrysearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  ListNode *currentnode = (hashtable.tableentrylists[entrysearchkey].head);
  while(currentnode->next != NULL && currentnode->entry->value != entrysearchvalue)
    currentnode = currentnode->next;
  //if(currentnode->entry->value == entrysearchvalue)
  //  return currentnode;
  return currentnode;
}

void initializeTableList(int listfirstentrykey, int listfirstentryvalue)
{
  ListNode **headbuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
  headbuffer = &(hashtable.tableentrylists[listfirstentrykey].head);
  initializeNewEntry(listfirstentrykey, listfirstentryvalue, headbuffer);
}

//it's returning the memory as blank since it's in the function
void initializeNewEntry(int newentrykey, int newentryvalue, ListNode **newnode)
{
  *newnode = (ListNode *)hashTableSafeMalloc(sizeof(ListNode));
  //IT'S OVERWRITING THE LIST POINTER DATA BEFORE IT WITH THE KEY AND VALUE
  //changed entry to pointer, but that didn't seem to fix it, investigate
  (*newnode)->entry = (ListEntry *)hashTableSafeMalloc(sizeof(ListEntry *));
  *((*newnode)->entry) = *(ListEntry *)hashTableSafeMalloc(sizeof(ListEntry));
  (*newnode)->entry->key = newentrykey;
  (*newnode)->entry->value = newentryvalue;
  (*newnode)->entry->count = 1;
  (*newnode)->next = EMPTY;
}

void insert(int newentryvalue)
{
  int newentrykey = hashCode(newentryvalue)%hashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(hashtable.tableentrylists[newentrykey].head == NULL)
    initializeTableList(newentrykey, newentryvalue);
  else
  {
    //see if it's in the list, if so increment value, if not add it
    ListNode *currentnode = (hashtable.tableentrylists[newentrykey].head);
    currentnode = find(newentrykey, newentryvalue);
    if(currentnode != NULL && currentnode->entry->value == newentryvalue)
      currentnode->entry->count++;
    else if(currentnode != NULL) //it never gets to this point, why?  find must be coming back as null
    {
      ListNode **currentnodebuffer = (ListNode **)hashTableSafeMalloc(POINTERSIZE);
      currentnodebuffer = &(currentnode->next);
      initializeNewEntry(newentrykey, newentryvalue, currentnodebuffer); //if it's null, should already be handled above
    }
  }
}

int hashCode(int counttohash)
{
  return counttohash;
}

void initializeTable()
{
  //allocate and clear space for an array of pointers to list heads
  //ListNode temp[DEFAULTTABLESIZE] = hashTableSafeMalloc(sizeof(ListNode)*DEFAULTTABLESIZE*POINTERSIZE);

  //List newhashtablearray[DEFAULTTABLESIZE];
  hashtable.tableentrylists = hashTableSafeMalloc(DEFAULTTABLESIZE*sizeof(List)*POINTERSIZE);
  //clear the array
  int i;
  for(i=0; i<DEFAULTTABLESIZE; i++)
  {
    //List templist = newhashtablearray[i];
    //templist = *(List *)hashTableSafeMalloc(sizeof(List));
    //templist.head = NULL;
    //newhashtablearray[i] = templist;

    //newhashtablearray[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    //newhashtablearray[i].head = NULL;
    hashtable.tableentrylists[i] = *(List *)hashTableSafeMalloc(sizeof(List));
    hashtable.tableentrylists[i].head = NULL;
  }
  //hashtable.tableentrylists = newhashtablearray;
  hashtablesize = 0;
  hashtablecapacity = DEFAULTTABLESIZE;
}

void *hashTableSafeMalloc (unsigned int sz)
{
    void *p;
    if ((p = (void *)malloc (sz)) == NULL)
    {
        printf ("memory allocation failed, exiting ...\n");
        exit (1);
    }
    memset (p,EMPTY,sz);
    return (p);
}

int main()
{
  initializeTable();



  insert(3);
  insert(1);
  insert(4);
  insert(0);
  insert(5);
  insert(6);
  printTable();

  return 0;
}
