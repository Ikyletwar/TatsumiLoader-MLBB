#pragma once
#include "include/ImGui/imgui.h"
#include "include/ImGui/imgui_internal.h"

namespace ImGui {
    bool RadioButton(const char* label, float* v, float v_button);
    bool CustomCheckbox(const char* label, bool* v);
}
