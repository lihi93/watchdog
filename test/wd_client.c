#include <stdio.h>
#include "wd_client.h"
#include <time.h>

int main(int argc, char *argv[])
{
      time_t start = time(NULL);
      size_t thresh = 10;
      time_t interval = 2;
      WDStart(argc, argv, thresh, interval);
      
      while (time(NULL) < (start + 60))
      {
            /* empty */
      }
      WDStop();
      return 0;
}