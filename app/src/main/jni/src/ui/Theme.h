#pragma once
// TatsumiLoader - tema warna UI (accent-driven).
//
// Semua warna UI diturunkan dari SATU variabel aksen (g_accent).
// Ganti preset = ubah 1 angka di kPresets, seluruh UI ikut berubah.
//
// CARA TAMBAH PRESET (1 langkah):
//   1. Tambah 1 baris ke kPresets + 1 nama ke kPresetNames.
//   Tidak ada kode lain yang perlu disentuh.

#include "imgui.h"

// Arah warna yang dipakai shell: teks, chip nilai, rail slider, ikon sidebar.
namespace UI {

struct Accent {
  float r, g, b;
};

// Indeks = nilai setting "uiAccentPreset" yang tersimpan di cfg.
inline const Accent kPresets[] = {
    {198, 166, 106},  // 0 Champagne Gold - luxury, hangat (default)
    {45, 138, 140},   // 1 Deep Teal      - modern, profesional
    {129, 122, 200},  // 2 Slate Violet   - premium, ada karakter
    {224, 112, 58},   // 3 Ember          - hangat, bold
};
inline const char *kPresetNames[] = {
    "Champagne Gold",
    "Deep Teal",
    "Slate Violet",
    "Ember",
};
inline const int kPresetCount = 4;

// Aksen aktif. Di-set ulang tiap frame oleh ApplyTheme() dari setting cfg.
inline Accent g_accent = {198, 166, 106};

inline void SetAccent(int idx) {
  if (idx < 0) idx = 0;
  if (idx >= kPresetCount) idx = kPresetCount - 1;
  g_accent = kPresets[idx];
}

// Turunkan warna dari aksen. k<1 = redup, k>1 = lebih terang.
inline ImVec4 Shade(float k, float alpha = 1.0f) {
  const float r = g_accent.r / 255.0f;
  const float g = g_accent.g / 255.0f;
  const float b = g_accent.b / 255.0f;
  if (k <= 1.0f) {
    return ImVec4(r * k, g * k, b * k, alpha);
  }
  const float t = 1.0f - 1.0f / k;  // makin besar k, makin putih
  return ImVec4(r + (1.0f - r) * t, g + (1.0f - g) * t, b + (1.0f - b) * t, alpha);
}

// Turunkan abu-abu netral dari luminance aksen, supaya dark mode tetap
// menyatu dengan warna tema (bukan abu-abu mati).
inline ImVec4 Neutral(float k, float alpha = 1.0f) {
  const float lum = (g_accent.r * 0.30f + g_accent.g * 0.59f + g_accent.b * 0.11f) / 255.0f;
  float v = k;
  if (v > 1.0f) v = lum + (1.0f - lum) * (1.0f - 1.0f / k);
  return ImVec4(v, v, v * 1.02f, alpha);
}

// Base = warna aksen untuk elemen interaktif (slider, toggle aktif).
// Bright = versi lebih terang untuk teks aktif/hover.
// Dim = versi redup untuk border tipis & teks mati.
inline ImVec4 Base() { return Shade(1.0f); }
inline ImVec4 Bright() { return Shade(1.35f); }
inline ImVec4 Dim() { return Shade(0.55f); }
inline ImVec4 Faint() { return Shade(0.32f, 0.60f); }

// Warna dasar ImGui. Dipanggil tiap frame sebelum widget digambar.
inline void ApplyTheme() {
  ImGuiStyle &style = ImGui::GetStyle();

  // Ukuran setback agar widget baru (yang digambar manual) terasa lapang.
  style.WindowPadding = ImVec2(18, 16);
  style.ChildRounding = 10.0f;
  style.FramePadding = ImVec2(14, 10);
  style.FrameRounding = 8.0f;
  style.ItemSpacing = ImVec2(12, 10);
  style.ItemInnerSpacing = ImVec2(8, 6);
  style.IndentSpacing = 20.0f;
  style.ScrollbarSize = 12.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabMinSize = 22.0f;
  style.TouchExtraPadding = ImVec2(14, 14);
  style.TabRounding = 8.0f;
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowBorderSize = 1.0f;
  style.ChildBorderSize = 0.0f;
  style.ScrollbarRounding = 8.0f;

  ImVec4 *c = style.Colors;
  c[ImGuiCol_Text] = Neutral(0.92f);
  c[ImGuiCol_TextDisabled] = Neutral(0.45f);
  c[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.055f, 0.065f, 0.97f);
  c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
  c[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.98f);
  c[ImGuiCol_Border] = ImVec4(Shade(0.45f).x, Shade(0.45f).y, Shade(0.45f).z, 0.45f);
  c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
  c[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.115f, 0.60f);
  c[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.17f, 0.80f);
  c[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
  c[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.05f, 1.0f);
  c[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.10f, 1.0f);
  c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.60f);
  c[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.0f);
  c[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
  c[ImGuiCol_ScrollbarGrab] = Neutral(0.30f, 0.90f);
  c[ImGuiCol_ScrollbarGrabHovered] = Dim();
  c[ImGuiCol_ScrollbarGrabActive] = Base();
  c[ImGuiCol_CheckMark] = Bright();
  c[ImGuiCol_Button] = ImVec4(0.14f, 0.14f, 0.16f, 1.0f);
  c[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.23f, 1.0f);
  c[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.30f, 1.0f);
  c[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
  c[ImGuiCol_HeaderHovered] = ImVec4(0.21f, 0.21f, 0.24f, 1.0f);
  c[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.26f, 0.30f, 1.0f);
  c[ImGuiCol_Separator] = ImVec4(0.22f, 0.22f, 0.25f, 0.60f);
  c[ImGuiCol_SeparatorHovered] = Base();
  c[ImGuiCol_SeparatorActive] = Bright();
  c[ImGuiCol_ResizeGrip] = ImVec4(Base().x, Base().y, Base().z, 0.25f);
  c[ImGuiCol_ResizeGripHovered] = ImVec4(Bright().x, Bright().y, Bright().z, 0.55f);
  c[ImGuiCol_ResizeGripActive] = Base();
  c[ImGuiCol_SliderGrab] = Base();
  c[ImGuiCol_SliderGrabActive] = Bright();
  c[ImGuiCol_ScrollbarGrab] = Neutral(0.30f, 0.90f);
  c[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
  c[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.22f, 0.25f, 1.0f);
  c[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.32f, 1.0f);
  c[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
  c[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.23f, 1.0f);
  c[ImGuiCol_TextSelectedBg] = ImVec4(Base().x, Base().y, Base().z, 0.35f);
  c[ImGuiCol_DragDropTarget] = ImVec4(Bright().x, Bright().y, Bright().z, 0.90f);
  c[ImGuiCol_NavHighlight] = ImVec4(Base().x, Base().y, Base().z, 0.60f);
  c[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 1, 1, 0.70f);
  c[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  c[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.05f, 0.06f, 0.70f);
  c[ImGuiCol_TableHeaderBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.0f);
  c[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
  c[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
  c[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
  c[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.04f);
  c[ImGuiCol_PlotLines] = ImVec4(Base().x, Base().y, Base().z, 0.60f);
  c[ImGuiCol_PlotLinesHovered] = Bright();
  c[ImGuiCol_PlotHistogram] = ImVec4(Base().x, Base().y, Base().z, 0.40f);
  c[ImGuiCol_PlotHistogramHovered] = ImVec4(Bright().x, Bright().y, Bright().z, 0.80f);
}

}  // namespace UI
