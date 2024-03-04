#ifndef SCHED_H__
#define SCHED_H__

#include <time.h>
#include <stddef.h>

#include "task.h"

typedef struct scheduler sch_t;
/*
* SchCreate Description:
*	Allocates memory for a new scheduler object.
*
* @params:
*   None
*
* @returns:
*	sch_t * - a pointer to the new scheduler object.
*
* @Complexity
*	O(1)
*/
sch_t *SchCreate(void);
/*
* SchDestroy Description:
*	Releases the memory alloacted for a scheduler, and internal allocations.
*   All refrences to the scheduler or related tasks become invalid.
*
* @params:
*   scheduler - pointer to a sch_t type
*   If the pointer is invalid, behavior is undefined.
*
* @returns:
*	void
*
* @Complexity
*	O(n)
*/
void SchDestroy(sch_t *scheduler);

/*
* SchAddTask Description:
*	Adds a new task to scheduler.
*
* @params:
*   scheduler - pointer to a sch_t type
*               If the pointer is invalid, behavior is undefined.
*   op_func   - Operation function pointer.
*             - Return value of 0 is for success: any other return value will stop the execution of tasks
*   params    - Parameter used by operation function.
*   exec_time - Time to execute task.
*   interval  - Interval of task execution, zero reserved for no repeats
*
* @returns:
*   UID of task added. if there was a problem with adding
* 		the task, will return a bad UID.
*
* @Complexity
*	O(n)
*/
ilrd_uid_t SchAddTask(sch_t *scheduler ,int (*op_func)(void *), void *params, time_t exec_time , time_t interval);

/*
* SchRemoveTask Description:
*	Removes a task from scheduler.
*
* @params:
*   scheduler - pointer to a sch_t type
*               If the pointer is invalid, behavior is undefined.
*   task_uid  - UID of task to remove.
*
* @returns:
*   None.
*
* @Complexity
*	O(n)
*/
void SchRemoveTask(sch_t *scheduler ,ilrd_uid_t task_uid);

/*
* SchIsEmpty Description:
*	Checks if the scheduler contains any tasks.
*
* @params:
*   scheduler - pointer to a sch_t type
*   If the pointer is invalid, behavior is undefined.
*
* @returns:
*	1 if empty, otherwise returns 0.
*
* @Complexity
*	O(1)
*/
int SchIsEmpty(const sch_t *scheduler);
/*
* SchSize Description:
*	Returns the current number of tasks in the scheduler.
*   Tasks' intervals count as one task.
*
* @params:
*   scheduler - pointer to a sch_t type
*   If the pointer is invalid, behavior is undefined.
*
* @returns:
*	Number of tasks currently in the scheduler.
*
* @Complexity
*	O(n)
*/
size_t SchSize(const sch_t *scheduler);

/*
* SchRun Description:
*	Executes the scheduler.
*
* @params:
*   Pointer to scheduler.
*   If the pointer is invalid, behavior is undefined.
*
* @returns:
*	Return value of operation function assigned to task.
*   0 - All tasks completed succesfully, scheduler remains empty.
*   any other value - scheduler stopped due to operation func returning a non-zero value
*
* @Complexity
*	O(n)
*/
int SchRun(sch_t *scheduler);

/*
* SchStop Description:
*   Stops the Running of the scheduler 
*	can added as a stop task to the scheduler.
*
* @params:
*   Pointer to scheduler.
*   If the pointer is invalid, behavior is undefined.
*
*
* @Complexity
*	O(1)
*/
void SchStop(sch_t *scheduler);

/*
* SchClear Description:
*	Clears all tasks from an existing scheduler
*
* @params:
*   Pointer to scheduler.
*   If the pointer is invalid, behavior is undefined.
*
* @returns:
*	None
*
* @Complexity
*	O(n)
*/
void SchClear(sch_t *scheduler);

#endif

