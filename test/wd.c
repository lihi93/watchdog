#include <stdio.h>
#include <semaphore.h>
#include "wd_client.h"

#define INIT_VAL (0)
extern int start_caller_flag;

int main(int argc, char *argv[])
{
      start_caller_flag = 1;
      WDStart(argc, argv, INIT_VAL, INIT_VAL);
      
      
      return 0;
}