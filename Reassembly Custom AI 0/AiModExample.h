#pragma once

// reassembly/"outlaws1" core headers
#include <core/Geometry.h>

// reassembly game headers
#include <game/AI_modapi.h>

extern "C" {

__declspec(dllexport) void GetApiVersion(int * major, int * minor);
__declspec(dllexport) bool CreateAiActions(AI* ai);

} // extern "C"
