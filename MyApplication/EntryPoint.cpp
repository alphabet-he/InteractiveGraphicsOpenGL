#include "cMyApplication.h"

int main(int argc, char** argv) 
{
	cMyApplication* myApplication = new cMyApplication();
	myApplication->Initialize(argc, argv);
	myApplication->Run();
	return 0;
}