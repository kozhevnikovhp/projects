// HugePing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(int argc, char* argv[])
{
	FILE *fd = fopen("ping_prot.txt", "wt");
	int MaxDelay = 0, MinDelay = 10000, Total = 0, n=0;
	int delay, range_min = 635, range_max = 649;
	int range = range_max-range_min;
	srand((unsigned)time(NULL));
	for (int i = 0; i<200*1000; i++)
	{
		delay = range_min+(rand() % range);
		Total+=delay;
		if (delay>MaxDelay)
			MaxDelay = delay;
		if (delay<MinDelay)
			MinDelay = delay;
		n++;
		fprintf(fd, "Ответ от 192.168.13.1: число байт=1000 время=%dмс TTL=128\n", delay);
	}
	Total/=n;
	fprintf(fd, "%d %d %d\n", MinDelay, MaxDelay, Total);
	printf("%d %d %d\n", MinDelay, MaxDelay, Total);
	fclose(fd);

	return 0;
}
