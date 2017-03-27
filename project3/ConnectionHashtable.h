/*
  Brennan McFarland
  bfm21
  ConnectionHashtable.h
  3/3/17
  The header for ConnectionHashtable.c
*/

#include "McFarlandNetworks.h"

#define DEFAULTCONNECTIONHASHTABLESIZE 16
#define CONNECTIONHASHTABLEGROWTHFACTOR 2
#define ENTRYCOUNTINITIALIZER 1

typedef struct ConnectionHashtableListEntry
{
  char *orig_ip;
  char *resp_ip;
  unsigned int orig_port;
  unsigned int resp_port;
  int secsinceepoch_start;
  int msecsincesec_start;
  int secsinceepoch_end;
  int msecsincesec_end;
  //deprecated, remove these once it's all transferred over to the new stuff
  int count;
  int datavol;
} ConnectionHashtableListEntry;

typedef struct ConnectionHashtableListNode
{
  ConnectionHashtableListEntry *entry;
  struct ConnectionHashtableListNode *next;
} ConnectionHashtableListNode;

typedef struct ConnectionHashtableList
{
  ConnectionHashtableListNode *head;
} ConnectionHashtableList;

typedef struct ConnectionHashtable
{
  ConnectionHashtableList *tableentrylists;
} ConnectionHashtable;

void printConnectionHashtableDebug();
ConnectionHashtableListNode *findInConnectionHashtable(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport);
int ConnectionHashtableTestStringEquality(char *string1, char *string2);
void initializeConnectionHashtableList(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch_start, int msecsincesec_start);
void initializeNewConnectionHashtableEntry(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport,
  int secsinceepoch_start, int msecsincesec_start, ConnectionHashtableListNode **newnode);
void insertInConnectionHashtable(char **originatoripaddress, char **responderipaddress,
  unsigned int originatorport, unsigned int responderport, int secsinceepoch_start,
  int msecsincesec_start);
unsigned int ConnectionHashtableHashCode(char **originatoripaddress,
  char **responderipaddress, unsigned int originatorport, unsigned int responderport);
void initializeConnectionHashtable();
void *ConnectionHashtableSafeMalloc (unsigned int sz);
