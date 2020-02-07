#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
	struct node* prev;
	void* element;
	struct node* next;
};

struct queue {
	int length;
	struct node* head;
	struct node* tail;
};

typedef struct queue* queue_t;

queue_t queue_create(void)
{
	queue_t new_queue;
	new_queue = (queue_t)malloc(sizeof(queue_t));
	if (new_queue == NULL) {
		return NULL;
	}
	new_queue->length = 0;
	new_queue->head = NULL;
	new_queue->tail = NULL;
	return new_queue;
}

int queue_destroy(queue_t queue)
{
	if (queue != NULL || queue_length(queue) > 0) {
		return -1;
	} 
	else {
		free(queue);
		return 0;
	}

}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	struct node* temp = (struct node*)malloc(sizeof(struct node));
	if (temp == NULL) {
		return -1;
	}
	temp->element = data;
	temp->next = NULL;
	temp->prev = NULL;
	if (queue->length == 0) {
		queue->head = temp;
		queue->tail = temp;
		queue->length = 1;
	}
	else {
		temp->next = queue->head;
		queue->head->prev = temp;
		queue->head = queue->head->prev;
		queue->length += 1;
	}

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	if (queue->length == 0) {
		return -1;
	}

	if (queue->length == 1) {
		*data = queue->tail->element;
		queue->head = NULL;
		free(queue->head);
		queue->tail = NULL;
		free(queue->tail);
		queue->length = 0;

		return 0;
	} 

	*data = queue->tail->element;
	queue->tail->prev->next = NULL;
	queue->tail = queue->tail->prev;
	free(queue->tail);
	queue->length -= 1;

	return 0;
}

int queue_delete(queue_t queue, void* data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	} 

	if (queue->length == 0) {
		return -1;
	}

	struct node *current = queue->head;
	while (current != NULL) {
		if (current->element == data) {
			current->prev->next = current->next;
			current->next->prev = current->prev;
			free(current);
			queue->length -= 1;
			
			return 0;
		}
		else {
			if (current->next == NULL) {
				break;
			}
			else {
				current = current->next;
			}
		}
	}

	return -1;
	
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if (queue == NULL || func == NULL) {
		return -1;
	}

	struct node* current = queue->head;
	while (current != NULL) {
		if (func(current->element, arg) == 1) {
			*data = current->element;
			return 0;
		}
		else {
			current = current->next;
		}
	}
	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	return queue->length;
}

