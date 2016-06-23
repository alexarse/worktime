#include "Worktime.hpp"

int main(int argc, char* argv[])
{
	Worktime::Options opt(argc, argv);
	
	Worktime wt("/usr/local/share/worktime/", "time_data.json");
	
	if(!wt.IsOpen()) {
		return -1;
	}
	
	wt.ProcessOptions(opt);
	
	return 0;
}