#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void errorBox(char *format, ...){
	va_list args;
	va_start(args,format);
	static char msg[2048];
	vsnprintf(msg,sizeof(msg),format,args);
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONERROR);
	va_end(args);
}

char *getLocalFileName(char *fullPath){
	return strrchr(fullPath, '\\') ? strrchr(fullPath, '\\') + 1 : fullPath;
}