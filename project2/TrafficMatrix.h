#include "McFarlandNetworks.h"

#define DEFAULTTRAFFICMATRIXSIZE 16
#define TRAFFICMATRIXGROWTHFACTOR 2

typedef struct MatrixListEntry
{
  int ipaddresskey;
  char *sourceaddressvalue;
  char *destinationaddressvalue;
  int count;
  int datavol;
} MatrixListEntry;

typedef struct MatrixListNode
{
  MatrixListEntry *entry;
  struct MatrixListNode *next;
} MatrixListNode;

typedef struct MatrixList
{
  MatrixListNode *head;
} MatrixList;

typedef struct TrafficMatrix
{
  MatrixList *tableentrylists;
} TrafficMatrix;

void printTrafficMatrixDebug();
MatrixListNode *findInTrafficMatrix(int entryipaddresssearchkey, char **sourceipaddresssearchvalue, char **destipaddresssearchvalue);
int trafficMatrixTestStringEquality(char *string1, char *string2);
void initializeTrafficMatrixList(int listfirstentryipaddresskey, char **listfirstentrysourceipaddress, char **listfirstentrydestipaddress, int listfirstentrydatavol);
void initializeNewTrafficMatrixEntry(int newentryipaddresskey, char **newentrysourceipaddress, char **newentrydestipaddress, int newentrydatavol, MatrixListNode **newnode);
void insertInTrafficMatrix(char **newentrysourceipaddress, char **newentrydestipaddress, int newentrydatavol);
unsigned int trafficMatrixHashCode(char **sourceipaddresstohash, char **destipaddresstohash);
void initializeTrafficMatrix();
void *trafficMatrixSafeMalloc (unsigned int sz);
