#include "smartctl.h"

int main(int argc, char **argv)
{
	int Mode = 0;

	Mode = GetModeFromArg(argc, argv);

	switch(Mode)
	{
		case MODE_STAT:
			ShowStat();
			break;
		case MODE_START:
			StartSmartExpert();
			break;
		case MODE_STOP:
			StopSmartExpert();
			break;
		default:
			Usage(argv[0]);
			break;
	}
}
