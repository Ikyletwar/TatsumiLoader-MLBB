#pragma once
#include "imgui.h"
#include "Decoder64.h"
#include <cmath>

inline void DrawHeroIcon(ImDrawList* drawList, ImVec2 pos, int heroId, int hp, int maxHp, float radius = 50.0f, bool isEnemy = true, int level = 0, int respawnTimer = 0) {
    ImTextureID tex = GetHeroTexture(heroId);

    // 1. Shadow & Glow effect for depth
    ImU32 glowColor = isEnemy ? IM_COL32(255, 0, 0, 60) : IM_COL32(0, 255, 100, 60);
    drawList->AddCircleFilled(pos, radius + 4.0f, glowColor, 40);
    drawList->AddCircleFilled(pos, radius + 2.0f, IM_COL32(0, 0, 0, 180), 40);

    // 2. Main Avatar Image with Border
    ImU32 iconTint = (respawnTimer > 0) ? IM_COL32(100, 100, 100, 255) : IM_COL32(255, 255, 255, 255);

    if (tex) {
        // Subtle background in case of transparent textures
        drawList->AddCircleFilled(pos, radius, IM_COL32(30, 30, 30, 255), 40);
        
        drawList->AddImageRounded(tex, 
            ImVec2(pos.x - radius, pos.y - radius), 
            ImVec2(pos.x + radius, pos.y + radius), 
            ImVec2(0,0), ImVec2(1,1), 
            iconTint, radius);
    } else {
        drawList->AddCircleFilled(pos, radius, IM_COL32(80, 80, 80, 255), 40);
        // Fallback text if texture missing
        char fallback[8];
        snprintf(fallback, sizeof(fallback), "%d", heroId);
        ImVec2 tSize = ImGui::CalcTextSize(fallback);
        drawList->AddText(ImVec2(pos.x - tSize.x*0.5f, pos.y - tSize.y*0.5f), IM_COL32_WHITE, fallback);
    }

    // 3. Thick Stylized Border
    ImU32 borderColor = isEnemy ? IM_COL32(200, 0, 0, 255) : IM_COL32(0, 200, 100, 255);
    if (respawnTimer > 0) borderColor = IM_COL32(80, 80, 80, 255); // Gray border if dead

    drawList->AddCircle(pos, radius, borderColor, 40, 2.5f);
    drawList->AddCircle(pos, radius + 1.0f, IM_COL32(0, 0, 0, 200), 40, 1.0f);

    // 4. Respawn Timer or HP Arc
    if (respawnTimer > 0) {
        // Darken overlay
        drawList->AddCircleFilled(pos, radius, IM_COL32(0, 0, 0, 150), 40);

        char respawnTxt[8];
        snprintf(respawnTxt, sizeof(respawnTxt), "%d", respawnTimer);
        ImVec2 tSize = ImGui::CalcTextSize(respawnTxt);

        // Shadow for text
        drawList->AddText(ImVec2(pos.x - tSize.x*0.5f + 1, pos.y - tSize.y*0.5f + 1), IM_COL32(0, 0, 0, 255), respawnTxt);
        // Main text
        drawList->AddText(ImVec2(pos.x - tSize.x*0.5f, pos.y - tSize.y*0.5f), IM_COL32(255, 255, 0, 255), respawnTxt);
    } else if (maxHp > 0) {
        float healthPercent = (float)hp / (float)maxHp;
        if (healthPercent > 1.0f) healthPercent = 1.0f;
        if (healthPercent < 0.0f) healthPercent = 0.0f;

        ImU32 hpColor;
        if (isEnemy) {
            hpColor = IM_COL32(255, 40, 40, 255); // Vibrant Red
        } else {
            if (healthPercent > 0.7f) hpColor = IM_COL32(0, 255, 120, 255); // Emerald Green
            else if (healthPercent > 0.3f) hpColor = IM_COL32(255, 200, 0, 255); // Amber
            else hpColor = IM_COL32(255, 50, 50, 255); // Red
        }

        float arcRadius = radius + 5.0f;
        float startAngle = -1.5708f; // Top
        float endAngle = startAngle + (healthPercent * 6.28318f);

        // Arc Background (Track)
        drawList->PathArcTo(pos, arcRadius, 0.0f, 6.28318f, 40);
        drawList->PathStroke(IM_COL32(0, 0, 0, 150), false, 3.5f);

        // HP Arc itself
        drawList->PathArcTo(pos, arcRadius, startAngle, endAngle, 40);
        drawList->PathStroke(hpColor, false, 3.5f);
        
        // Gloss effect on arc
        drawList->PathArcTo(pos, arcRadius - 0.5f, startAngle, endAngle, 40);
        drawList->PathStroke(IM_COL32(255, 255, 255, 80), false, 1.0f);
    }

    // 5. Level Badge (Bottom-Right)
    if (level > 0) {
        float badgeRadius = radius * 0.4f;
        ImVec2 badgePos = ImVec2(pos.x + radius * 0.75f, pos.y + radius * 0.75f);
        
        // Badge Background
        drawList->AddCircleFilled(badgePos, badgeRadius, IM_COL32(15, 15, 15, 240), 20);
        drawList->AddCircle(badgePos, badgeRadius, IM_COL32(200, 200, 200, 255), 20, 1.2f);
        
        // Level Text
        char lvl[4];
        snprintf(lvl, sizeof(lvl), "%d", level);
        ImVec2 textSize = ImGui::CalcTextSize(lvl);
        drawList->AddText(ImVec2(badgePos.x - textSize.x*0.5f, badgePos.y - textSize.y*0.5f), IM_COL32_WHITE, lvl);
    }
}

inline void DrawSpellIcon(ImDrawList* drawList, ImVec2 pos, int spellId, int cdTime, float radius = 25.0f) {
    ImTextureID tex = GetSpellTexture(spellId);

    // Background shadow
    drawList->AddCircleFilled(pos, radius + 2.0f, IM_COL32(0, 0, 0, 150), 30);

    if (tex) {
        drawList->AddImageRounded(tex,
            ImVec2(pos.x - radius, pos.y - radius),
            ImVec2(pos.x + radius, pos.y + radius),
            ImVec2(0,0), ImVec2(1,1),
            IM_COL32(255,255,255,255), radius);
    } else {
        // Fallback: draw a colored circle with ID
        drawList->AddCircleFilled(pos, radius, IM_COL32(100, 100, 100, 255), 30);
        char idStr[16];
        snprintf(idStr, sizeof(idStr), "%d", spellId);
        ImVec2 tSize = ImGui::CalcTextSize(idStr);
        drawList->AddText(ImVec2(pos.x - tSize.x * 0.5f, pos.y - tSize.y * 0.5f), IM_COL32_WHITE, idStr);
    }

    // Border
    drawList->AddCircle(pos, radius, IM_COL32(255, 255, 255, 200), 30, 1.5f);

    // Cooldown Overlay
    if (cdTime > 0) {
        // Darken the icon
        drawList->AddCircleFilled(pos, radius, IM_COL32(0, 0, 0, 180), 30);

        // Cooldown Text
        char cdStr[8];
        snprintf(cdStr, sizeof(cdStr), "%d", cdTime);
        ImVec2 tSize = ImGui::CalcTextSize(cdStr);
        drawList->AddText(ImVec2(pos.x - tSize.x * 0.5f, pos.y - tSize.y * 0.5f), IM_COL32_WHITE, cdStr);
    }
}
