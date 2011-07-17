#include "owimotorcontroller.hpp"

int main(){
	usbowiarm arm(0);
	arm.test();
	return 0;
}
