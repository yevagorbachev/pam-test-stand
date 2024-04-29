#include <string.h>
#define MAX_NUM_TASKS 10

typedef enum {
	S_FREE = 0, // not an allocated task
	S_STOP, // stopped 
	S_RUN, 	// running
} status_t;

typedef int ret_t;
typedef int tid_t;
typedef unsigned long time_t;

typedef struct task_t {
	ret_t (*entry) (task_t* tdata);
	time_t (*timer) (void);
	time_t next;
	time_t period;
	status_t status;
} task_t;

ret_t init_sched();
tid_t add_task(task_t task_info);
void free_task(tid_t tid);

ret_t start_task(tid_t tid);
ret_t stop_task(tid_t tid);
void run_tasks();

