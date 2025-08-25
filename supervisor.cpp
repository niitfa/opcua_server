#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "hello_from_supervisor.h"

int main(int argc, char* argv[])
{
	hello_from_supervisor();
	return 0;
}
