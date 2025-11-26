#pragma once

#if defined(_WIN32)
    #ifdef MODEL_PROVIDER_BUILD
        #define MODEL_PROVIDER_API __declspec(dllexport)
    #else
        #define MODEL_PROVIDER_API __declspec(dllimport)
    #endif
#else
    #define MODEL_PROVIDER_API
#endif