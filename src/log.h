
#ifndef __LOG_H__
#define __LOG_H__

void log_debug(int dbg_lvl, char* file, int line, char* fmt, ...);

#define LOGD(format, ...) { log_debug(1, __FILE__, __LINE__, format, ##__VA_ARGS__); }

#endif /* __LOG_H__ */
