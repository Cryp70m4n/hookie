// data.h

#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// name is shared object name
// version format is major.minor.revision that is why we use char * instead of float
// baseAddress is first address that contains current shared object from /proc/pid/maps
typedef struct SharedObject {
		char *name;
		char *version;
		unsigned long baseAddress;
} SharedObject;

// pid = proccess ID
// name = commandline name we get it from /proc/pid/comm
// shared objects = list of shared objects that binary uses we get it from /proc/pid/maps
typedef struct ProcessData {
		int pid;
		char *name;
		SharedObject **loadedSharedObjects;
		// more
} ProcessData;


SharedObject **parseLoadedSharedObjects(int pid);
void freeSharedObjects(SharedObject **sharedObjects);

char *parseName(int pid);

#endif /* DATA_H */
