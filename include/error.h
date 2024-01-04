#pragma once

#include <intrin.h>

void errorBox(char *format, ...);
char *getLocalFileName(char *fullPath);

#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#if NDEBUG
#define ASSERT(cnd)\
	do {\
		if (!(cnd)){\
			errorBox("File: %s\nLine: %d\nCondition: %s",getLocalFileName(__FILE__),__LINE__,#cnd);\
			exit(1);\
		}\
	} while (0)
#else
#define ASSERT(cnd)\
	do {\
		if (!(cnd)){\
			__debugbreak();\
		}\
	} while (0)
#endif