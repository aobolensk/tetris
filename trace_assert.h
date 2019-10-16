#ifndef TRACE_ASSERT_H
#define TRACE_ASSERT_H
#if defined(__GNUC__) && defined(__linux__)
#include <execinfo.h>
#include <unistd.h>
#include "config.h"
#endif  // defined(__GNUC__) && defined(__linux__)

void print_stacktrace() {
    #if defined(__GNUC__) && defined(__linux__)
    void *stacktrace[MAX_STACK_TRACE_SIZE];
    int size = backtrace(stacktrace, MAX_STACK_TRACE_SIZE);
    backtrace_symbols_fd(stacktrace, size, STDERR_FILENO);
    #endif  // defined(__GNUC__) && defined(__linux__)
}

void trace_signal(int signal) {
    fprintf(stderr, "Signal %d\n", signal);
    print_stacktrace();
    exit(1);
}

#ifdef NDEBUG
#define trace_assert(expression) (expression)
#else
#define trace_assert(expression)                                                                \
    if (!(expression)) {                                                                        \
        fprintf(stderr, "Assertion failed: %s at %s:%d\n", #expression, __FILE__, __LINE__);    \
        print_stacktrace();                                                                     \
        exit(1);                                                                                \
    }
#endif  // NDEBUG

#endif /* TRACE_ASSERT_H */
