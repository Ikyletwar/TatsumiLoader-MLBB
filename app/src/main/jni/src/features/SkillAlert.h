#pragma once
// TatsumiLoader - notifier ulti & retri musuh (modul mandiri).
//
// Modul ini SENGAJA tidak tahu apa itu CoolDownData / getCoolDown / DrawMonster.
// Caller cuma mengirim 3 detik-detik cooldown per hero lewat SkillCdView.
//
// Siklus hidup per frame (semua dipanggil dari thread render yang sama):
//   DrawMonster()  -> SkillAlert::Tick(heroId, view)   // deteksi transisi
//   Layout_tick_UI -> SkillAlert::RenderSettings()    // panel ESP
//   Layout_tick_UI -> SkillAlert::Draw(drawList)      // gambar kotak alert
//   AttachToGame() -> SkillAlert::Reset()             //	match baru, jangan lupa
//
// CARA TAMBAH EVENT BARU (3 langkah, tanpa sentuh main.cpp):
//   1. Tambah field uint8_t di HeroWatch (batas: 2 bit = 4 tahap).
//   2. Tambah 1 blok transisi di Tick().
//   3. Tambah 1 baris X("key", var) di core/Settings.h.
//   Sisanya (warna, gambar, persist) otomatis.

struct ImDrawList;  // forward declare di scope global (bukan di namespace)

// Input minimal: nilai dalam DETIK, 0 = siap dipakai.
struct SkillCdView {
  int ultiSec;
  int spellSec;
  int spellId;
};

namespace SkillAlert {

// Hapus semua state + alert tampil. WAJIB dipanggil saat attach ulang game.
void Reset();

// Panggil 1x per hero musuh per frame.
void Tick(int heroId, const SkillCdView &cd);

// Setting di tab ESP > "ULTI & RETRI ALERT".
void RenderSettings();

// Gambar kotak alert. Pakai Alert_PosX / Alert_PosY / Alert_Scale yang sama
// dengan alert Lord/Turtle, jadi satu slider "Alert Scale" mengatur keduanya.
void Draw(ImDrawList *draw);

}  // namespace SkillAlert