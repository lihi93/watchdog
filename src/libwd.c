/************************************
* author: Lihi Cohen                *
* subject: Watchdog library         *
*************************************/

/* #define _POSIX_C_SOURCE 200809L */
#include <stdio.h> /* printf */
#include <pthread.h> /* pthread_create, pthread_join */
#include <stdatomic.h> /* atomic_size_t */
#include <signal.h> /* SIGUSR1, SIGUSR2, SIGSTOP, SIGCONT */
#include <unistd.h> /* sleep, pause, getpid, fork */
#include <semaphore.h> /* sem_open */
#include <stdlib.h> /* malloc */
#include <string.h> /* itoa */
#include <fcntl.h> /* O_CREAT*/
#include <sys/wait.h> /* wait */
#include "sched.h"
#include "wd_client.h"

#define BUFF_SIZE (20)
#define DECIMAL (10)
#define USER_PROC (0)
#define OVERWRIGHT_VAL (1)
#define STOP (1)
#define RUN (0)

#ifndef NDEBUG
#define PRINTF_DEBUG(...) (printf(__VA_ARGS__))
#else
#define PRINTF_DEBUG(...)
#endif

pid_t wd_pid = 0;
atomic_size_t counter = 0;
int start_caller_flag = 0;
atomic_int stop_flag = 0;
sch_t *scheduler;
sem_t *sem;
pthread_t thread;
int argc_glbl;
char **argv_glbl;

/********************* FORWARD DECLARATIONS ***********************/

/* tasks operations */
static int SignalSender(void *param);
static int CounterCheck(void *param);

/* signal handlers */
static void ResetCounter(int signum);
static void WdTermination(int signum);

/* thread routine */
static void *ThreadRoutine(void *args);

/* helpful functions */
static void SigActInit(void);
static wd_stat_t CleanUp(void);

/********************* USER FUNCTIONS ***********************/

wd_stat_t WDStart(int argc, char *argv[], size_t thresh, time_t time_interval)
{
	char thresh_buff[BUFF_SIZE] = {0};
	char time_buff[BUFF_SIZE] = {0};
	time_t time_interval_env = 0;
	ilrd_uid_t cntr_check = {0};
	ilrd_uid_t signal_send = {0};
	
	argc_glbl = argc;
	argv_glbl = argv;

	scheduler = SchCreate();
	if (NULL == scheduler)
	{
		return FAILURE;
	}
	
	if (USER_PROC == start_caller_flag)
	{
		sem_unlink("/bin_sem");
		sprintf(time_buff, "%ld", time_interval);
		if (FAILURE == setenv("_TIME_INTERVAL", time_buff, OVERWRIGHT_VAL))
		{
			SchDestroy(scheduler);
			return FAILURE;
		}
	}
	sem = sem_open("/bin_sem", O_CREAT, 0644, 0);
	
	SigActInit();
	time_interval_env = (time_t)atoi(getenv("_TIME_INTERVAL"));

	PRINTF_DEBUG("I am: %d\n", start_caller_flag);
	PRINTF_DEBUG("%s\n", argv[0]);
	if (USER_PROC == start_caller_flag)
	{
		sprintf(thresh_buff, "%ld", thresh);
		if (FAILURE == setenv("_THRESHOLD", thresh_buff, OVERWRIGHT_VAL))
		{
			SchDestroy(scheduler);
			return FAILURE;
		}

		wd_pid = fork();
		if (wd_pid == 0)
		{
			PRINTF_DEBUG("going to do exec for wd %d\n", wd_pid);
			if (SUCCESS != execvp("bin/wd.out", argv))
			{
				CleanUp();
				return FAILURE;
			}
		}
		else if (wd_pid > 0)
		{
			cntr_check = SchAddTask(scheduler, CounterCheck, NULL, time(NULL), time_interval_env);
			signal_send = SchAddTask(scheduler, SignalSender, NULL, time(NULL), time_interval_env);
			if (UIDIsSame(UIDGetBad(), cntr_check) || UIDIsSame(UIDGetBad(), signal_send))
			{
				CleanUp();
				return FAILURE;
			}

			if (0 != pthread_create(&thread, NULL, ThreadRoutine, NULL))
			{
				CleanUp();
				return FAILURE;
			}
		}
		else if (0 > wd_pid)
		{
			return FAILURE;
		}
	}
	else 
	{
		sem_post(sem);
		kill(getppid(), SIGCONT);
		wd_pid = getppid();

		cntr_check = SchAddTask(scheduler, CounterCheck, NULL, time(NULL), time_interval_env);
		signal_send = SchAddTask(scheduler, SignalSender, NULL, time(NULL), time_interval_env);
		if (UIDIsSame(UIDGetBad(), cntr_check) || UIDIsSame(UIDGetBad(), signal_send))
		{
			CleanUp();
			return FAILURE;
		}	
		SchRun(scheduler);
		
		SchDestroy(scheduler);
	}
	
	return SUCCESS;
}


wd_stat_t WDStop(void)
{
	wd_stat_t status = SUCCESS;
	status = kill(wd_pid, SIGUSR2);
	status += raise(SIGUSR2);
	
	status += wait(NULL);
	status += pthread_join(thread, NULL);
	
	status += CleanUp();
	return status;
}


/********************* USER THREAD ***********************/

static void *ThreadRoutine(void *args)
{
	(void)args;
	sem_wait(sem); /* allow first exec*/
		  
	SchRun(scheduler);
	SchDestroy(scheduler);
	
	return NULL;
}

/********************* SCHEDULER TASKS ***********************/

static int CounterCheck(void *param)
{
	(void)param;
	size_t threshold = (size_t)atoi(getenv("_THRESHOLD"));
	if (RUN != stop_flag)
	{
		return STOP;
	}
	PRINTF_DEBUG("Counter is: %ld, I am: %d\n", counter, start_caller_flag);
	if (counter > threshold)
	{
		if (USER_PROC == start_caller_flag)
		{
			wait(NULL);
			wd_pid = fork();
			if (0 < wd_pid)
			{
				counter = 0;
			}
			else if (0 == wd_pid)
			{
				kill(getppid(), SIGSTOP);
				if (SUCCESS != execvp("bin/wd.out", argv_glbl))
				{
					WDStop();
				}
			}
			else if (0 > wd_pid)
			{
				WDStop();
			}
		}
		else 
		{
			counter = 0;
			if (SUCCESS != execvp(argv_glbl[0], argv_glbl))
			{
				WDStop();
			}
		}
	}
	else
	{
		++counter;
	}

	return SUCCESS;
}


static int SignalSender(void *param)
{
	(void)param;
	
	if (RUN != stop_flag)
	{
		return STOP;
	}

	if (USER_PROC != start_caller_flag)
	{
		PRINTF_DEBUG("im the watchdog\n");
	}
	else
	{
		PRINTF_DEBUG("im the user\n");
	}
	
	kill(wd_pid, SIGUSR1);

	return SUCCESS;
}

/********************* SIGNAL HANDLERS ***********************/

static void ResetCounter(int signum)
{
	if (signum == SIGUSR1)
	{
		counter = 0;
	}
}

static void WdTermination(int signum)
{
	if (signum == SIGUSR2)
	{
		++stop_flag;
	}
}

/********************* HELPFUL FUNCTIONS ***********************/

static void SigActInit(void)
{
	struct sigaction sa1 = {0};
	struct sigaction sa2 = {0};

	sa1.sa_handler = ResetCounter;
	sa1.sa_flags = 0;
	sigaction(SIGUSR1, &sa1, NULL);
	sa2.sa_handler = WdTermination;
	sa2.sa_flags = 0;
	sigaction(SIGUSR2, &sa2, NULL);
}

static wd_stat_t CleanUp(void)
{
	wd_stat_t status = SUCCESS;
	status += sem_unlink("/bin_sem");
	status += sem_close(sem);

	status += unsetenv("_THRESHOLD");
	status += unsetenv("_TIME_INTERVAL");
	return status;
}







