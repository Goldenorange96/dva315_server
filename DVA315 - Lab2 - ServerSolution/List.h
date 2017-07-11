#pragma once
#include "wrapper.h"
#ifndef List_H
#define List_H

typedef struct List {
	planet_type *Head;
	int Size;
}List;

typedef struct Node {
	struct Node *Body;
	int Data;
}Node;


/*Create List will 
1) Allocate memory for a list
2 Set Items to 0
3 Set Head to NULL

pre- There is no list
post- List has now been created
*/

List* Create_List();

/*Creates the first item
1) Allocate memeory for a node
2) Point List Head to Node Body
3) Set the data to user input 
4) Increase Items with 1
5) Set Node Body to NULL

pre - There must be a list, if not then the function won't be called
post- First item has now been created and is in hte list
*/
void Add_first_Item(List *list, int Input);

/*Create more items and add it to the end of the list
1) Allocate memory for a node
2) Iterate through the list
3) Set the data in the new node to user input
4) Point the former node to the next one
5) Increase Items in List with 1
6) Set The new node body to NULL

pre - there should be a list and either one or NO items
post - Item will be added at the end of the list
*/
void Add_Item_Last(List *list, planet_type input);



/*Delete current list
1) Make the list pointer point to the next node
2) Free the memory of the node that list USED to point to
3) repeat until all nodes are dead
4) free the list

pre - There must be a list
post - List will have been deleted
*/

List *Delete_List(List *list);

/*Prints the current list and all nodes in it
1)Set a temporary node pointer to point where list is pointed
2)Iterate through the list
3)print out the Identifier and Data

pre - list with items must existed
post - Either the list will print an error or print the list
*/
void Print_List(List* list);

/*Searches for the node the user asks for
1)Set a temporary node pointer to point where list is pointed
2)Iterate through the list
3)Check every node for the data
4)Print out the data

pre - Items must exist in the list
post - Item and position is printed out for the user
*/
void Search(List* list, int input);

/*Will add an item between the items;
1) Iterate through the list and check where ID is
2) Allocate memory for an Item
3) Point the current items Body (from the iterator) to the newly created Item
4) Point the new items Body to the node after it

pre - List and items must exist
post - Item is added succesfully
*/
void Add_Between_ID(List* list, int input, int ID);


/*Will delete the selected node
1) Iterate through the list and check where the item is ( probably by number, example number 4)
2)  make a temporary item which will point at the item that is supposed to be deleted
3) Point the former item to the item AFTER the deleted item
4) Delete the temp node, effectively destroying it.


pre - List must exist and have items
post - Selected node will be deleted
*/

void Destroy_Item(List* list, int ID);


planet_type* GetPlanet(List * list, char * ID, char* pName);

int checker(char input[], char check[]);

#endif