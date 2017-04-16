/*
  Brennan McFarland
  bfm21
  IPHashtable.h
  3/3/17
  The header for IPHashtable.c
*/


#define DEFAULTHASHTABLESIZE 64
#define HASHTABLEGROWTHFACTOR 2
#define ENTRYCOUNTINITIALIZER 1

typedef struct ListEntry
{
  int key;
  int value;
  int count;
} ListEntry;

typedef struct ListNode
{
  ListEntry *entry;
  struct ListNode *next;
} ListNode;

typedef struct List
{
  ListNode *head;
} List;

typedef struct Hashtable
{
  List *tableentrylists;
} Hashtable;

void printConnections();
ListNode *findConnection(int entrysearchkey, int entrysearchvalue);
void initializeConnectionsList(int listfirstentrykey, int listfirstentryvalue);
void initializeNewEntry(int newentrykey, int newentryvalue, ListNode **newnode);
void insertConnection(int newentryvalue);
int hashCode(int counttohash);
void initializeConnections();
void *hashTableSafeMalloc (unsigned int sz);
