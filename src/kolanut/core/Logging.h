#pragma once

#include <iostream>

#ifndef VERBOSITY
#define VERBOSITY 5
#endif

#ifndef NDEBUG
#define knM_log(type, x) \
    std::cout << "[" << (type) << "] " << x << std::endl

#define knM_error(type, x) \
    std::error << "[" << (type) << "] " << x << std::endl
#else
#define knM_log(type, x) do {} while(false)
#define knM_error(type, x) do {} while(false)
#endif

#if VERBOSITY >= 5
#define knM_logDebug(x) knM_log("Debug", x)
#else
#define knM_logDebug(x) do {} while(false)
#endif

#if VERBOSITY >= 4
#define knM_logTrace(x) knM_log("Trace", x)
#else
#define knM_logDebug(x) do {} while(false)
#endif

#if VERBOSITY >= 3
#define knM_logInfo(x) knM_log("Info", x)
#else
#define knM_logDebug(x) do {} while(false)
#endif

#if VERBOSITY >= 2
#define knM_logError(x) knM_log("Error", x)
#else
#define knM_logDebug(x) do {} while(false)
#endif

#if VERBOSITY >= 1
#define knM_logFatal(x) knM_log("Fatal", x)
#else
#define knM_logDebug(x) do {} while(false)
#endif