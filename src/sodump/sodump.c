/*
 * SODump => Shared Object Dump
 * Shared Object Dump is linux command-line utility (part of hookie-tools).
 * SODump takes one argument --pid and returns followin data:
 * 	=> shared object name
 * 		=> shared object version
 * 			=> shared object base address
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/sodump/data.h"


int main(int argc, char *argv[]) {
		if (argc < 3) {
				printf("\033[0;31m[X] >>> Sufficient number of arguments.\033[0m\n\033[0;34m[?] => USAGE: sodump --pid PID\033[0m\n");
				return -1;
		}

		int pid = -1;
		for(int i = 1; i < argc-1; i++) {
				if (!strcmp(argv[i], "--pid") | !strcmp(argv[i], "-p")) {
						char *endptr;
						pid = strtol(argv[i+1], &endptr, 10);
						break;
				}
		}

		if (pid == -1) {
				printf("\033[0;31m[X] >>> FAILED TO PARSE PID\033[0m\n");
				return -2;
		}

		char *name = parseName(pid);

		if (name == NULL) {
				printf("\033[0;31m[X] >>> FAILED TO LOAD PARSE NAME\033[0m\n");
				return -3;
		}
	
		SharedObject **loadedSharedObjects = parseLoadedSharedObjects(pid);

		if (loadedSharedObjects == NULL) {
				printf("\033[0;31m[X] >>> FAILED TO LOAD SHARED OBJECTS\033[0m\n");
				free(name);
				return -4;
		}
		
		ProcessData *pData = malloc(sizeof(ProcessData));;
		if (pData == NULL) {
				free(name);
				freeSharedObjects(loadedSharedObjects);
		}

		pData->pid = pid;
		pData->name = strdup(name);
		pData->loadedSharedObjects = loadedSharedObjects;

		printf("(\033[0;34mPID\033[0m) -> [\033[0;33m%d\033[0m] <<<< | >>>> [\033[0;33m%s\033[0m] <- (\033[0;34mNAME\033[0m)\n", pData->pid, pData->name);


		int k = 0;
		while(loadedSharedObjects[k] != NULL) {
				printf("(\033[0;32mSHARED OBJECT\033[0m) -> [\033[0;31m%s\033[0m]\n\t(\033[0;35mVERSION\033[0m) -> [\033[0;36m%s\033[0m]\n\t\t(\033[0;34mBASE ADDRESS\033[0m) -> [\033[0;33m0x%lx\033[0m]\n", loadedSharedObjects[k]->name, loadedSharedObjects[k]->version, loadedSharedObjects[k]->baseAddress);
				k++;
		}

		free(name);
		freeSharedObjects(loadedSharedObjects);
		free(pData->name);
		free(pData);

		return 0;
}
