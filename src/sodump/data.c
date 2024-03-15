#include "../../include/sodump/data.h"

#define MAX_LINE_LENGTH 256
#define MAX_SHARED_OBJECT_LENGTH 64
#define MAX_SHARED_OBJECT_VERSION_LENGTH 8
#define MAX_SHARED_OBJECTS 512
#define MAX_ADDRESS_LENGTH 64
#define MAX_NAME_LENGTH 64

SharedObject **parseLoadedSharedObjects(int pid) {
		char mapsPath[64];
		snprintf(mapsPath, sizeof(mapsPath), "/proc/%d/maps", pid);

		FILE *mapsFile;
		mapsFile = fopen(mapsPath, "r");
		if (mapsFile == NULL) {
				return NULL;
		}

		char line[MAX_LINE_LENGTH];

		SharedObject **sharedObjects = malloc(MAX_SHARED_OBJECTS * sizeof(SharedObject *));
		if (sharedObjects == NULL) {
				fclose(mapsFile);
				return NULL;
		}
		for(int i = 0; i < MAX_SHARED_OBJECTS; i++) {
				sharedObjects[i] = NULL;
		}

		int currentSharedObject = 0;
		while(fgets(line, MAX_LINE_LENGTH, mapsFile) != NULL) {
				SharedObject *sharedObject = malloc(MAX_SHARED_OBJECT_LENGTH * sizeof(SharedObject));
				if (sharedObject == NULL) {
						continue;
				}

				bool soDetected = false;
				int i = strlen(line)-1;
				int k = 0;

				while(line[i] != '/' && line[i] != '\t') {
						// we can do if line[i-1] without checking if i > 0 because in maps / or \t will always appear before index 0
						if (line[i] == 'o' && line[i-1] == 's') {
								soDetected = true;
						}				
						i--;
				}
				i++; // cuz i captured  /

				if(soDetected == false) {
						continue;
				}

				char *versionString = malloc(MAX_SHARED_OBJECT_VERSION_LENGTH * sizeof(char));				
				if (versionString == NULL) {
						continue;
				}
				int f = 0;

				char *tmpString = malloc(MAX_SHARED_OBJECT_LENGTH * sizeof(char));
				if (tmpString == NULL) {
						free(versionString);
						continue;
				}

				bool versionDetected = false;
				while(i < strlen(line)-1) {
						if (k > 4) {
								if ( (tmpString[k-3] == '.' && tmpString[k-2] == 's' && tmpString[k-1] == 'o') && versionDetected == false) {
										versionDetected = true;
										i++;
										continue;
								}
						}
						if (versionDetected == true) {
								versionString[f] = line[i];
								f++;
						} else {
								tmpString[k] = line[i];
								k++;
						}
						i++;
				}
				if (k == 0) {
						free(versionString);
						free(tmpString);
						continue;
				}

				char *baseAddressString = malloc(MAX_ADDRESS_LENGTH * sizeof(char));

				if (baseAddressString == NULL) {
						free(versionString);
						free(tmpString);
						continue;
				}

				tmpString[k] = '\0';
				sharedObject->name = strdup(tmpString);
				free(tmpString);

				versionString[f] = '\0';
				sharedObject->version = strdup(versionString);
				free(versionString);

				int l = 0;
				int r = 0;
				while(line[l] != '-') {
						baseAddressString[r] = line[l];
						r++;
						l++;
				}
				char *endptr;
				unsigned long baseAddress = strtoul(baseAddressString, &endptr, 16);
				sharedObject->baseAddress = baseAddress;
				free(baseAddressString);


				int tmp = 0;
				bool alreadyDetected = false;
				while(sharedObjects[tmp] != NULL) {
						if (!strcmp(sharedObjects[tmp]->name, sharedObject->name) && !strcmp(sharedObjects[tmp]->version, sharedObject->version)) {
								alreadyDetected = true;
						}
						tmp++;
				}

				if (!alreadyDetected) {
						sharedObjects[currentSharedObject] = sharedObject;
						currentSharedObject++;
				}
		}
		fclose(mapsFile);

		return sharedObjects;
}

void freeSharedObjects(SharedObject **sharedObjects) {
		if (sharedObjects == NULL) return;
		int i = 0;
		while (sharedObjects[i] != NULL) {
				free(sharedObjects[i]->name);
				free(sharedObjects[i]->version);
				free(sharedObjects[i]);
				i++;
		}
		free(sharedObjects);
}

char *parseName(int pid) {
		char namePath[64];
		snprintf(namePath, sizeof(namePath), "/proc/%d/comm", pid);

		FILE *nameFile;
		nameFile = fopen(namePath, "r");
		if (nameFile == NULL) {
				return NULL;
		}

		char *name = malloc(MAX_NAME_LENGTH * sizeof(char));
		if (name == NULL) {
				fclose(nameFile);
				return NULL;
		}

		if (fgets(name, MAX_NAME_LENGTH, nameFile) != NULL) {
        char *newline = strchr(name, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
				fclose(nameFile);
				return name;
		}

		free(name);
		fclose(nameFile);
		return NULL;
}
