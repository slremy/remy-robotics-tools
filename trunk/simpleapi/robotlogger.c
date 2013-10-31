/*
 This program is the basic form of the code to read data from a robot.
 It should be modified based on the type of data needed for logging.
 
 It makes use of the curl libraries, and it depends on robot.h/c, timing.h/c
 
 
 gcc robotlogger.c robot.c timing.c  -lcurl -Wall -o 
*/
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "robot.h"
#include "timing.h"

void fetchData(void) {
	data temp;
	double px = -1, py = -1, pz = -1, ox = -1, oy = -1, oz = -1, ow = -1, t = -1;

	temp=getFromRobot("state?");
	sscanf(temp.contents, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", &px, &py, &pz, &ox, &oy, &oz, &ow); 
	printf("%lf %lf %lf %lf %lf %lf %lf %lf\n", ClockGetTime(), px, py, pz, ox, oy, oz, ow);
	return;
}

void signalHandler(int cause, siginfo_t *HowCome, void *ucontext) {
	fetchData();
}


int main(void)
{
	char temp_base_url[64] = "http://somehost:someport/";
	char permissions[64] = "&id=someID";
	
	setHost(temp_base_url);
	setPermission(permissions);
	
	struct itimerval it_val;	/* for setting itimer */
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 986000/10;	
	it_val.it_interval = it_val.it_value;

	struct sigaction sa;
	sa.sa_sigaction = signalHandler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = SA_SIGINFO; /* we want a siginfo_t */
		
	if (sigaction (SIGPROF, &sa, 0)) 
	{
		perror("Unable to catch SIGPROF");
	}
	else 
	{
		setitimer(ITIMER_PROF, &it_val, NULL);
		while (1) 
		{
			;
		}
	}
	return 0;
}