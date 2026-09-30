#pragma once
#include "include/ImGui/imgui.h"

bool Toggle(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = 35.0f;
    float width = height * 1.9f;
    float radius = height * 0.5f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    bool clicked = ImGui::IsItemClicked();
    if (clicked)
        *v = !*v;



    ImU32 col_bg = *v
    ? ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.95f))
    : ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 0.95f));

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, radius);

    float circle_x = *v ? (p.x + width - radius) : (p.x + radius);
    draw_list->AddCircleFilled(ImVec2(circle_x, p.y + radius), radius - 2.0f, IM_COL32(255, 0, 0, 255));

    ImGui::SameLine();
    ImGui::Text("%s", str_id);

    return clicked;
}

