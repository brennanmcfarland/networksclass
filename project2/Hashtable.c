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

/*
public void insert(String key){
  int hashCode = key.hashCode();
  //if it already exists in the list, increment the # occurences, if not, create it
  for(int i=0; i<table[Math.abs(hashCode)%table.length].size(); i++){
    if(table[Math.abs(hashCode)%table.length].get(i).key.equals(key)){
      table[Math.abs(hashCode)%table.length].get(i).addOccurence();
      return;
    }
  }
  */

//for debugging
void printTable()
{
  int i;
  for(i=0; i<sizeof(hashtable.tableentrylists); i++)
  {
    //does this work if no head?  the heads are filled with random data, not cleared
    ListNode *currentnode = (hashtable.tableentrylists[i].head);
    while(currentnode != NULL && currentnode->next != NULL)
    {
      printf("%d",currentnode->entry->value);
      currentnode = currentnode->next;
    }
  }
}

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

void insert(int newentryvalue)
{
  int newentrykey = hashCode(newentryvalue)%hashtablecapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(hashtable.tableentrylists[newentrykey].head == EMPTY)
  {
    printf("there's no existing list");
    //make it the first element in the list
    ListNode newnode = *(ListNode *)hashTableSafeMalloc(sizeof(ListNode));
    //IT'S OVERWRITING THE LIST POINTER DATA BEFORE IT WITH THE KEY AND VALUE
    //changed entry to pointer, but that didn't seem to fix it, investigate
    newnode.entry = (ListEntry *)hashTableSafeMalloc(sizeof(ListEntry));
    newnode.entry->key = newentrykey;
    newnode.entry->value = newentryvalue;
    newnode.next = EMPTY;
    hashtable.tableentrylists[newentrykey].head = &newnode;
  }
  else
  {
    printf("there's an existing list");
    //see if it's in the list
    ListNode *currentnode = (hashtable.tableentrylists[newentrykey].head);
    while(currentnode != NULL && currentnode->next != NULL && currentnode->entry->key != newentrykey)
      currentnode = currentnode->next;
    if(currentnode->entry->key == newentrykey)
      //we found it, increment that entry in the list
      currentnode->entry->value++;
    else
    {
      //it's not there, add it to the list
      ListNode newnode = *(ListNode *)hashTableSafeMalloc(sizeof(ListNode));
      newnode.entry->key = newentrykey;
      newnode.entry->value = newentryvalue;
      newnode.next = NULL;
      currentnode->next = &newnode;
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
  List newhashtablearray[DEFAULTTABLESIZE];
  //clear the array
  int i;
  for(i=0; i<DEFAULTTABLESIZE; i++)
  {
    List templist = newhashtablearray[i];
    templist = *(List *)hashTableSafeMalloc(sizeof(List));
    templist.head = NULL;
    newhashtablearray[i] = templist;
  }
  hashtable.tableentrylists = newhashtablearray;
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
  //insert(1);
  //insert(4);
  //insert(0);
  //insert(5);
  //insert(6); //causing a segfault.  why?
  printTable();

  return 0;
}
