#pragma once

// reassembly/"outlaws1" core headers
#include <core/Geometry.h>

// reassembly game headers
#include <game/AI_modapi.h>
#include <game/Blocks.h>

extern "C" {

// __declspec(dllexport) bool SupportsConfig(const char * name, const AICommandConfig& cfg);

__declspec(dllexport) AIAction * CreateAiAction(const char * name, AI* ai);

} // extern "C"
