#pragma once
#include "localai_c_api.h"
#include "IPlugin.h"
#include <iostream>

extern "C" IPlugin* CreatePlugin();

extern "C" void DestroyPlugin(IPlugin* p);