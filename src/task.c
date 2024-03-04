#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "task.h"

struct task
{
	ilrd_uid_t uid;
	op_func_t function;
	void *param;
	time_t exec_time;
	time_t interval;
}; 


task_t *TaskCreate(int (*op_func_t)(void *), void *params, time_t exec_time , time_t interval)
{
	task_t *task = (task_t *)malloc(sizeof(task_t));

	assert(NULL != op_func_t);
	
	if (NULL == task)
	{
		return NULL;
	}
	
	task->uid = UIDCreate();
	
	if (!UIDIsSame(task->uid, UIDGetBad()))
	{
		task->function = op_func_t;
		task->param = params;
		task->exec_time = exec_time;
		task->interval = interval;
	}
	
	return task;
}


void TaskDestroy(task_t *task)
{
	assert(NULL != task);
	
	free(task);
}


int TaskRun(task_t *task)
{
	assert(NULL != task);
		
	return task->function(TaskGetParam(task));
}


time_t TaskGetExecTime(task_t *task)
{
	assert(NULL != task);
	
	return task->exec_time;
}


time_t TaskGetInterval(task_t *task)
{
	assert(NULL != task);
	
	return task->interval;
}


ilrd_uid_t TaskGetUID(task_t *task)
{
	assert(NULL != task);
	
	return task->uid;
}

void *TaskGetParam(task_t *task)
{
	assert(NULL != task);
	
	return task->param;
}


op_func_t TaskGetFunc(task_t *task)
{	
	assert(NULL != task);
	
	return task->function;
}


void TaskSetExecTime(task_t *task, time_t exec_time)
{
	assert(NULL != task);
	
	task->exec_time = exec_time;
}

void TaskSetInterval(task_t *task, time_t interval)
{
	assert(NULL != task);
	
	task->interval = interval;
}


void TaskSetFunc(task_t *task, int (*op_func_t)(void *))
{
	assert(NULL != task);
	
	task->function = op_func_t;
}


void TaskSetParams(task_t *task, void *params)
{
	assert(NULL != task);
	
	task->param = params;
}














