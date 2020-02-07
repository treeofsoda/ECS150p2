#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

enum run_state {READY,RUN,BLOCK,ZOMBIE};

static uthread_t cur_tid = 0;
static queue_t ready_queue;
static queue_t block_queue;
static queue_t zombie_queue;

struct TCB* current_thread;

struct TCB {
	uthread_t tid;
	uthread_ctx_t* cxt;
	void* stack_ptr;
	enum run_state state;
};

static void create_main_thread(void) {
	read_queue = queue_create();
	block_queue = queue_create();
	zombie_queue = queue_create();

	struct TCB* main_thread = (struct TCB*) malloc(sizeof(struct TCB*));
	main_thread->tid = 0;
	cur_tid = 1;
	main_thread->stack_ptr = uthread_ctx_alloc_stack();
	if (uthread_ctx_init(&(main_thread->cxt), main_thread->stack_ptr, NULL, NULL) == -1) {
		fprintf(stderr, "Init main_thread error.\n");
	}
	if (main_thread == NULL || main_thread->stack_ptr == NULL) {
		fprintf(stderr, "Create main_thread or stack pointer error.\n");
	}
	current_thread = main_thread;
}

void uthread_yield(void)
{
	struct TCB* prev_thread = current_thread;
	struct TCB* next_thread = (struct TCB*) malloc(sizeof(struct TCB*));

	if (current_thread->state != BLOCK || current_thread->state != ZOMBIE) {
		queue_enqueue(ready_queue, current_thread);
	}

	queue_dequeue(ready_queue, (void**)&next_thread);
	next_thread->state = RUN;
	current_thread = next_thread;
	uthread_ctx_switch(&(prev_thread->cxt), &(current_thread->cxt));

}

uthread_t uthread_self(void)
{
	return current_thread->tid;
}

int uthread_create(uthread_func_t func, void *arg)
{
	if (cur_tid = 0) {
		create_main_thread();
	}

	struct TCB* new_thread = (struct TCB*) malloc(sizeof(struct TCB));
	if (new_thread == NULL) {
		return -1;
	}
	new_thread->stack_ptr = uthread_ctx_alloc_stack();
	if (new_thread->stack_ptr == NULL) {
		return -1;
	}
	if (uthread_ctx_init(&(new_thread->cxt), new_thread->stack_ptr, NULL, NULL) == -1) {
		return -1;
	}
	new_thread->tid = cur_tid;
	cur_tid++;
	new_thread->state = READY;

	if (queue_enqueue(ready_queue, new_thread) == -1) {
		return -1;
	}

	return new_thread->tid;
}

void uthread_exit(int retval)
{
	/* TODO Phase 2 */
}

int uthread_join(uthread_t tid, int *retval)
{
	if (tid == 1 || tid == current_thread->tid) {
		return -1;
	}

	struct TCB* temp_thread = (struct TCB*) malloc(sizeof(struct TCB));
	temp_thread = NULL;

	int found_in_ready = (queue_iterate(ready_queue, &find_item, (void**)&tid, (void**)&temp_thread));
	if (found_in_ready == 1 && temp_thread != NULL) {
		current_thread->state = BLOCK;
		temp_thread->state = READY;
		queue_enqueue(block_queue, current_thread);
		queue_dequeue(ready_queue, temp_thread);
		uthread_yield();
	}

	temp_thread = NULL;
	int found_in_zombie = (queue_iterate(zombie_queue, &find_item, (void**)&tid, (void**)&temp_thread));
	if (found_in_zombie == 1 && temp_thread != NULL) {
		queue_delete(zombie_queue, temp_thread);

		free(temp_thread);
		cur_tid--;
		
		return 0;
	}
	return -1;
	/* TODO Phase 3 */
}

static int find_item(void* data, void* arg) //This function is from queue_tester.c
{
	int* a = (int*)data;
	int match = (int)(long)arg;

	if (*a == match)
		return 1;

	return 0;
}
