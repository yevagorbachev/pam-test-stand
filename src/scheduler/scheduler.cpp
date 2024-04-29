#include "scheduler.hpp"

static task_t tasks[MAX_NUM_TASKS] = {0};

ret_t init_sched() {
	for (tid_t id = 0; id < MAX_NUM_TASKS; id++) {
		(tasks + id)->status = S_FREE;
	}
	return 0;
}

tid_t add_task(task_t task_info) {
	/* the id is the index of the first free task */
	tid_t id;
	for (id = 0; id < MAX_NUM_TASKS; id++) {
		if (S_FREE == (tasks + id)->status) {
			break;
		}
	}
	
	/* error cases */
	if (id == MAX_NUM_TASKS) { // no space
		return -1; 
	}
	if (NULL == task_info.entry) { // invalid entry point
		return -2;
	}
	if (NULL == task_info.timer) { // invalid timer
		return -3;
	}

	// OK to add the task, do it and return the ID where it is
	memcpy(tasks + id, &task_info, sizeof(task_info));
	return id;
}

void free_task(tid_t id) {
	memset(tasks + id, 0, sizeof(task_t));
}

ret_t start_task(tid_t tid) {
	if (S_FREE == (tasks + tid)->status) { // not an allocated task
		return -1;
	}

	(tasks + tid)->status = S_RUN;
	return 0;
}

ret_t stop_task(tid_t tid) {
	if (S_FREE == (tasks + tid)->status) { // not an allocated task 
		return -1;
	}
	
	(tasks + tid)->status = S_STOP;
	return 0;
}

void run_tasks() {
	for (tid_t id = 0; id < MAX_NUM_TASKS; id++) {
		task_t* task = tasks + id;
		if (task->status != S_RUN) {
			continue;
		}
		if (NULL == task->timer) {
			continue;
		}
		if (NULL == task->entry) {
			continue;
		}

		ret_t ret = 0;
		time_t now = task->timer();
		if (now < task->next) {
			continue;
		}

		ret = task->entry(task);
		task->next += task->period;
	}
}
