#pragma once

#if defined(_WIN32)
    #ifdef CAPTURE_BUILD
        #define CAPTURE_API __declspec(dllexport)
    #else
        #define CAPTURE_API __declspec(dllimport)
    #endif
#else
    #define CAPTURE_API
#endif