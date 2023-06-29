#include "linkedList.h"
#include "stack.h"
#include "utils.h"

#include <iostream>
#include <cstdlib>

using std::cout;
using std::endl;

int main()
{
	cout << "Testing linkedList:" << endl;
	linkedList* list = new linkedList;
	initList(list);
	addToBeginning(list, 1);
	addToBeginning(list, 4);
	addToBeginning(list, 9);
	cout << "the numbers are: 1 4 9" << endl;
	cout << "Your numbers in reverse order are:";
	for (int i = 0; i < 3; i++)
	{
		cout << " " << removeHead(list);
	}

	cout << endl;
	cleanList(list);
	delete list;

	cout << "Testing stack" << endl;
	stack* s = new stack;
	initStack(s);
	push(s, 2);
	push(s, 5);
	push(s, 7);
	cout << "the numbers are: 2 5 7" << endl;
	cout << "Your numbers in reverse order are:";
	for (int i = 0; i < 3; i++)
	{
		cout << " " << pop(s);
	}

	cout << endl;
	cleanStack(s);
	delete s;


	cout << "Testing reverse" << endl;
	int numbers[5];
	cout << "the numbers are:";
	for (int i = 0; i < 5; i++)
	{
		numbers[i] = i;
		cout << " " << i;
	}
	cout << endl;
	reverse(numbers,5);
	cout << "Your numbers in reverse order are:";
	for(int i = 0; i < 5; i++)
	{
		cout << " " <<numbers[i];
	}
	cout << endl;
	
	system("pause");
	return 0;
}