//double linked list to help manage jobs
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

typedef struct Node{
	void* data;
	struct Node *next;
	struct Node *prev;
} Node;

typedef struct List{
	Node* front;
	Node* back;
	int m_size;
} List;

static Node* create(void* data, Node* prev, Node* next)
{
	Node* node = malloc(sizeof(Node));
	node->data = data;
	node->prev = prev;
	node->next = next;
	return node;
}

//initializes list
void init_list(List* list)
{
	assert (list != NULL);
	list->front = list->back = NULL;
	list->m_size = 0;
}

//checks if list is empty
bool empty( List* list)
{
	assert(list != NULL);
	return (list->m_size == 0);
}

//adds node to front of list
void add_front(List* list, void* data)
{
	assert(list != NULL);
	if (empty(list))
	{
		list->front = list->back = create(data, NULL, NULL);
		list->m_size = 1;
	}
	else
	{
		Node *nf = create(data, list->front, NULL);
		list->front->next = nf;
		list->front = nf;
		list->m_size++;
	}
}

//removes node from list
void* delete_node(List* list, Node* node, void(*destructor)(void*))
{
	assert(list != NULL);
	assert(node != NULL);
	assert(!empty(list));

	void* node_data = node->data;
	if(destructor)
	{
		destructor(node->data);
		node_data=NULL;
	}
	if(node->prev)
	{
		node->prev->next = node->next;
	}
	else
	{
		list->back=node->next;
	}
	if(node->next)
	{
		node->next->prev = node->prev;
	}
	else
	{
		list->front = node->prev;
	}
	free(node);
	list->m_size--;
	return(node_data);
}

void* remove_back(List* list, void(*destructor)(void*))
{
	return delete_node(list, list->back, destructor);
}

void* peek(Node* node)
{
	assert(node != NULL);
	return(node->data);
}

void* peek_front(List* list)
{
	assert(list != NULL);
	assert(!empty(list));
	return(peek(list->front));
}

void* peek_back(List* list)
{
	assert(list != NULL);
	assert(!empty(list));
	return(peek(list->back));

void deconstruct(List *list, void(*destructor)(void*))
{
	assert(list != NULL);
	while(!empty(list))
	{
		remove_back(list, destructor);
	}
}

#endif
