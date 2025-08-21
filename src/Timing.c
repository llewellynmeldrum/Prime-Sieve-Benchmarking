#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Timing.h"
Timespan GetTimespan(timeval before, timeval after){
	time_t s_diff = labs(after.tv_sec - before.tv_sec);
	suseconds_t us_diff = abs(after.tv_usec - before.tv_usec);
	//printf("BEFORE: us=%07d s=%ld\n AFTER: us=%07d s=%ld\n", before.tv_usec, before.tv_sec, after.tv_usec, after.tv_sec);
	return (Timespan){
		.m = ((time_t) s_diff)/SECS_PER_MIN,
		.s = ((time_t) s_diff)%SECS_PER_MIN, 
		.ms =((time_t) us_diff)/USECS_PER_MSEC,
		.us =((time_t) us_diff)%USECS_PER_MSEC, 
	};
}

void PrintTimespan(Timespan t){
	// 03 m 27 s 123 ms 456 µs OR 03m27s (123 ms, 456 µs) OR 03:27:123:456
	printf("%02ld m %02ld s %03ld ms %03ld µs\n",t.m ,t.s, t.ms, t.us);
}
