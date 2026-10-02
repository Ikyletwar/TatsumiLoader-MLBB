#include "SkillAlert.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#include "imgui.h"
#include "ui/Shell.h"
#include "utils/xorstr.hpp"
// Deklarasi saja: definisinya ada di Includes/GUI_Custom.cpp, path header-nya
// tidak ada di LOCAL_C_INCLUDES jadi tidak boleh di-#include.
namespace ImGui {
bool CustomCheckbox(const char *label, bool *v);
}
// Header-nya tidak bisa di-#include: ToString.h tidak punya include sendiri dan
// memakai to_string tanpa namespace, jadi hanya jalan kalau dipaksakan dari
// using-namespace ::std. Jadi cukup deklarasi, definisinya tetap milik main.cpp.
std::string HeroToString(int heroid);

// Diambil dari main.cpp (file variable, bukan static) supayaAlert Scale,
// posisi, dan ukuran layar cuma punya satu sumber kebenaran.
extern int abs_ScreenX;
extern int abs_ScreenY;
extern float Alert_PosX;
extern float Alert_PosY;
extern float Alert_Scale;

// Setting milik modul ini (didaftarkan di core/Settings.h).
bool ultReadyAlert = true;
bool retriReadyAlert = true;
bool ultCastAlert = false;
bool retriCastAlert = false;
int skillAlertMs = 4000;

namespace {

using Clock = std::chrono::steady_clock;

constexpr uint8_t kUnknown = 0;
constexpr uint8_t kReady = 1;
constexpr uint8_t kCooling = 2;

// Hero yang tidak terlihat selama ini dianggap keluar dari spectator/mati
// dan state-nya dibuang tanpa menambah lastSeenMs.
constexpr uint64_t kStaleMs = 3000;
// Setelah ini tidak terlihat, hapus dari peta (match baru / hero ganti).
constexpr uint64_t kPruneMs = 60000;
// Durasi blink border saat alert baru muncul.
constexpr int kBlinkMs = 1000;
constexpr int kMaxAlerts = 8;

// Tahap cooldown per skill: 0 unknown, 1 siap, 2 sedang cooldown.
struct HeroWatch {
  uint8_t ult;
  uint8_t retri;
  uint64_t lastSeenMs;

  HeroWatch() : ult(kUnknown), retri(kUnknown), lastSeenMs(0) {}
};

// Nama "AlertBox" bukan "SkillAlert" supaya tidak bentrok sama namespace
// SkillAlert di file yang sama.
struct AlertBox {
  std::string text;
  std::string sub;
  ImU32 color;
  Clock::time_point expiry;
  Clock::time_point born;
};

std::unordered_map<int, HeroWatch> g_watch;
std::vector<AlertBox> g_alerts;

// Samakan normalisasi ID dengan SpellIcons_loader (2002/20021 -> 20020).
int NormSpellId(int id) {
  if (id > 20000) return (id / 10) * 10;
  if (id > 2000) return id * 10;
  if (id > 0 && id < 100) return 20000 + (id * 10);
  return id;
}

bool IsRetriSpell(int id) { return NormSpellId(id) == 20020; }

uint64_t NowMs() {
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          Clock::now().time_since_epoch())
          .count());
}

// Ganti alert yang sama (teks identik) daripada menambah duplikat.
void Push(const std::string &text, const std::string &sub, ImU32 color) {
  const auto now = Clock::now();
  const auto ttl = std::chrono::milliseconds(
      skillAlertMs > 200 ? skillAlertMs : 200);
  for (auto &a : g_alerts) {
    if (a.text == text && now < a.expiry) {
      a.expiry = now + ttl;
      return;
    }
  }
  g_alerts.push_back({text, sub, color, now + ttl, now});
  while (g_alerts.size() > kMaxAlerts) g_alerts.erase(g_alerts.begin());
}

// Satu-satunya tempat keputusan. FROM/TO adalah tahap sebelum & sesudah.
// 0 = unknown -> seed, tidak pernah menembak (anti false positive).
void Step(uint8_t &state, uint8_t to, bool notifyReady, bool notifyCast,
          const char *label, int heroId, const char *readySub,
          const char *castSub, ImU32 readyColor, ImU32 castColor) {
  const uint8_t from = state;
  state = to;
  if (from == kUnknown || from == to)
    return;
  const std::string hero = HeroToString(heroId);
  if (to == kCooling && notifyCast) {
    Push(std::string(label) + ": " + hero, castSub, castColor);
  } else if (to == kReady && notifyReady) {
    Push(std::string(label) + ": " + hero, readySub, readyColor);
  }
}

}  // namespace

namespace SkillAlert {

void Reset() {
  g_watch.clear();
  g_alerts.clear();
}

void Tick(int heroId, const SkillCdView &cd) {
  if (heroId <= 0 || heroId >= 1000)
    return;

  const uint64_t nowMs = NowMs();

  // Hero yang terlalu lama tak terlihat: buang state diam-diam supaya pas
  // respawn (CD reset ke 0) tidak memicu alert "READY" palsu.
  for (auto it = g_watch.begin(); it != g_watch.end();) {
    if (nowMs - it->second.lastSeenMs > kStaleMs) {
      it->second.ult = kUnknown;
      it->second.retri = kUnknown;
    }
    it = (nowMs - it->second.lastSeenMs > kPruneMs) ? g_watch.erase(it)
                                                    : std::next(it);
  }

  HeroWatch &w = g_watch[heroId];
  w.lastSeenMs = nowMs;

  const uint8_t ultNow = cd.ultiSec > 0 ? kCooling : kReady;
  Step(w.ult, ultNow, ultReadyAlert, ultCastAlert, xorstr_("ULTI"), heroId,
       xorstr_("ultimate musuh siap"), xorstr_("ultimate musuh ditekan"),
       IM_COL32(255, 180, 0, 255), IM_COL32(255, 60, 60, 255));

  // Retri cuma relevan untuk musuh yang battle spell-nya retribution.
  if (cd.spellId > 0 && IsRetriSpell(cd.spellId)) {
    const uint8_t retriNow = cd.spellSec > 0 ? kCooling : kReady;
    Step(w.retri, retriNow, retriReadyAlert, retriCastAlert, xorstr_("RETRI"),
         heroId, xorstr_("retri musuh siap"), xorstr_("retri musuh ditekan"),
         IM_COL32(0, 200, 255, 255), IM_COL32(255, 0, 180, 255));
  }
}

void RenderSettings() {
  UI::Note(xorstr_("READY = skill siap, CAST = skill sudah ditekan musuh"));
  UI::Toggle(xorstr_("Ulti Ready Alert"), &ultReadyAlert,
             xorstr_("Kotak muncul saat ulti musuh siap"));
  UI::Toggle(xorstr_("Retri Ready Alert"), &retriReadyAlert,
             xorstr_("Kotak muncul saat retri musuh siap"));
  UI::Toggle(xorstr_("Ulti Cast Alert"), &ultCastAlert,
             xorstr_("Kotak muncul saat ulti musuh ditekan"));
  UI::Toggle(xorstr_("Retri Cast Alert"), &retriCastAlert,
             xorstr_("Kotak muncul saat retri musuh ditekan"));
  UI::Note(xorstr_("Ukuran mengikuti slider Alert Scale di tab VISUAL"));
  UI::SliderI(xorstr_("Duration"), &skillAlertMs, 1000, 15000, "ms");
}

void Draw(ImDrawList *draw) {
  if (!draw)
    return;
  const auto now = Clock::now();
  g_alerts.erase(std::remove_if(g_alerts.begin(), g_alerts.end(),
                                [&](const AlertBox &a) {
                                  return now >= a.expiry;
                                }),
                 g_alerts.end());
  if (g_alerts.empty())
    return;

  // Sengaja pakai Alert_Scale yang sama dengan DrawGlobalWarning supaya satu
  // slider "Alert Scale" mengatur alert Lord/Turtle DAN alert ulti/retri.
  const float centerX = abs_ScreenX * Alert_PosX;
  const float centerY = abs_ScreenY * Alert_PosY;
  const float s = Alert_Scale;

  const float boxW = 360.0f * s, boxH = 54.0f * s, gap = 8.0f * s;
  float y = centerY + 35.0f * s + 14.0f;  // di bawah box objective
  const size_t n = std::min<size_t>(g_alerts.size(), kMaxAlerts);

  for (size_t i = 0; i < n; i++) {
    const AlertBox &a = g_alerts[g_alerts.size() - n + i];
    ImVec2 bMin(centerX - boxW * 0.5f, y), bMax(centerX + boxW * 0.5f, y + boxH);
    draw->AddRectFilled(bMin, bMax, IM_COL32(15, 15, 15, 230), 10.0f * s);

    const long ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - a.born)
                           .count();
    const bool blink = ageMs < kBlinkMs && ((ageMs / 250) % 2 == 0);
    draw->AddRect(bMin, bMax, blink ? IM_COL32(255, 255, 255, 255) : a.color,
                  10.0f * s, 0, 2.5f * s);

    ImVec2 tSize = ImGui::CalcTextSize(a.text.c_str());
    draw->AddText(NULL, 19.0f * s,
                  ImVec2(centerX - tSize.x * s * 0.5f, y + 8.0f * s),
                  IM_COL32(255, 255, 255, 255), a.text.c_str());
    ImVec2 sSize = ImGui::CalcTextSize(a.sub.c_str());
    draw->AddText(NULL, 13.0f * s,
                  ImVec2(centerX - sSize.x * s * 0.5f, y + 30.0f * s),
                  IM_COL32(200, 200, 200, 255), a.sub.c_str());

    // Majukan baris, kalau tidak semua alert menumpuk di koordinat yang sama.
    y += boxH + gap;
  }
}

}  // namespace SkillAlert