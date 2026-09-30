#pragma once
#include "imgui.h"

// Mengambil tekstur skill berdasarkan HeroID dan Index Skill (1, 2, atau 3)
ImTextureID GetSkillIconTexture(int heroId, int skillIndex);
void ClearSkillIconCache();
