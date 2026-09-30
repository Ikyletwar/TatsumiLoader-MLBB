#pragma once
#include "imgui.h"
#include <string>

ImTextureID GetRankIconTexture(const std::string& name);
void ClearRankIconCache();
