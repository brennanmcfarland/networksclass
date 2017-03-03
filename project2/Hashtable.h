#define EMPTY 0
#define DEFAULTTABLESIZE 4
#define HASHTABLEGROWTHFACTOR 2

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

void printTable();
//void growTable();
ListNode *find(int entrysearchkey, int entrysearchvalue);
void initializeTableList(int listfirstentrykey, int listfirstentryvalue);
void initializeNewEntry(int newentrykey, int newentryvalue, ListNode **newnode);
void insert(int newentryvalue);
int hashCode(int counttohash);
void initializeTable();
void *hashTableSafeMalloc (unsigned int sz);
