#pragma once
// TatsumiLoader - isi 6 halaman konten.
//
// Berkas ini WAJIB di-#include dari main.cpp SESUDAH semua variabel global
// setting dideklarasikan (file ini memakai variabel tersebut langsung).
//
// POLA: tiap halaman = 1 HeaderBar + beberapa kartu accordion (UI::Section).
// Semua nilai/range setting harus SAMA PERSIS dengan UI lama supaya cfg lama
// tetap kompatibel.

#include "Shell.h"
#include "Theme.h"

namespace Pages {

// ------------------------------------------------------------------
// AUTO  (halaman paling padat -> paling detail)
// ------------------------------------------------------------------
inline void Auto() {
  UI::HeaderBar(xorstr_("AUTO"), xorstr_("Auto retri, auto spell, dan auto aim"));

  // ---- 1. RETRIBUTION ----
  if (UI::Section(xorstr_("RETRIBUTION"), xorstr_("auto Lord / Turtle"), 0, 0)) {
    UI::Toggle(xorstr_("Auto Retribution"), &autoRetribution,
               xorstr_("Tekan skill Lord / Turtle otomatis saat CD siap"));
    UI::Toggle(xorstr_("Show Circle"), &showRetriCircle,
               xorstr_("Tampilkan lingkaran titik sentuh"));
    UI::Note(xorstr_("POSISI SENTUH"));
    UI::SliderF(xorstr_("Retri X"), &retriTouchX, 0.0f, 3000.0f, "px", "%.0f");
    UI::SliderF(xorstr_("Retri Y"), &retriTouchY, 0.0f, 1500.0f, "px", "%.0f");
    UI::Note(xorstr_("TUNING"));
    UI::SliderF(xorstr_("Max Range"), &retriMaxRange, 5.0f, 12.0f, "m");
    UI::SliderF(xorstr_("Early Margin"), &retriEarlyMargin, 0.0f, 500.0f, "dmg", "%.0f");
    UI::SliderI(xorstr_("Spam Interval"), &retriSpamMs, 30, 300, "ms");
    UI::Note(xorstr_("BEHAVIOUR"));
    UI::Toggle(xorstr_("Double Tap"), &retriDoubleTap);
    UI::Toggle(xorstr_("Nearby Enemy Only"), &retriNearbyOnly);
    if (retriNearbyOnly) {
      UI::SliderF(xorstr_("Nearby Range"), &retriNearbyRange, 3.0f, 30.0f, "m");
    }
    UI::EndSection();
  }

  // ---- 2. TARGETS ----
  if (UI::Section(xorstr_("TARGET MONSTERS"), xorstr_("6 target"), 0, 1)) {
    UI::Toggle(xorstr_("Red Buff"), &AutoRetributionRed);
    UI::Toggle(xorstr_("Blue Buff"), &AutoRetributionBlue);
    UI::Toggle(xorstr_("Lord"), &AutoRetributionLord);
    UI::Toggle(xorstr_("Turtle"), &AutoRetributionTurtle);
    UI::Toggle(xorstr_("Crab"), &AutoRetributionCrab);
    UI::Toggle(xorstr_("Lito"), &AutoRetributionLito);
    UI::EndSection();
  }

  // ---- 3. AUTO SPELL ----
  if (UI::Section(xorstr_("AUTO SPELL (EXECUTE)"), xorstr_("kill / execute"), 0, 2)) {
    UI::Toggle(xorstr_("Enable Auto Execute"), &autoSpellExecute,
               xorstr_("Lempar skill saat HP musuh di bawah ambang"));
    if (autoSpellExecute) {
      UI::SliderF(xorstr_("Execute HP %"), &spellExecPct, 1.0f, 40.0f, "%");
      UI::SliderF(xorstr_("Execute Range"), &spellRange, 1.0f, 10.0f, "m");
      UI::SliderF(xorstr_("Spell X"), &spellX, 0.0f, 3000.0f, "px", "%.0f");
      UI::SliderF(xorstr_("Spell Y"), &spellY, 0.0f, 1500.0f, "px", "%.0f");
      UI::SliderI(xorstr_("Spell Spam"), &spellSpamMs, 300, 5000, "ms");
    }
    UI::EndSection();
  }

  // ---- 4. LING AUTO SWORD ----
  if (UI::Section(xorstr_("LING AUTO SWORD"), xorstr_("dash + skill 2"), 0, 3)) {
    UI::Toggle(xorstr_("Enable Auto Sword"), &autoSwordLing);
    if (autoSwordLing) {
      UI::SliderF(xorstr_("Dash Range"), &lingSwordDashRange, 1.0f, 20.0f, "m");
      UI::SliderF(xorstr_("Dash Delay"), &lingDashDelay, -0.1f, 1.0f, "s", "%.2f");
      UI::SliderF(xorstr_("Skill 2 X"), &lingSkill2X, 0.0f, (float)abs_ScreenX, "px", "%.0f");
      UI::SliderF(xorstr_("Skill 2 Y"), &lingSkill2Y, 0.0f, (float)abs_ScreenY, "px", "%.0f");
      if (UI::ActionBtn(xorstr_("RESET SKILL 2 POSITION"))) {
        lingSkill2X = abs_ScreenX * 0.85f;
        lingSkill2Y = abs_ScreenY * 0.75f;
      }    }
    UI::EndSection();
  }

  // ---- 5. GUSION AUTO COMBO ----
  if (UI::Section(xorstr_("GUSION AUTO COMBO"), xorstr_("1-2-1-2-3"), 0, 4)) {
    UI::Toggle(xorstr_("Enable Auto Combo"), &autoComboGusion,
               xorstr_("Urutan 1-2-1-2-3 dengan reset otomatis"));
    if (autoComboGusion) {
      UI::SliderI(xorstr_("Combo Speed"), &gusionComboDelay, 10, 500, "ms");
      UI::SliderF(xorstr_("Skill 2 X"), &gusionSkill2X, 0.0f, (float)abs_ScreenX, "px", "%.0f");
      UI::SliderF(xorstr_("Skill 2 Y"), &gusionSkill2Y, 0.0f, (float)abs_ScreenY, "px", "%.0f");
      UI::SliderF(xorstr_("Skill 3 X"), &gusionSkill3X, 0.0f, (float)abs_ScreenX, "px", "%.0f");
      UI::SliderF(xorstr_("Skill 3 Y"), &gusionSkill3Y, 0.0f, (float)abs_ScreenY, "px", "%.0f");
      if (UI::ActionBtn(xorstr_("RESET GUSION BUTTONS"))) {
        gusionSkill1X = abs_ScreenX * 0.85f; gusionSkill1Y = abs_ScreenY * 0.75f;
        gusionSkill2X = abs_ScreenX * 0.75f; gusionSkill2Y = abs_ScreenY * 0.85f;
        gusionSkill3X = abs_ScreenX * 0.70f; gusionSkill3Y = abs_ScreenY * 0.65f;
      }
    }
    UI::EndSection();
  }

  // ---- 6. DD KIMMY AUTO AIM ----
  if (UI::Section(xorstr_("DD KIMMY AUTO AIM"), xorstr_("target priority"), 0, 5)) {
    UI::Toggle(xorstr_("Enable Kimmy Aim"), &AutoAim::enabled);
    if (AutoAim::enabled) {
      UI::SliderF(xorstr_("FOV Radius"), &AutoAim::fovRadius, 50.0f, 1500.0f, "px", "%.0f");
      UI::SliderF(xorstr_("Smooth Factor"), &AutoAim::smoothFactor, 0.01f, 0.5f, "", "%.2f");
      static const char *kPrio[] = {"Closest", "Lowest HP", "Player Priority"};
      ImGui::Combo(xorstr_("Target Priority"), &AutoAim::priorityMode, kPrio, 3);
      UI::Toggle(xorstr_("Show Visuals"), &AutoAim::showVisuals);
    }
    UI::Note(xorstr_("JOYSTICK"));
    UI::Toggle(xorstr_("Show Joystick UI"), &AutoAim::joyVisible);
    if (AutoAim::joyVisible || AutoAim::enabled) {
      UI::SliderF(xorstr_("Joy Radius"), &AutoAim::joyRadius, 100.0f, 300.0f, "px", "%.0f");
      UI::SliderF(xorstr_("Joy X"), &AutoAim::joyCenterX, 0.0f, (float)abs_ScreenX, "px", "%.0f");
      UI::SliderF(xorstr_("Joy Y"), &AutoAim::joyCenterY, 0.0f, (float)abs_ScreenY, "px", "%.0f");
      if (UI::ActionBtn(xorstr_("RESET JOYSTICK TO RIGHT"))) {
        AutoAim::joyCenterX = abs_ScreenX * 0.85f;
        AutoAim::joyCenterY = abs_ScreenY * 0.75f;
      }
    }
    UI::EndSection();
  }
}

// ------------------------------------------------------------------
// ESP
// ------------------------------------------------------------------
inline void Esp() {
  UI::HeaderBar(xorstr_("ESP"), xorstr_("Player, cooldown, dan notifier ulti/retri"));

  if (UI::Section(xorstr_("PLAYER ESP"), xorstr_("garis + avatar"), 1, 0)) {
    UI::Toggle(xorstr_("ESP Line"), &drawMLine, xorstr_("Garis dari hero ke layar"));
    UI::Toggle(xorstr_("Hero ESP"), &iconhero, xorstr_("Avatar + level di atas kepala"));
    if (iconhero || ESP_Player_Cooldown) {
      UI::SliderF(xorstr_("ESP Height"), &Avatar_OffsetY, 0.0f, 500.0f, "px", "%.0f");
      UI::SliderF(xorstr_("Icon Size"), &g_HeroIconSize, 10.0f, 60.0f, "px", "%.0f");
    }
    UI::Toggle(xorstr_("Prediction ESP"), &drawMPrediction);
    UI::EndSection();
  }

  if (UI::Section(xorstr_("COOLDOWN & DRONE"), xorstr_("cd skill + fov"), 1, 1)) {
    UI::Toggle(xorstr_("Enemy Cooldown"), &ESP_Player_Cooldown);
    if (ESP_Player_Cooldown) {
      UI::SliderF(xorstr_("CD Size"), &Cooldown_Size, 5.0f, 40.0f, "px", "%.0f");
    }
    UI::Note(xorstr_("DRONE"));
    UI::Toggle(xorstr_("Enable Drone"), &EnableDrone);
    if (EnableDrone) UI::SliderF(xorstr_("FOV"), &FieldView, -9.0f, -1.0f, "", "%.1f");
    UI::EndSection();
  }

  if (UI::Section(xorstr_("ULTI & RETRI ALERT"), xorstr_("notifier musuh"), 1, 2)) {
    SkillAlert::RenderSettings();
    UI::EndSection();
  }
}

// ------------------------------------------------------------------
// VISUAL
// ------------------------------------------------------------------
inline void Visual() {
  UI::HeaderBar(xorstr_("VISUAL"), xorstr_("Monster, minimap, dan alert"));

  if (UI::Section(xorstr_("MONSTERS"), xorstr_("nama + ikon"), 2, 0)) {
    UI::Toggle(xorstr_("Monster ESP"), &drawMonsterName);
    if (drawMonsterName) {
      UI::SliderF(xorstr_("Monster Icon Size"), &g_MonsterIconSize, 10.0f, 60.0f, "px", "%.0f");
    }
    UI::EndSection();
  }

  if (UI::Section(xorstr_("MINIMAP"), xorstr_("overlay radar"), 2, 1)) {
    UI::Toggle(xorstr_("Enable Minimap"), &MinimapIcon);
    UI::Toggle(xorstr_("Show Monsters"), &drawMonsterMinimap);
    UI::Toggle(xorstr_("Show Lord/Turtle"), &MinimapIconLordTurtle);
    UI::Toggle(xorstr_("Hide Minimap Line"), &HideLine);
    UI::Toggle(xorstr_("Show All Monsters"), &MinimapIconBuff);
    UI::Toggle(xorstr_("Show Minion Dot"), &drawMinionMinimap);
    UI::Note(xorstr_("GEOMETRI"));
    UI::SliderI(xorstr_("Minimap Size"), &MinimapSize, 100, 600, "px");
    UI::SliderI(xorstr_("Minimap X"), &MinimapPos, 0, 1000, "px");
    UI::SliderI(xorstr_("Minimap Y"), &MinimapPosY, 0, 1000, "px");
    UI::SliderI(xorstr_("Hero Icon Size"), &g_MinimapHeroSize, 10, 100, "px");
    UI::SliderI(xorstr_("Monster Icon Size"), &g_MinimapMonsterSize, 10, 100, "px");
    UI::SliderI(xorstr_("Minion Dot Size"), &g_ICSize, 2, 20, "px");
    // Slot 6 = sub-kartu, terpisah dari slot kartu MINIMAP (page 2).
    if (UI::Section(xorstr_("Calibration"), nullptr, 6, 0)) {
      UI::SliderF(xorstr_("Scale"), &g_MinimapScale, 10.0f, 200.0f, "", "%.1f");
      UI::SliderF(xorstr_("Offset X"), &g_Res1_OffsetX, -100.0f, 100.0f, "px", "%.1f");
      UI::SliderF(xorstr_("Offset Y"), &g_Res1_OffsetY, -100.0f, 100.0f, "px", "%.1f");
      UI::EndSection();
    }
    UI::EndSection();
  }

  if (UI::Section(xorstr_("ALERT WARNING"), xorstr_("lord/turtle + skill"), 2, 2)) {
    UI::Toggle(xorstr_("Show Alert"), &drawAlertUnderAttack);
    UI::Toggle(xorstr_("Show Alert HP"), &Alert_ShowHPText);
    UI::SliderF(xorstr_("Alert X"), &Alert_PosX, 0.0f, 1.0f, "", "%.2f");
    UI::SliderF(xorstr_("Alert Y"), &Alert_PosY, 0.0f, 1.0f, "", "%.2f");
    UI::SliderF(xorstr_("Alert Scale"), &Alert_Scale, 0.5f, 3.0f, "x", "%.2f");
    UI::EndSection();
  }

  if (UI::Section(xorstr_("OBJECTIVE ALERT"), xorstr_("low HP"), 2, 3)) {
    UI::Toggle(xorstr_("Lord/Turtle Alert"), &drawObjectiveAlert);
    UI::SliderF(xorstr_("Low HP %"), &objectiveLowPct, 5.0f, 50.0f, "%");
    UI::EndSection();
  }
}

// ------------------------------------------------------------------
// ROOM -> lewat modul yang sudah ada (tidak disentuh)
// ------------------------------------------------------------------
inline void Room() { RenderRoomPlayerInfoImGui(); }

// ------------------------------------------------------------------
// INFO
// ------------------------------------------------------------------
inline void Banner(const char *title, const char *sub) {
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = UI::Fh();
  const float h = fh * 2.0f + 30.0f;
  ImVec2 p = ImGui::GetCursorScreenPos();
  ImDrawList *dl = ImGui::GetWindowDrawList();
  const ImVec2 a = p, b = ImVec2(p.x + w, p.y + h);

  dl->AddRectFilledMultiColor(a, b, UI::Col(UI::Shade(0.85f, 0.28f)),
                              UI::Col(UI::Shade(0.35f, 0.20f)),
                              UI::Col(UI::Shade(0.35f, 0.20f)),
                              UI::Col(UI::Shade(0.85f, 0.28f)));
  dl->AddRect(a, b, UI::Col(UI::Shade(0.55f, 0.60f)), 12.0f, 0, 1.5f);

  const float tw = ImGui::CalcTextSize(title).x;
  ImGui::SetCursorScreenPos(ImVec2(p.x + (w - tw) * 0.5f, p.y + 14.0f));
  ImGui::TextColored(UI::Bright(), "%s", title);
  if (sub && sub[0]) {
    const float sw = ImGui::CalcTextSize(sub).x;
    ImGui::SetCursorScreenPos(ImVec2(p.x + (w - sw) * 0.5f, p.y + 16.0f + fh));
    ImGui::TextColored(UI::Shade(1.15f, 0.90f), "%s", sub);
  }
  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  ImGui::Spacing();
}

inline void SocialBtn(const char *label, const char *url, int kind) {
  const float bw = (ImGui::GetContentRegionAvail().x - 10.0f) * 0.5f;
  if (ImGui::Button(label, ImVec2(bw, 42))) OpenURL(url);
  ImVec2 mn = ImGui::GetItemRectMin(), mx = ImGui::GetItemRectMax();
  ImDrawList *fg = ImGui::GetWindowDrawList();
  ImVec2 c = ImVec2(mn.x + 24.0f, (mn.y + mx.y) * 0.5f);
  const ImU32 white = IM_COL32(255, 255, 255, 255);
  if (kind == 0) {  // instagram
    fg->AddRect(ImVec2(c.x - 8.0f, c.y - 8.0f), ImVec2(c.x + 8.0f, c.y + 8.0f), white, 3.5f, 0, 2.0f);
    fg->AddCircle(c, 3.5f, white, 16, 2.0f);
    fg->AddCircleFilled(ImVec2(c.x + 5.5f, c.y - 5.5f), 1.6f, white);
  } else {  // tiktok
    fg->AddLine(ImVec2(c.x + 3.0f, c.y - 8.0f), ImVec2(c.x + 3.0f, c.y + 6.0f), white, 2.5f);
    fg->AddCircleFilled(ImVec2(c.x - 1.0f, c.y + 6.0f), 4.0f, white);
    fg->AddLine(ImVec2(c.x + 3.0f, c.y - 8.0f), ImVec2(c.x + 8.0f, c.y - 5.0f), white, 2.5f);
  }
}

inline void Info() {
  UI::HeaderBar(xorstr_("INFO"), xorstr_("Status lisensi dan perangkat"));

  Banner(xorstr_("PREMIUM SUBSCRIPTION"), xorstr_("Tersambung - thank you for supporting"));

  if (UI::Section(xorstr_("DEVICE"), xorstr_("informasi perangkat"), 4, 0)) {
    char model[PROP_VALUE_MAX] = {0};
    __system_property_get(xorstr_("ro.product.model"), model);
    UI::Kv(xorstr_("Model"), model, UI::Neutral(0.95f));
    UI::Kv(xorstr_("Package"), g_package_name.c_str(), UI::Neutral(0.85f));
    UI::Kv(xorstr_("Injection"), is_attached ? xorstr_("Active") : xorstr_("Pending"),
           is_attached ? UI::Shade(1.2f) : ImVec4(0.85f, 0.35f, 0.35f, 1.0f));
    UI::Kv(xorstr_("Version"), kTatsumiVersion, UI::Base());
    UI::EndSection();
  }

  if (UI::Section(xorstr_("FOLLOW US"), xorstr_("channel resmi"), 4, 1)) {
    SocialBtn(xorstr_("  Instagram @ikyletwar"),
              xorstr_("https://www.instagram.com/ikyletwar/"), 0);
    ImGui::SameLine();
    SocialBtn(xorstr_("  TikTok @ikyletwar"),
              xorstr_("https://www.tiktok.com/@ikyletwar"), 1);
    UI::EndSection();
  }

  ImGui::Spacing();
  ImGui::TextDisabled(xorstr_("Copyright (c) 2026 @TatsumiLoader"));
}

// ------------------------------------------------------------------
// SETTINGS
// ------------------------------------------------------------------
inline void Settings() {
  UI::HeaderBar(xorstr_("SETTINGS"), xorstr_("Config, tampilan, dan sistem"));

  if (UI::Section(xorstr_("CONFIG"), xorstr_("game + config file"), 5, 0)) {
    if (UI::ActionBtn(xorstr_("RE-SCAN GAME"), true)) AttachToGame();
    UI::Note(is_attached ? xorstr_("Status: READY (auto-attached)")
                         : xorstr_("Status: SEARCHING GAME..."));
    ImGui::Spacing();
    if (UI::ActionBtn(xorstr_("SAVE SETTINGS"))) SaveTatsumiSettings();
    if (UI::ActionBtn(xorstr_("LOAD SETTINGS"))) LoadTatsumiSettings();
    UI::EndSection();
  }

  if (UI::Section(xorstr_("SYSTEM"), xorstr_("tampilan + proses"), 5, 1)) {
    UI::SliderF(xorstr_("UI Scale"), &uiUserScale, 0.8f, 1.6f, "x", "%.2f");
    ImGui::Combo(xorstr_("Accent Preset"), &uiAccentPreset, UI::kPresetNames, UI::kPresetCount);
    {
      static const char *kFps[] = {"30 FPS", "60 FPS", "90 FPS", "120 FPS"};
      static const int kFpsVal[] = {30, 60, 90, 120};
      int fpsIdx = 1;
      for (int i = 0; i < 4; i++)
        if (uiFpsCap == kFpsVal[i]) fpsIdx = i;
      if (ImGui::Combo(xorstr_("UI FPS Cap"), &fpsIdx, kFps, 4)) uiFpsCap = kFpsVal[fpsIdx];
    }
    static float opacity = 1.0f;
    if (UI::SliderF(xorstr_("UI Opacity"), &opacity, 0.1f, 1.0f, "", "%.2f")) {
      ImGui::GetStyle().Alpha = opacity;
    }
    UI::Toggle(xorstr_("AdGuard DNS"), &useAdGuardDns,
               xorstr_("Resolvable lewat DNS AdGuard"));
    ImGui::Spacing();
    if (UI::ActionBtn(xorstr_("EXIT CHEAT"))) main_thread_flag = false;
    UI::EndSection();
  }
}

}  // namespace Pages
