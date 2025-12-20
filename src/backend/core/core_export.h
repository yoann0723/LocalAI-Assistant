#pragma once

#if defined(_WIN32)
#ifdef CORE_BUILD
#define LOCALAI_API __declspec(dllexport)
#else
#define LOCALAI_API __declspec(dllimport)
#endif
#else
#define LOCALAI_API
#endif