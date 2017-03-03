#include "McFarlandNetworks.h"

#define DEFAULTTABLESIZE 64
#define HASHTABLEGROWTHFACTOR 2

typedef struct ListEntry
{
  u_int32_t key;
  u_int32_t value;
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

void printSourceIPTable();
void printDestIPTable();
//void growTable();
ListNode *findInSourceIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue);
ListNode *findInDestIPTable(u_int32_t entrysearchkey, u_int32_t entrysearchvalue);
void initializeSourceIPTableList(u_int32_t listfirstentrykey, u_int32_t listfirstentryvalue);
void initializeDestIPTableList(u_int32_t listfirstentrykey, u_int32_t listfirstentryvalue);
void initializeNewEntry(u_int32_t newentrykey, u_int32_t newentryvalue, ListNode **newnode);
void insertSourceIP(u_int32_t newentryvalue);
void insertDestIP(u_int32_t newentryvalue);
u_int32_t hashCode(u_int32_t counttohash);
void initializeTables();
void *hashTableSafeMalloc (unsigned int sz);
