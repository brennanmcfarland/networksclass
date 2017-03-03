#include <stdlib.h>
#include <stdio.h>
#include <string.h> //for memset
#include </usr/include/netinet/ip.h>
#include "TrafficMatrix.h"


TrafficMatrix trafficmatrix;
int trafficmatrixsize = 0;
int trafficmatrixcapacity = DEFAULTTRAFFICMATRIXSIZE;


//for debugging
void printTrafficMatrixDebug()
{
  int i;
  for(i=0; i<sizeof(trafficmatrix.tableentrylists); i++)
  {
    MatrixListNode *currentnode = (trafficmatrix.tableentrylists[i].head);
    if(currentnode != NULL)
    {
      do
      {
        printf(" %s|%s ",currentnode->entry->sourceaddressvalue,
          currentnode->entry->destinationaddressvalue);
      }while((currentnode = currentnode->next) != NULL);
    }
    printf("\n");
  }
}


/*
void growTrafficMatrix()
{
    //copy the old table and make a new one to put the rehashed entries in
    List *oldhashtablearray = hashtable.tableentrylists;
    List newhashtablearray[sizeof(hashtable.tableentrylists)*TRAFFICMATRIXGROWTHFACTOR];
    //clear the array
    int i;
    for(i=0; i<sizeof(*oldhashtablearray)*POINTERSIZE; i++)
    {
      newhashtablearray[i] = *(List *)trafficMatrixSafeMalloc(sizeof(List));
    }
    for(i=sizeof(*oldhashtablearray)*POINTERSIZE; i<sizeof(newhashtablearray)*POINTERSIZE; i++)
    {
      List templist = newhashtablearray[i];
      templist = *(List *)trafficMatrixSafeMalloc(sizeof(List));
      templist.head = NULL;
    }
    hashtable.tableentrylists = newhashtablearray;
    hashtablecapacity = hashtablecapacity*TRAFFICMATRIXGROWTHFACTOR;
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
MatrixListNode *findInTrafficMatrix(int entryipaddresssearchkey, char **sourceipaddresssearchvalue, char **destipaddresssearchvalue)
{
  //if the list DNE, it's not there
  if(trafficmatrix.tableentrylists[entryipaddresssearchkey].head == NULL)
    return NULL;
  //see if it's in the list
  MatrixListNode *currentnode = (trafficmatrix.tableentrylists[entryipaddresssearchkey].head);
  while(currentnode->next != NULL &&
    ((currentnode->entry->sourceaddressvalue != *sourceipaddresssearchvalue)
    || currentnode->entry->destinationaddressvalue != *destipaddresssearchvalue))
    currentnode = currentnode->next;
  return currentnode;
}

//returns whether 2 strings are equal
int trafficMatrixTestStringEquality(char *string1, char *string2)
{
  if(sizeof(string1) != sizeof(string2))
  {
    return FALSE;
  }
  int i;
  for(i=0; i<sizeof(string1)-1; i++)
  {
    if(string1[i] != string2[i])
    {
      return FALSE;
    }
  }
  return TRUE;
}

void initializeTrafficMatrixList(int listfirstentryipaddresskey, char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress, int listfirstentrydatavol)
{
  MatrixListNode **headbuffer = (MatrixListNode **)trafficMatrixSafeMalloc(POINTERSIZE);
  headbuffer = &(trafficmatrix.tableentrylists[listfirstentryipaddresskey].head);
  initializeNewTrafficMatrixEntry(listfirstentryipaddresskey, listfirstentrysourceipaddress, listfirstentrydestipaddress, listfirstentrydatavol, headbuffer);
}

void initializeNewTrafficMatrixEntry(int newentryipaddresskey, char **newentrysourceipaddress, char **newentrydestipaddress, int newentrydatavol, MatrixListNode **newnode)
{
  *newnode = (MatrixListNode *)trafficMatrixSafeMalloc(sizeof(MatrixListNode));
  (*newnode)->entry = (MatrixListEntry *)trafficMatrixSafeMalloc(sizeof(MatrixListEntry *));
  *((*newnode)->entry) = *(MatrixListEntry *)trafficMatrixSafeMalloc(sizeof(MatrixListEntry));
  (*newnode)->entry->ipaddresskey = newentryipaddresskey;
  (*newnode)->entry->sourceaddressvalue = *newentrysourceipaddress;
  (*newnode)->entry->destinationaddressvalue = *newentrydestipaddress;
  (*newnode)->entry->count = 1;
  (*newnode)->entry->datavol = newentrydatavol;
  (*newnode)->next = NULL;
}

void insertInTrafficMatrix(char **newentrysourceipaddress, char **newentrydestipaddress, int newentrydatavol)
{
  unsigned int newentryipaddresskey = trafficMatrixHashCode(newentrysourceipaddress, newentrydestipaddress)%(unsigned int)trafficmatrixcapacity;
  //if it already exists in the list, increment the # occurences, if not create
  //first check if list DNE
  if(trafficmatrix.tableentrylists[newentryipaddresskey].head == NULL)
    initializeTrafficMatrixList(newentryipaddresskey, newentrysourceipaddress, newentrydestipaddress, newentrydatavol);
  else
  {
    //printf("key is %d\n", newentryipaddresskey);
    //see if it's in the list, if so increment value, if not add it
    MatrixListNode *currentnode = (trafficmatrix.tableentrylists[newentryipaddresskey].head);
    currentnode = findInTrafficMatrix(newentryipaddresskey, newentrysourceipaddress, newentrydestipaddress);
    //if(currentnode != NULL && currentnode->entry->ipaddressvalue == newentryipaddressvalue)
    if(currentnode != NULL && (trafficMatrixTestStringEquality(currentnode->entry->sourceaddressvalue,
      *newentrysourceipaddress) == TRUE) && trafficMatrixTestStringEquality(
        currentnode->entry->destinationaddressvalue, *newentrydestipaddress) == TRUE)
    {
      currentnode->entry->count++;
      currentnode->entry->datavol += newentrydatavol;
    }
    else if(currentnode != NULL) //it never gets to this point, why?  find must be coming back as null
    {
      MatrixListNode **currentnodebuffer = (MatrixListNode **)trafficMatrixSafeMalloc(POINTERSIZE);
      currentnodebuffer = &(currentnode->next);
      initializeNewTrafficMatrixEntry(newentryipaddresskey, newentrysourceipaddress, newentrydestipaddress, newentrydatavol, currentnodebuffer); //if it's null, should already be handled above
    }
  }
  trafficmatrixsize++;
}

unsigned int trafficMatrixHashCode(char **sourceipaddresstohash, char **destipaddresstohash)
{
  //printf("THING TO BE KEYED: %s", *ipaddresstohash);
  //printf("KEY: %d", (int)(*ipaddresstohash[0])+(int)(*ipaddresstohash[1])+(int)(*ipaddresstohash[2])+(int)(*ipaddresstohash[3]));
  unsigned int hashcode = (unsigned int)(*sourceipaddresstohash[0])
    +(unsigned int)(*sourceipaddresstohash[1])+(unsigned int)(*destipaddresstohash[0])
    +(unsigned int)(*destipaddresstohash[1]);
  return hashcode;
}

void initializeTrafficMatrix()
{
  trafficmatrix.tableentrylists = trafficMatrixSafeMalloc(DEFAULTTRAFFICMATRIXSIZE*sizeof(MatrixList)*POINTERSIZE);
  //clear the array
  int i;
  //why is the first value not clearing???
  for(i=0; i<DEFAULTTRAFFICMATRIXSIZE; i++)
  {
    trafficmatrix.tableentrylists[i] = *(MatrixList *)trafficMatrixSafeMalloc(sizeof(MatrixList));
    trafficmatrix.tableentrylists[i].head = NULL;
  }
  trafficmatrixsize = 0;
  trafficmatrixcapacity = DEFAULTTRAFFICMATRIXSIZE;
}

void *trafficMatrixSafeMalloc (unsigned int sz)
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

/*int main(int argc, char *argv[])
{
  initializeTrafficMatrix();

  char *temptest = "dsf3jd9";
  char **temptestptr = &temptest;
  char *temptest2 = "DDDD";
  char **temptest2ptr = &temptest2;
  char *temptest3 = "BBBB";
  char **temptest3ptr = &temptest3;
  char *temptest4 = "AAAA";
  char **temptest4ptr = &temptest4;
  char *temptest5 = "QQQQ";
  char **temptest5ptr = &temptest5;
  char *temptest6 = "ZZZZ";
  char **temptest6ptr = &temptest6;
  insertInTrafficMatrix(temptestptr, temptest2ptr);
  insertInTrafficMatrix(temptest2ptr, temptest3ptr);
  insertInTrafficMatrix(temptest3ptr, temptest2ptr);
  insertInTrafficMatrix(temptest4ptr, temptest4ptr);
  insertInTrafficMatrix(temptest5ptr, temptest6ptr);
  insertInTrafficMatrix(temptest6ptr, temptest4ptr);
  insertInTrafficMatrix(temptest6ptr, temptest4ptr);

  printTrafficMatrix();

  return 0;
}*/
