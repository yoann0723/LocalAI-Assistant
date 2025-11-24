#pragma once

#include "AIModelHub.h"
#include <IPlugin.h>
#include <iostream>

extern "C" IPlugin* CreatePlugin();

extern "C" void DestroyPlugin(IPlugin* p);