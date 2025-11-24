#pragma once

#if defined(_WIN32)
#define LAACHAR_T wchar_t
#ifdef CORE_BUILD
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#else
#define LAACHAR_T char
#define CORE_API
#endif