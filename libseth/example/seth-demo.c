#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// you need to define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
// to make it compiliable for POSIX, the 
// LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS is same for windows.

#define LIBSETH_APPLICATION_PORTABLE_FOR_POSIX
#define LIBSETH_APPLICATION_PORTABLE_FOR_WINDOWS
#include <seth.h>
#include <nkget.h>

// use LIBSETH_APPLICATION to define a seth application
// pass a application name to it.

LIBSETH_APPLICATION("demo")

int seth_application_main(int argc, char *argv[])
{
	// this is the entry point of the application
	// like ANSI C `main` function
	return 0;
}
