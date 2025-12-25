#ifndef TEST_COMMON_H
#define TEST_COMMON_H

// Common test utilities and stubs

// Stub for raylib's TraceLog to avoid dependency in tests
#define LOG_INFO 0
#define LOG_DEBUG 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define TraceLog(level, ...) ((void)0)

#endif // TEST_COMMON_H

