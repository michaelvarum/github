#include "linkedList.h"
#include <cstdlib>

void initList(linkedList* list)
{
	list->head = nullptr;
}

void addToBeginning(linkedList* list , unsigned int newValue)
{
    link* newLink = (link*)malloc(sizeof(link));
    newLink->value = newValue;
    newLink->next = list->head;
    list->head = newLink;
}

int removeHead(linkedList* list)
{
	// Throwing an exception can be added if list->head == NULL, depending on the specific requirements.
    link* temp = list->head;
    int value = temp->value;
    list->head = list->head->next;
    free(temp);
    return value;
}

void deleteLink(link* l)
{
	//Your code here
}
 
void cleanList(linkedList* list)
{
    link* current = list->head;
    link* next;
    
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    
    list->head = NULL;
}