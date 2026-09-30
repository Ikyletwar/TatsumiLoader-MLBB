#define IMGUI_DEFINE_MATH_OPERATORS
#include "GUI_Custom.h"

bool ImGui::RadioButton(const char* label, float* v, float v_button)
{
    const bool pressed = RadioButton(label, *v == v_button);
    if (pressed)
        *v = v_button;
    return pressed;
}

bool ImGui::CustomCheckbox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), square_sz + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

    // Background kotak pas aktif (Merah) atau nggak aktif (Gelap)
    ImU32 col_bg = *v ? IM_COL32(255, 0, 0, 255) : IM_COL32(30, 30, 30, 255);

    window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, col_bg, 4.0f); // 4.0f itu rounded corner

    if (*v)
    {
        // Gambar tanda centang (V) warna hitam/gelap di dalam kotak merah
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
        RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), IM_COL32(0, 0, 0, 255), square_sz - pad * 2.0f);
    }

    if (label_size.x > 0.0f)
    {
        ImU32 col_text = *v ? IM_COL32(255, 255, 255, 255) : IM_COL32(150, 150, 150, 255);
        PushStyleColor(ImGuiCol_Text, col_text);
        RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);
        PopStyleColor();
    }

    return pressed;
}
