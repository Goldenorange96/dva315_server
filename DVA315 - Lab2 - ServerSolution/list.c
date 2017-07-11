#include "List.h"
#include <stdio.h>
#include <stdlib.h>




List* Create_List(){
	List *list;
		list = (List*)malloc(sizeof(List));
		list->Size= 0;
		list->Head = NULL;
		if (list != NULL && list->Size==0 && list->Head==NULL) {
			printf("List has been created\n");
				return list;
		}
		return NULL;
}




void Add_first_Item(List *list, int Input) {
	Node *New;
	New = (Node*)malloc(sizeof(Node));
	if (New == NULL) {
		printf("Could not allocate memory");
	}
		
	else {
		New->Body = NULL;
		New->Data = Input;
		list->Head = New;
		list->Size = 1;
	}
}



void Add_Item_Last(List *list, planet_type input) {
	planet_type *it;
	it = list->Head;
	planet_type *Iterator = (planet_type*)malloc(sizeof(planet_type));
	Iterator->life = input.life;
	Iterator->mass = input.mass;
	strcpy(Iterator->name, input.name);
	Iterator->next = NULL;
	strcpy(Iterator->pid, input.pid);
	Iterator->sx = input.sx;
	Iterator->sy = input.sy;
	Iterator->vx = input.vx;
	Iterator->vy = input.vy;

	if (list->Head == NULL) {
		list->Head = Iterator;
		list->Size++;
	}
	
	else {
		while (it->next != NULL) {
			it = it->next;
		}
		it->next = Iterator;
		list->Size++;
		return;
	}
}

void Print_List(List *list) {
	int count = 1;
	Node *Iterator;
	Iterator = list->Head;
	if (list == NULL) {
		printf("List does not exist, try creating a new one first!\n");
	}
	else {
		while (Iterator != NULL) {
			printf("Item number:%d\nItem data:%d\n\n", count, Iterator->Data);
			Iterator = Iterator->Body;
			count++;
		}
		printf("Current amout of items in list:%d\n", list->Size);
	}
}

void Search(List *list, int input) {
	Node *Iterator; int count = 1;
	if (list->Head == NULL) {
		printf("There is no data to search for! Create some items!\n");
	}
	else {
		Iterator = list->Head;
		while (input != Iterator->Data && Iterator->Body != NULL) {
			Iterator = Iterator->Body;
			count++;
		}
		if (input == Iterator->Data) {
			printf("The data exists in item number:%d\n", count);
			
		}
		else {
			printf("The data does not exist!\n");
		}
	}
}


void Add_Between_ID(List* list, int input, int ID) {
	Node *Iterator_Left, *New, *Iterator_Right;
	int i;
	Iterator_Left = list->Head;
	for (i = 1; i < ID; i++) {
		Iterator_Left = Iterator_Left->Body;
		Iterator_Right = Iterator_Left;
		i++;
	}
	New = (Node*)malloc(sizeof(Node));
	if (New == NULL) {
		printf("Could not allocate memory for new Item");
	}
	
	else {
		Iterator_Right = Iterator_Left->Body;
		New->Data = input;
		Iterator_Left->Body = New;
		New->Body = Iterator_Right;
		list->Size++;
		printf("Node succesfully added!\n");
	}
}

List *Delete_List(List* list) {
	Node *Iterator, *Deleted;
	Iterator = list->Head;
	while(Iterator != NULL) {
		Deleted = Iterator;
		Iterator = Iterator->Body;
		free(Deleted);
	}
	free(Iterator);
	free(list);
	list = NULL;
	return list;
}


void Destroy_Item(List *list, char * PID, char * Name) {
	planet_type *Iterator_Left, *Iterator_Right, *Temp;
	int i;
	Iterator_Left = list->Head;
	Iterator_Right = Iterator_Left;



	while (strcmp(Iterator_Left->name, Name) != 0 && strcmp(Iterator_Left->pid, PID) != 0) {
		Iterator_Left = Iterator_Left->next;
		Iterator_Right = Iterator_Left;
	}
	if (strcmp(Iterator_Left->name, Name) == 0 && strcmp(Iterator_Left->pid, PID) == 0 && Iterator_Left->next == NULL) {
		if (list->Size == 1) {
			Temp = Iterator_Left;
			free(Temp);
			list->Head = NULL;
			list->Size = 0;
		}
		else {
			Temp = Iterator_Left;
			free(Temp);
			list->Size--;
		}
	}

	else if (strcmp(Iterator_Left->name, Name) == 0 && strcmp(Iterator_Left->pid, PID) == 0 && Iterator_Left->next->next == NULL) {
		Iterator_Right = Iterator_Left->next;
		Iterator_Left->next = NULL;
		Temp = Iterator_Right;
		free(Temp);
		list->Size--;
	}

	else if (strcmp(list->Head->pid, PID) == 0 && strcmp(list->Head->name, Name) == 0 && list->Size > 1) {
		Iterator_Right = Iterator_Left->next;
		Temp = Iterator_Left;
		list->Head = Iterator_Right;
		free(Temp);
		list->Size--;
	}

	else {
		Iterator_Right = Iterator_Left->next;
		Iterator_Right = Iterator_Right->next;
		Temp = Iterator_Left->next;
		Iterator_Left->next = Iterator_Right;
		free(Temp);
		list->Size--;
	}

}

planet_type * GetPlanet(List * list, char * ID, char* pName) {
	planet_type *Iterator;
	int cmp1 = 0, cmp2 = 0;
	Iterator = list->Head;
	while (Iterator != NULL) {
		cmp1 = strcmp(Iterator->pid, ID);
		cmp2 = strcmp(Iterator->name, pName);
		if (cmp1 == 0 && cmp2 == 0) {
			return Iterator;
		}
		Iterator = Iterator->next;
	}
	
}


int checker(char input[], char check[])
{
	int i, result = 1;
	for (i = 0; input[i] != '\0' && check[i] != '\0'; i++) {
		if (input[i] != check[i]) {
			result = 0;
			break;
		}
	}
	return result;
}