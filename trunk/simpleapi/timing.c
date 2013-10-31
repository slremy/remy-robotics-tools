#include "timing.h"


double ClockGetTime(){
        struct timeval tv;   // see gettimeofday(2)
        gettimeofday(&tv, NULL);
        double t = (double) tv.tv_sec + (double) tv.tv_usec/1000000.0; 
        return(t);
}