#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/* a link that contains a positive integer value */
typedef struct link
{
	unsigned int value;
    struct link* next;
} link;

typedef struct linkedList
{
	link* head;
} linkedList;

void initList(linkedList* list); // initialize list
void addToBeginning(linkedList* list, unsigned int newValue); // add new link in the beginning of list with newValue in it
int removeHead(linkedList* list); // return the value from the first link in list, and change the first link to head->next. return -1 if list is empty
void cleanList(linkedList* list); // delete all links from memory

#endif // LINKEDKIST_H