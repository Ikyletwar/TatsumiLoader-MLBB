#pragma once
// TatsumiLoader - shell UI: sidebar, kartu accordion, dan widget custom.
//
// Semua widget di sini digambar manual (InvisibleButton + DrawList) supaya
// bentuknya tidakIkut style bawaan ImGui. Semuanya header-only (inline).
//
// CARA PAKAI:
//   UI::Sidebar(&tab, lebar, versi, attached);   // di dalam window utama
//   ImGui::SameLine();
//   ImGui::BeginChild("Content", ImVec2(0,0), 0);
//   UI::HeaderBar("AUTO", "subtitle");
//   if (UI::Section("RETRIBUTION", "hint", page, idx)) {
//      UI::Toggle("Auto Retribution", &autoRetribution, "sub-label");
//      UI::Slider("Retri X", &retriTouchX, 0, 3000, "px", "%.0f");
//      UI::EndSection();   // WAJIB dipanggil, biar indent-nya balance
//   }

#include <cmath>
#include <cstdio>

#include "imgui.h"
#include "Theme.h"

namespace UI {

// ------------------------------------------------------------------
// Helper kecil
// ------------------------------------------------------------------
inline ImU32 Col(const ImVec4 &v) { return ImGui::GetColorU32(v); }

// ID unik per-widget. Pakai alamat variabel supaya 2 slider dengan label
// sama tidak saling berebut ID.
inline void MakeId(char *buf, size_t n, const char *label, const void *owner) {
  snprintf(buf, n, "##%s@%p", label ? label : "x", owner);
}

// Hitung tinggi font saat ini (sudah termasuk FontGlobalScale).
inline float Fh() { return ImGui::GetFontSize() * ImGui::GetIO().FontGlobalScale; }

// ------------------------------------------------------------------
// Ikon vektor untuk sidebar (tanpa font icon -> nol dependensi)
// kind: 0=AUTO 1=ESP 2=VISUAL 3=ROOM 4=INFO 5=SETTINGS
// ------------------------------------------------------------------
inline void DrawNavIcon(int kind, ImVec2 c, float s, ImU32 col) {
  ImDrawList *dl = ImGui::GetWindowDrawList();
  const float t = 2.0f * s / 9.0f;  // ketebalan garis ikut skala

  switch (kind) {
    case 0: {  // petir (garis zig-zag, bukan fill supaya aman dari concave)
      ImVec2 p[4] = {
          ImVec2(c.x + s * 0.30f, c.y - s),
          ImVec2(c.x - s * 0.45f, c.y + s * 0.10f),
          ImVec2(c.x + s * 0.10f, c.y + s * 0.10f),
          ImVec2(c.x - s * 0.30f, c.y + s),
      };
      dl->AddPolyline(p, 4, col, 0, t * 1.6f);
      break;
    }
    case 1: {  // mata
      ImVec2 p[20];
      for (int i = 0; i < 20; i++) {
        const float a = (float)i / 20.0f * 6.2831853f;
        p[i] = ImVec2(c.x + std::cos(a) * s, c.y + std::sin(a) * s * 0.58f);
      }
      dl->AddPolyline(p, 20, col, 0, t);
      dl->AddCircleFilled(c, s * 0.30f, col);
      break;
    }
    case 2: {  // target / crosshair
      dl->AddCircle(c, s * 0.80f, col, 20, t);
      dl->AddCircleFilled(c, s * 0.18f, col);
      dl->AddLine(ImVec2(c.x - s * 1.15f, c.y), ImVec2(c.x - s * 0.95f, c.y), col, t);
      dl->AddLine(ImVec2(c.x + s * 0.95f, c.y), ImVec2(c.x + s * 1.15f, c.y), col, t);
      dl->AddLine(ImVec2(c.x, c.y - s * 1.15f), ImVec2(c.x, c.y - s * 0.95f), col, t);
      dl->AddLine(ImVec2(c.x, c.y + s * 0.95f), ImVec2(c.x, c.y + s * 1.15f), col, t);
      break;
    }
    case 3: {  // grid 2x2
      const float w = s * 0.72f, h = s * 0.72f, g = s * 0.26f;
      dl->AddRectFilled(ImVec2(c.x - w - g * 0.5f, c.y - h - g * 0.5f),
                        ImVec2(c.x - g * 0.5f, c.y - g * 0.5f), col, 2.0f);
      dl->AddRectFilled(ImVec2(c.x + g * 0.5f, c.y - h - g * 0.5f),
                        ImVec2(c.x + w + g * 0.5f, c.y - g * 0.5f), col, 2.0f);
      dl->AddRectFilled(ImVec2(c.x - w - g * 0.5f, c.y + g * 0.5f),
                        ImVec2(c.x - g * 0.5f, c.y + h + g * 0.5f), col, 2.0f);
      dl->AddRectFilled(ImVec2(c.x + g * 0.5f, c.y + g * 0.5f),
                        ImVec2(c.x + w + g * 0.5f, c.y + h + g * 0.5f), col, 2.0f);
      break;
    }
    case 4: {  // info
      dl->AddCircle(c, s, col, 24, t);
      dl->AddRectFilled(ImVec2(c.x - t * 0.5f, c.y - s * 0.42f),
                        ImVec2(c.x + t * 0.5f, c.y + s * 0.18f), col, 1.0f);
      dl->AddCircleFilled(ImVec2(c.x, c.y + s * 0.52f), t * 0.9f, col);
      break;
    }
    default: {  // roda gigi
      dl->AddCircle(c, s * 0.58f, col, 20, t);
      for (int i = 0; i < 8; i++) {
        const float a = (float)i / 8.0f * 6.2831853f;
        const float ca = std::cos(a), sa = std::sin(a);
        dl->AddLine(ImVec2(c.x + ca * s * 0.78f, c.y + sa * s * 0.78f),
                    ImVec2(c.x + ca * s * 1.18f, c.y + sa * s * 1.18f), col, t);
      }
      dl->AddCircleFilled(c, s * 0.20f, col);
      break;
    }
  }
}

// ------------------------------------------------------------------
// Sidebar: logo + 6 nav item + status pill di bawah
// ------------------------------------------------------------------
inline bool NavItem(int kind, const char *label, bool active) {
  const float w = ImGui::GetContentRegionAvail().x;
  const float h = 44.0f;
  ImVec2 p = ImGui::GetCursorScreenPos();

  char id[96];
  snprintf(id, sizeof(id), "##nav%d", kind);
  ImGui::InvisibleButton(id, ImVec2(w, h));
  const bool pressed = ImGui::IsItemClicked();
  const bool hovered = ImGui::IsItemHovered();

  ImDrawList *dl = ImGui::GetWindowDrawList();
  if (active) {
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(Shade(0.55f, 0.22f)), 9.0f);
    dl->AddRectFilled(p, ImVec2(p.x + 3.0f, p.y + h), Col(Base()), 3.0f);
  } else if (hovered) {
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(Neutral(0.20f, 0.45f)), 9.0f);
  }

  const ImU32 icol = active ? Col(Bright()) : Col(Neutral(0.55f));
  DrawNavIcon(kind, ImVec2(p.x + 22.0f, p.y + h * 0.5f), 8.5f, icol);

  const float fh = Fh();
  const ImVec2 ts = ImGui::CalcTextSize(label);
  ImGui::SetCursorScreenPos(ImVec2(p.x + 42.0f, p.y + (h - fh) * 0.5f));
  ImGui::TextColored(active ? Bright() : Neutral(0.82f), "%s", label);

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  return pressed;
}

inline void Sidebar(int *tab, float width, const char *version, bool attached) {
  static const char *kNames[6] = {"AUTO", "ESP", "VISUAL", "ROOM", "INFO", "SETTINGS"};
  static const int kIcons[6] = {0, 1, 2, 3, 4, 5};

  ImGui::BeginChild("##Sidebar", ImVec2(width, 0), ImGuiChildFlags_Border);

  // ---- logo ----
  {
    ImVec2 p = ImGui::GetCursorScreenPos();
    const float box = 30.0f;
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, ImVec2(p.x + box, p.y + box), Col(Shade(0.85f, 1.0f)), 8.0f);
    const float fh = Fh();
    ImGui::SetCursorScreenPos(ImVec2(p.x + box * 0.5f - fh * 0.30f,
                                     p.y + box * 0.5f - fh * 0.5f));
    ImGui::TextColored(ImVec4(0.06f, 0.06f, 0.07f, 1.0f), "T");
    ImGui::SetCursorScreenPos(ImVec2(p.x + box + 10.0f,
                                     p.y + box * 0.5f - fh * 0.5f));
    ImGui::TextColored(Base(), "%s", "TATSUMI");
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + box));
  }
  ImGui::Spacing();
  ImGui::Spacing();

  // ---- 6 nav item ----
  for (int i = 0; i < 6; i++) {
    if (NavItem(kIcons[i], kNames[i], *tab == i)) *tab = i;
    ImGui::Spacing();
  }

  // ---- sisipkan ruang kosong, pin status ke bawah ----
  {
    const float avail = ImGui::GetContentRegionAvail().y;
    const float need = 62.0f;
    if (avail > need) ImGui::Dummy(ImVec2(0, avail - need));
  }

  // ---- status pill ----
  {
    const float w = ImGui::GetContentRegionAvail().x;
    const float h = 28.0f;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(Neutral(0.16f, 0.70f)), 8.0f);
    const ImU32 dot = attached ? IM_COL32(80, 220, 140, 255) : IM_COL32(230, 90, 90, 255);
    dl->AddCircleFilled(ImVec2(p.x + 14.0f, p.y + h * 0.5f), 4.0f, dot);
    const float fh = Fh();
    ImGui::SetCursorScreenPos(ImVec2(p.x + 26.0f, p.y + (h - fh) * 0.5f));
    ImGui::TextColored(attached ? Neutral(0.90f) : Neutral(0.55f), "%s",
                       attached ? "ATTACHED" : "SEARCHING");
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  }
  ImGui::TextDisabled("%s", version ? version : "");

  ImGui::EndChild();
}

// ------------------------------------------------------------------
// Header konten: judul besar + subjudul
// ------------------------------------------------------------------
inline void HeaderBar(const char *title, const char *sub) {
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  ImVec2 p = ImGui::GetCursorScreenPos();

  ImDrawList *dl = ImGui::GetWindowDrawList();
  // aksen pendek di kiri judul
  dl->AddRectFilled(ImVec2(p.x, p.y + fh * 0.14f),
                    ImVec2(p.x + 3.0f, p.y + fh * 0.86f), Col(Base()), 2.0f);

  ImGui::SetCursorScreenPos(ImVec2(p.x + 12.0f, p.y));
  ImGui::TextColored(Neutral(0.97f), "%s", title);
  float y = p.y + fh + 2.0f;

  if (sub && sub[0]) {
    ImGui::SetCursorScreenPos(ImVec2(p.x + 12.0f, y));
    ImGui::TextDisabled("%s", sub);
    y += fh + 2.0f;
  }
  (void)w;
  ImGui::SetCursorScreenPos(ImVec2(p.x, y + 4.0f));
}

// ------------------------------------------------------------------
// Accordion: 1 kartu terbuka per halaman.
// page = indeks halaman (0..5), idx = urutan kartu di halaman itu.
// Return true kalau terbuka -> isi kartu. WAJIB tutup dengan EndSection().
// ------------------------------------------------------------------
// page 0..5 = halaman utama (AUTO/ESP/VISUAL/ROOM/INFO/SETTINGS).
// page 6..7 = slot ekstra untuk sub-kartu (mis. Calibration di dalam MINIMAP),
// supaya sub-kartu tidak merebut slot accordion kartu induknya.
inline int g_openCard[8] = {0, 0, 0, 0, 0, 0, 0, 0};

inline bool Section(const char *title, const char *hint, int page, int idx) {
  if (page < 0 || page > 7) page = 0;

  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  const float h = fh + 20.0f;
  ImVec2 p = ImGui::GetCursorScreenPos();

  char id[64];
  snprintf(id, sizeof(id), "##card%d_%d", page, idx);
  ImGui::InvisibleButton(id, ImVec2(w, h));
  if (ImGui::IsItemClicked()) g_openCard[page] = (g_openCard[page] == idx) ? -1 : idx;

  const bool open = g_openCard[page] == idx;
  const bool hovered = ImGui::IsItemHovered();

  ImDrawList *dl = ImGui::GetWindowDrawList();
  const ImVec2 a = p, b = ImVec2(p.x + w, p.y + h);
  if (open) {
    dl->AddRectFilled(a, b, Col(Shade(0.42f, 0.22f)), 10.0f);
    dl->AddRectFilled(a, ImVec2(a.x + 3.0f, b.y), Col(Base()), 3.0f);
  } else if (hovered) {
    dl->AddRectFilled(a, b, Col(Neutral(0.20f, 0.50f)), 10.0f);
  } else {
    dl->AddRectFilled(a, b, Col(Neutral(0.13f, 0.55f)), 10.0f);
  }

  // chevron di kanan (berputar kalau terbuka)
  {
    const float cx = b.x - 18.0f, cy = p.y + h * 0.5f, r = 5.0f;
    const ImU32 cc = open ? Col(Bright()) : Col(Neutral(0.55f));
    if (open) {
      dl->AddLine(ImVec2(cx - r, cy - r * 0.5f), ImVec2(cx, cy + r * 0.5f), cc, 2.0f);
      dl->AddLine(ImVec2(cx, cy + r * 0.5f), ImVec2(cx + r, cy - r * 0.5f), cc, 2.0f);
    } else {
      dl->AddLine(ImVec2(cx - r * 0.5f, cy - r), ImVec2(cx + r * 0.5f, cy), cc, 2.0f);
      dl->AddLine(ImVec2(cx + r * 0.5f, cy), ImVec2(cx - r * 0.5f, cy + r), cc, 2.0f);
    }
  }

  ImGui::SetCursorScreenPos(ImVec2(a.x + 14.0f, p.y + (h - fh) * 0.5f));
  ImGui::TextColored(open ? Bright() : Neutral(0.86f), "%s", title);

  // hint di kanan judul, kalau masih muat
  if (hint && hint[0]) {
    const float tw = ImGui::CalcTextSize(title).x;
    const float hx = a.x + 14.0f + tw + 12.0f;
    if (hx + 60.0f < b.x) {
      ImGui::SetCursorScreenPos(ImVec2(hx, p.y + (h - fh) * 0.5f));
      ImGui::TextDisabled("%s", hint);
    }
  }

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  // Hanya Indent: widget di dalam sini mengukur lebar sendiri lewat
  // GetContentRegionAvail, jadi PushItemWidth tidak perlu (dan kalau ditambah
  // akan memotong lebar dua kali).
  if (open) ImGui::Indent(18.0f);
  return open;
}

inline void EndSection() { ImGui::Unindent(18.0f); }

// ------------------------------------------------------------------
// Toggle: switch pill di kanan, label di kiri, sub-label opsional.
// ------------------------------------------------------------------
inline bool Toggle(const char *label, bool *v, const char *sub = nullptr) {
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  const float ph = 26.0f, pw = 48.0f;              // ukuran pill
  const float h = sub && sub[0] ? fh * 2.0f + 16.0f : ph + 10.0f;

  ImVec2 p = ImGui::GetCursorScreenPos();
  char id[96];
  MakeId(id, sizeof(id), label, (const void *)v);
  ImGui::InvisibleButton(id, ImVec2(w, h));
  bool pressed = ImGui::IsItemClicked();
  if (pressed) *v = !*v;

  ImDrawList *dl = ImGui::GetWindowDrawList();
  const bool hot = ImGui::IsItemHovered();
  if (hot && !*v) {
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(Neutral(0.20f, 0.35f)), 8.0f);
  }

  // pill
  ImVec2 pp(p.x + w - pw, p.y + (sub && sub[0] ? 0.0f : (h - ph) * 0.5f));
  const ImVec2 pb(pp.x + pw, pp.y + ph);
  dl->AddRectFilled(pp, pb, *v ? Col(Base()) : Col(Neutral(0.26f, 0.95f)), ph * 0.5f);
  const float kr = ph * 0.5f - 3.0f;
  const float kcx = *v ? pb.x - ph * 0.5f : pp.x + ph * 0.5f;
  dl->AddCircleFilled(ImVec2(kcx, pp.y + ph * 0.5f), kr,
                      *v ? Col(ImVec4(0.06f, 0.06f, 0.07f, 1.0f)) : Col(Neutral(0.70f)));

  // label + sub
  ImGui::SetCursorScreenPos(ImVec2(p.x, sub && sub[0] ? p.y : p.y + (h - fh) * 0.5f));
  ImGui::TextColored(*v ? Bright() : Neutral(0.88f), "%s", label);
  if (sub && sub[0]) {
    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + fh + 2.0f));
    ImGui::TextDisabled("%s", sub);
  }

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  return pressed;
}

// ------------------------------------------------------------------
// Slider: label kiri + chip nilai kanan + rail tipis di bawah.
// Semua tinggi area sentuh ikut widget (bukan cuma rail) -> enak di HP.
// ------------------------------------------------------------------
inline bool SliderF(const char *label, float *v, float mn, float mx,
                    const char *unit = nullptr, const char *fmt = "%.1f") {
  if (mx <= mn) return false;
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  const float chipH = fh + 8.0f;
  const float railH = 5.0f;
  const float hitH = chipH + 10.0f + 24.0f;  // label/chip + jeda + rail sentuh

  ImVec2 p = ImGui::GetCursorScreenPos();
  char id[96];
  MakeId(id, sizeof(id), label, (const void *)v);
  ImGui::InvisibleButton(id, ImVec2(w, hitH));
  const bool hot = ImGui::IsItemHovered();

  // --- nilai: chip kanan ---
  char num[48];
  snprintf(num, sizeof(num), fmt, *v);
  char buf[64];
  if (unit && unit[0]) snprintf(buf, sizeof(buf), "%s %s", num, unit);
  else snprintf(buf, sizeof(buf), "%s", num);
  const float vw = ImGui::CalcTextSize(buf).x;

  ImDrawList *dl = ImGui::GetWindowDrawList();
  // chip nilai
  const ImVec2 ca(p.x + w - vw - 16.0f, p.y), cb(p.x + w, p.y + chipH);
  dl->AddRectFilled(ca, cb, Col(Shade(0.55f, 0.22f)), 6.0f);
  ImGui::SetCursorScreenPos(ImVec2(cb.x - vw - 8.0f, p.y + 4.0f));
  ImGui::TextColored(Bright(), "%s", buf);

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + (chipH - fh) * 0.5f));
  ImGui::TextColored(Neutral(0.88f), "%s", label);

  // --- rail ---
  const float ry = p.y + chipH + 10.0f + railH * 0.5f;
  const float rx0 = p.x, rx1 = p.x + w;
  dl->AddRectFilled(ImVec2(rx0, ry - railH * 0.5f), ImVec2(rx1, ry + railH * 0.5f),
                    Col(Neutral(0.28f, 0.95f)), railH * 0.5f);
  float t = (*v - mn) / (mx - mn);
  t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
  dl->AddRectFilled(ImVec2(rx0, ry - railH * 0.5f), ImVec2(rx0 + (rx1 - rx0) * t, ry + railH * 0.5f),
                    Col(Base()), railH * 0.5f);
  const float kr = 9.0f;
  dl->AddCircleFilled(ImVec2(rx0 + (rx1 - rx0) * t, ry), kr, Col(Bright()));

  // --- interaksi ---
  bool changed = false;
  if (ImGui::IsItemActive()) {
    const float mxPos = ImGui::GetIO().MousePos.x;
    float nt = (mxPos - rx0) / (rx1 - rx0);
    nt = nt < 0.0f ? 0.0f : (nt > 1.0f ? 1.0f : nt);
    const float nv = mn + nt * (mx - mn);
    if (nv != *v) {
      *v = nv;
      changed = true;
    }
  }
  (void)hot;

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + hitH));
  return changed;
}

inline bool SliderI(const char *label, int *v, int mn, int mx, const char *unit = nullptr) {
  float fv = (float)*v;
  const bool ch = SliderF(label, &fv, (float)mn, (float)mx, unit, "%.0f");
  if (ch) *v = (int)fv;
  return ch;
}

// ------------------------------------------------------------------
// Tombol: outline (default) atau primary (aksen penuh)
// ------------------------------------------------------------------
inline bool ActionBtn(const char *label, bool primary = false) {
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  const float h = fh + 22.0f;

  ImVec2 p = ImGui::GetCursorScreenPos();
  char id[96];
  MakeId(id, sizeof(id), label, (const void *)label);
  ImGui::InvisibleButton(id, ImVec2(w, h));
  const bool pressed = ImGui::IsItemClicked();
  const bool hot = ImGui::IsItemHovered();

  ImDrawList *dl = ImGui::GetWindowDrawList();
  ImVec4 bg, fg;
  if (primary) {
    bg = hot ? Bright() : Base();
    fg = ImVec4(0.06f, 0.06f, 0.07f, 1.0f);
  } else {
    bg = Neutral(hot ? 0.24f : 0.15f, 0.90f);
    fg = Neutral(0.92f);
  }
  dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), Col(bg), 9.0f);
  if (!primary) {
    dl->AddRect(p, ImVec2(p.x + w, p.y + h), Col(Shade(0.55f, 0.60f)), 9.0f, 0, 1.5f);
  }

  const float tw = ImGui::CalcTextSize(label).x;
  ImGui::SetCursorScreenPos(ImVec2(p.x + (w - tw) * 0.5f, p.y + (h - fh) * 0.5f));
  ImGui::TextColored(fg, "%s", label);

  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
  return pressed;
}

// ------------------------------------------------------------------
// Catatan kecil di dalam kartu
// ------------------------------------------------------------------
inline void Note(const char *txt) {
  ImVec2 p = ImGui::GetCursorScreenPos();
  const float w = ImGui::GetContentRegionAvail().x;
  const float fh = Fh();
  const float h = fh + 12.0f;
  ImDrawList *dl = ImGui::GetWindowDrawList();
  dl->AddRectFilled(p, ImVec2(p.x + 3.0f, p.y + h), Col(Faint()), 2.0f);
  ImGui::SetCursorScreenPos(ImVec2(p.x + 12.0f, p.y + 6.0f));
  ImGui::TextDisabled("%s", txt);
  ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h));
}

// ------------------------------------------------------------------
// Baris key-value untuk tab INFO
// ------------------------------------------------------------------
inline void Kv(const char *k, const char *v, const ImVec4 &col) {
  const float w = ImGui::GetContentRegionAvail().x;
  ImGui::TextDisabled("%s", k);
  ImGui::SameLine(w * 0.42f);
  ImGui::TextColored(col, "%s", v ? v : "-");
}

}  // namespace UI
