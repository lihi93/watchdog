#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */
#include <unistd.h>	/* sleep */

#include "pq.h"
#include "uid.h"
#include "task.h"
#include "sched.h"

#define TRUE (1)
#define FALSE (0)

#define SUCCESS (0)
#define FAIL (1)

struct scheduler
{
	pq_t *pq;
	int running_status;
};


static int priority_func(const void *lhs, const void *rhs);
static int IsSameUID(const void *data, const void *param);

/*_______________user functions_______________*/


sch_t *SchCreate(void)
{
	sch_t *sched = (sch_t *)malloc(sizeof(sch_t));
	
	if (NULL == sched)
	{
		return NULL;
	}
	
	sched->pq = PQCreate(priority_func);
	if (NULL == sched->pq)
	{
		free(sched);
		return NULL;
	}

	sched->running_status = TRUE;
	
	return sched;
}


void SchDestroy(sch_t *scheduler)
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->pq);	
		
	SchClear(scheduler);
	
	PQDestroy(scheduler->pq);
	
	free(scheduler);
}


ilrd_uid_t SchAddTask(sch_t *scheduler ,int (*op_func)(void *), void *params, time_t exec_time ,time_t interval)
{
    task_t *task = NULL;
    
    assert(NULL != scheduler);
	assert(NULL != scheduler->pq);
    assert(NULL != op_func);
    
    task = TaskCreate(op_func, params, exec_time, interval);
    if (NULL == task)
    {
        return UIDGetBad();
    }
    
    if(0 != PQEnqueue(scheduler->pq, (void *)task))
    {
        return UIDGetBad();
    }
    
    return TaskGetUID(task); 
}


void SchRemoveTask(sch_t *scheduler ,ilrd_uid_t task_uid)
{
	assert(NULL != scheduler);
	assert(!PQIsEmpty(scheduler->pq));
	assert(NULL != scheduler->pq);
	
	TaskDestroy(PQErase(scheduler->pq, IsSameUID, &task_uid));
}


size_t SchSize(const sch_t *scheduler)
{
	assert(NULL != scheduler->pq);
	assert(NULL != scheduler);

	return PQSize(scheduler->pq);
}


void SchClear(sch_t *scheduler)
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->pq);

	while (!SchIsEmpty(scheduler))
	{
		TaskDestroy(PQPeek(scheduler->pq));
		
		PQDequeue(scheduler->pq);
	}
}


int SchIsEmpty(const sch_t *scheduler)
{
	assert(NULL != scheduler->pq);
	assert(NULL != scheduler);

	return PQIsEmpty(scheduler->pq);
}


int SchRun(sch_t *scheduler)
{
	task_t *task = {0};
	int func_status = 0;
	time_t current_time = 0;
	time_t time_until_exec = 0;

	assert(NULL != scheduler->pq);
	assert(NULL != scheduler);

	scheduler->running_status = TRUE;
	
	while (!SchIsEmpty(scheduler) && SUCCESS == func_status && TRUE == scheduler->running_status)
	{
		current_time = time(NULL);
		task = PQPeek(scheduler->pq);
		time_until_exec = TaskGetExecTime(task) - current_time;

		if(0 < time_until_exec)
		{
		  sleep(time_until_exec);
		}

		PQDequeue(scheduler->pq);
		func_status = TaskRun(task);

		if (0 < TaskGetInterval(task))
		{
		  TaskSetExecTime(task, TaskGetExecTime(task) + TaskGetInterval(task));
		  PQEnqueue(scheduler->pq, (void *)task);
		}
		else
		{
			TaskDestroy(task);
		}
	}
	
	scheduler->running_status = TRUE;

	return func_status;
}


void SchStop(sch_t *scheduler)
{
	assert(NULL != scheduler);
	
	scheduler->running_status = FALSE;
}


/*_______________inner functions_______________*/

static int priority_func(const void *lhs, const void *rhs)
{
	task_t *task_l = (task_t *)lhs;
	task_t *task_r = (task_t *)rhs;
	
	/* implementaion dependent*/
	return TaskGetExecTime(task_r) - TaskGetExecTime(task_l);
}


static int IsSameUID(const void *data, const void *param)
{
	ilrd_uid_t task_uid = TaskGetUID((task_t*)data);
	return UIDIsSame(task_uid, *(ilrd_uid_t *)param);
}





 

