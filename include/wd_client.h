#ifndef WD_H__
#define WD_H__

#include <stddef.h>
#include <time.h>

typedef enum wd_stat
{
    FAILURE = -1,
    SUCCESS = 0
}wd_stat_t;


/* User main must have the argc and argv arguments */

/*
* WDStart Description:
*	Creates a "Watchdog" process. The watchdog tests the calling process for responsiveness 
*     and reboots the calling process if not responsive.
*   The function sets enviornment variables named "_THRESHOLD" and "_TIME_INTERVAL",
*     which will be unset by the WDStop() function.  
*
* @params:
*   argc - argc passed to calling process
*   argv - array of argv[] passed to calling process
*   thresh - threshold which defines how many unresponsive signals sent before rebooting the calling process  
*   time_interval - time interval for responsiveness check
*
* @returns:
*	0(= SUCCESS) if successful. Otherwise will return a non-zero value (= FAILURE).
*
* @Complexity:
*    Time: O(n)
*    Space O(1)
*/
wd_stat_t WDStart(int argc, char *argv[], size_t thresh, time_t time_interval);

/*
* WDStop Description:
*	Terminates the "Watchdog" process in an orderly manner.
*
* @params:
*   none
*
* @returns:
*	0(= SUCCESS) if successful. Otherwise will return a non-zero value (= FAILURE).
*
* @Complexity:
*    Time: O(n)
*    Space O(1)
*/
wd_stat_t WDStop(void);

#endif
