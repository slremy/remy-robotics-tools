//  gcc -lm pendulum.c  -o pendulum

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

double time0=0;
double interval=0;
double integerpart;
double fractionalpart;

float control=0;
float h = 0.003, g=9.81, l=.1;

float Y[3];

double gamma1, gamma0;

double ClockGetTime(){
        struct timeval tv;   // see gettimeofday(2)
        gettimeofday(&tv, NULL);
        double t = (double) tv.tv_sec + (double) tv.tv_usec/1000000.0;
        return(t);
}

void calculateControl (int i)
{
	double y = gamma0 * Y[0] + gamma1 * Y[1];
        Y[1]=Y[0];
        Y[0]=y;
	printf("%lf  %lf\n",y,ClockGetTime()-time0);
}

void calculateControl2 (int i)
{
   printf("<--->>>>>>>%lf\n",ClockGetTime()-time0);

}

void exit_func (int i)
{
    signal(SIGINT,exit_func);
    printf("\nBye Bye!!!\n");
    exit(0);
}

int main (int argc, char* argv[])
{
  struct itimerval tout_val;
  double initial=0;
  if (argc > 1)
	initial = strtof(argv[1],0);

  gamma1 = - 1/(l*h*h), gamma0 = g/(l*h*h);
  gamma0 = - 2/(-g/l*h*h-1), gamma1 = 1/(-g/l*h*h-1);
  Y[0]=Y[1]=initial;

  interval = h; //interval in seconds!
  fractionalpart = modf (interval , &integerpart);

  tout_val.it_interval.tv_sec = integerpart;
  tout_val.it_interval.tv_usec = fractionalpart*1000*1000;
  tout_val.it_value.tv_sec = integerpart; /* X seconds timer */
  tout_val.it_value.tv_usec = fractionalpart*1000*1000;

  setitimer(ITIMER_REAL, &tout_val, 0);
  signal(SIGALRM,calculateControl); /* set the Alarm signal capture */

  interval = 1.00000 +1; //interval in seconds!
  fractionalpart = modf (interval , &integerpart);

  tout_val.it_interval.tv_sec = integerpart;
  tout_val.it_interval.tv_usec = fractionalpart*1000*1000;
  tout_val.it_value.tv_sec = integerpart; /* X seconds timer */
  tout_val.it_value.tv_usec = fractionalpart*1000*1000;

  setitimer(ITIMER_PROF, &tout_val, 0);
  signal(SIGPROF,calculateControl2); /* set the Alarm signal capture */


  time0 = ClockGetTime();
 
  while (1)
  {
    //printf("!");
  }
  signal(SIGINT,exit_func);
 
  return 0;

}
