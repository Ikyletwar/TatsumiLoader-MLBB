#pragma once
// TatsumiLoader - daftar setting TERPUSAT (single source of truth).
//
// CARA TAMBAH SETTING BARU (cukup 2 langkah):
//   1. Deklarasikan variabel global seperti biasa di main.cpp.
//   2. Tambah 1 baris X("key_cfg", variabel) di tabel tipe yang sesuai.
// Save + load otomatis ikut. Key JANGAN diubah kalau cfg lama harus kebaca.

#define TATSUMI_BOOL_SETTINGS                                                  \
  X("drawMHealth", drawMHealth)                                                 \
  X("drawMLine", drawMLine)                                                     \
  X("drawMAvatar", drawMAvatar)                                                 \
  X("drawMonsterHealth", drawMonsterHealth)                                     \
  X("drawMonsterID", drawMonsterID)                                             \
  X("drawMonsterName", drawMonsterName)                                         \
  X("iconhero", iconhero)                                                       \
  X("drawAlertUnderAttack", drawAlertUnderAttack)                               \
  X("EnableDrone", EnableDrone)                                                 \
  X("drawMPrediction", drawMPrediction)                                         \
  X("predictionColorBySpeed", predictionColorBySpeed)                           \
  X("predictionShowOnlyMoving", predictionShowOnlyMoving)                       \
  X("predictionShowArrowHead", predictionShowArrowHead)                         \
  X("predictionShowSpeedText", predictionShowSpeedText)                         \
  X("autoRetribution", autoRetribution)                                         \
  X("retriDoubleTap", retriDoubleTap)                                           \
  X("showRetriCircle", showRetriCircle)                                         \
  X("AutoRetributionRed", AutoRetributionRed)                                   \
  X("AutoRetributionBlue", AutoRetributionBlue)                                 \
  X("AutoRetributionLord", AutoRetributionLord)                                 \
  X("AutoRetributionTurtle", AutoRetributionTurtle)                             \
  X("AutoRetributionCrab", AutoRetributionCrab)                                 \
  X("AutoRetributionLito", AutoRetributionLito)                                 \
  X("AA_enabled", AutoAim::enabled)                                             \
  X("AA_showVisuals", AutoAim::showVisuals)                                     \
  X("AA_joyVisible", AutoAim::joyVisible)                                       \
  X("MinimapIcon", MinimapIcon)                                                 \
  X("MinimapIconBuff", MinimapIconBuff)                                         \
  X("MinimapIconLordTurtle", MinimapIconLordTurtle)                             \
  X("HideLine", HideLine)                                                       \
  X("drawMinionMinimap", drawMinionMinimap)                                     \
  X("ESP_Player_Cooldown", ESP_Player_Cooldown)                                 \
  X("Alert_ShowHPText", Alert_ShowHPText)                                       \
  X("autoSwordLing", autoSwordLing) \
  X("autoSpellExecute", autoSpellExecute) \
  X("drawObjectiveAlert", drawObjectiveAlert) \
  X("useAdGuardDns", useAdGuardDns)

#define TATSUMI_FLOAT_SETTINGS                                                 \
  X("FieldView", FieldView)                                                     \
  X("predictionCircleRadius", predictionCircleRadius)                           \
  X("retriTouchX", retriTouchX)                                                 \
  X("retriTouchY", retriTouchY)                                                 \
  X("retriEarlyMargin", retriEarlyMargin)                                       \
  X("retriMaxRange", retriMaxRange)                                             \
  X("AA_fovRadius", AutoAim::fovRadius)                                         \
  X("AA_smoothFactor", AutoAim::smoothFactor)                                   \
  X("AA_joyRadius", AutoAim::joyRadius)                                         \
  X("AA_joyDeadZone", AutoAim::joyDeadZone)                                     \
  X("AA_joyCenterX", AutoAim::joyCenterX)                                       \
  X("AA_joyCenterY", AutoAim::joyCenterY)                                       \
  X("minionMinimapSize", minionMinimapSize)                                     \
  X("g_Res0_MultX", g_Res0_MultX)                                               \
  X("g_Res0_MultY", g_Res0_MultY)                                               \
  X("g_Res1_OffsetX", g_Res1_OffsetX)                                           \
  X("g_Res1_OffsetY", g_Res1_OffsetY)                                           \
  X("g_MinimapScale", g_MinimapScale)                                           \
  X("g_HeroIconSize", g_HeroIconSize)                                           \
  X("Cooldown_OffsetY", Cooldown_OffsetY)                                       \
  X("Cooldown_Size", Cooldown_Size)                                             \
  X("Alert_PosX", Alert_PosX)                                                   \
  X("Alert_PosY", Alert_PosY)                                                   \
  X("Alert_Scale", Alert_Scale)                                                 \
  X("FloatingIcon_PosX", FloatingIcon_PosX)                                     \
  X("FloatingIcon_PosY", FloatingIcon_PosY)                                     \
  X("Avatar_OffsetY", Avatar_OffsetY)                                           \
  X("lingSwordDashRange", lingSwordDashRange)                                   \
  X("lingDashDelay", lingDashDelay)                                             \
  X("lingSkill2X", lingSkill2X)                                                 \
  X("lingSkill2Y", lingSkill2Y) \
  X("spellExecPct", spellExecPct) \
  X("spellRange", spellRange) \
  X("spellX", spellX) \
  X("spellY", spellY) \
  X("objectiveLowPct", objectiveLowPct)

#define TATSUMI_INT_SETTINGS                                                   \
  X("retriSpamMs", retriSpamMs)                                                 \
  X("AA_priorityMode", AutoAim::priorityMode)                                   \
  X("MinimapSize", MinimapSize)                                                 \
  X("MinimapPos", MinimapPos)                                                   \
  X("MinimapPosY", MinimapPosY)                                                 \
  X("g_MinimapHeroSize", g_MinimapHeroSize)                                     \
  X("g_MinimapMonsterSize", g_MinimapMonsterSize)                               \
  X("lingSwordID", lingSwordID)                                                 \
  X("gusionComboDelay", gusionComboDelay) \
  X("spellSpamMs", spellSpamMs)
