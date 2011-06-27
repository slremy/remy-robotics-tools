// g++ -o testprogram `pkg-config --cflags playerc++ ` `pkg-config --libs playerc++ ` testprogram.cc
// Includes
#include <stdio.h>
#include <libplayerc++/args.h>
#include <libplayerc++/playerc++.h>
 
using namespace PlayerCc;
 
// Initialization
int main(int argc, char **argv)
{
	parse_args(argc,argv); 
	PlayerClient robot(gHostname, gPort);
	Position2dProxy pp(&robot,gIndex);
  
	sleep(5);

	for(int i = 0; i < 10; i++)
	{
		sleep(1);
		pp.SetSpeed(1,0);
		printf("1,0\n");
	}

	pp.SetSpeed(0,0);
	printf("0,0\n");
sleep(5);
	for(int i = 0; i < 10; i++)
	{
		sleep(1);
		pp.SetSpeed(0,1);
		printf("0,1\n");
	}
 
	pp.SetSpeed(0,0);
	printf("0,0\n");
sleep(5);
	// End of program
	return 0;
}
