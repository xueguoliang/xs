#ifndef XS_LOG_H
#define XS_LOG_H

#define xs_dbg(fmt, ...) printf("%s %d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define xs_log(...)
//#define pnms_dbg(fmt, ...) printf("[%s]\t%s %d:" fmt, g_program, __FILE__, __LINE__, ##__VA_ARGS__)
//#define pnms_log printf
#define xs_err xs_dbg

#endif // XS_LOG_H
