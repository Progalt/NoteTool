#pragma once

#include <chrono>
#include <cstdio>
#include "OS.h"

#define PROFILE_BEGIN(name) auto startTime_##name = std::chrono::high_resolution_clock::now()

#define PROFILE_END(name, strName) auto endTime_##name = std::chrono::high_resolution_clock::now(); \
OS::GetInstance().DebugPrint("Profile Time %s: %f \n", strName, std::chrono::duration<double, std::milli>(endTime_##name - startTime_##name).count() )