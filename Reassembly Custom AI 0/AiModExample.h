#pragma once

// reassembly/"outlaws1" core headers
#include <core/Geometry.h>

// reassembly game headers
#include <game/AI_modapi.h>

extern "C" {

__declspec(dllexport) bool CreateAiActions(int versionMajor, int versionMinor, AI* ai);

} // extern "C"
