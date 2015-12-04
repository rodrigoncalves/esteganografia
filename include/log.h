#ifndef LOG_H
#define LOG_H
#define LOGFILE "gl.log"

extern void LogCreate();
extern void Log(char *message);
extern void LogErr(char *message);

#endif