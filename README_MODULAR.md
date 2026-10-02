# TatsumiLoader — Modular Map

`main.cpp` tetap entry point (3712 baris), tapi alur boot, config, dan
offset sekarang modular. Mau kembangin next time cukup sentuh modul kecil.

## Peta modul

| Path | Isi | Kapan disentuh |
|---|---|---|
| `src/app/main.cpp` | UI ImGui + loop + fitur (ESP/retri/skill) | tambah fitur/tab baru |
| `src/core/BootFlow.{h,cpp}` | launch MLBB + polling PID + attach | ubah cara launch/wait |
| `src/config/GameConfig.h` | daftar paket MLBB, path cfg/offset, URL sosmed, timeout | tambah varian MLBB |
| `src/config/Offsets.h` | semua offset (`inline size_t`, bukan const) + `kOffsetBundleVersion` | update offset di source |
| `src/config/OffsetOverrides.h` | loader `/data/local/tmp/TatsumiLoader.offsets` | (tidak perlu diubah) |
| `src/features/RoomInfo.*` | tabel lobby/dodge | fitur room |
| `src/features/SkillAlert.*` | notifier ulti/retri musuh, 4 event | tambah event notif |
| `src/utils/`, `src/Engine/` | touch, draw, memory | platform |

## Alur boot (paham dalam 10 detik)

```
pilih [1] ROOT / [2] KERNEL
  -> LoadOffsetOverrides()        // file menang atas bawaan
  -> BootEnsureGame()             // core/BootFlow.cpp
       -> pollOnce() ketemu? langsung attach
       -> kalau tidak: LaunchMLBB() via monkey, lalu WaitForMLPID(120s)
       -> ketemu: pid + AttachToGame() (libbase/is_attached set)
  -> GUI ImGui + menu jalan
  -> pid_monitor tetap standby kalau game restart
```

## Dapat offset baru? 2 cara (pilih yang cepat)

**A. Tanpa recompile (disarankan):** tulis file di HP:
```
# /data/local/tmp/TatsumiLoader.offsets
BattleManager.BasePtr=0x635b290
ShowEntity.Hp=0x1a4
Camera.v2=0xc8
```
Restart loader. Log `adb logcat` tampil `[offsets] override ...: N applied`.
Key valid = tabel di `OffsetOverrides.h`. Key salah tampil `unknown key`.

**B. Permanen di source:** ubah angka di `Offsets.h`, naikkan
`kOffsetBundleVersion`, rebuild `ndk-build`, backup.

## Tambah fitur baru (contoh)

1. State global taruh dekat fitur sejenis di `main.cpp`
   (atau bikin `src/features/FiturBaru.{h,cpp}`).
2. Logic baca/tulis memory pakai `OFF_xx()` macro, jangan hardcode angka.
3. Render loop di `DrawMonster`/`Layout_tick_UI`, setting di tab terkait.
4. `Save/LoadTatsumiSettings` + README ini kalau perlu persist.
5. `ndk-build -j4`, push `libs/arm64-v8a/TatsumiLoader`, tes.

## Build offline

```sh
export ANDROID_NDK_HOME=$HOME/Android/Sdk/ndk/27.0.12077973
cd app/src/main          # WAJIB: NDK cari jni/Android.mk relatif ke sini
$ANDROID_NDK_HOME/ndk-build -j4 \
  NDK_APPLICATION_MK=jni/Application.mk \
  NDK_APP_OUT=obj NDK_LIBS_OUT=libs
#Output: libs/arm64-v8a/TatsumiLoader
```

## Update 2026-09-30 (rapi + advance)

- `src/core/Settings.h` — tabel settings terpusat (X-macro BOOL/FLOAT/INT).
  Tambah setting = 1 baris, save/load otomatis ikut. Key cfg tidak berubah
  (kompatibel file lama) + `cfg_version=2` untuk migrasi ke depan.
- `src/features/DeviceReport.{h,cpp}` — format + antre laporan perangkat.
  `main.cpp` tinggal isi struct polos (tanpa urus curl/Telegram).
- `src/features/TelegramReport.{h,cpp}` — transport Bot API (tidak berubah).
- `main.cpp` 3769 -> ~3600 baris (save/load 200+ baris jadi ~30 baris).

## Update 2026-10-02 (notifier ulti/retri modular + 2 bug fix)

- `src/features/SkillAlert.{h,cpp}` — notifier ulti/retri pindah dari `main.cpp`
  ke modul sendiri. `main.cpp` cuma 4 titik kontak: `Tick()` (dari `DrawMonster`),
  `RenderSettings()` (tab ESP), `Draw()`, `Reset()` (dari `AttachToGame`).
- **Event baru: CAST.** Sebelumnya cuma READY (CD habis -> siap). Sekarang ada
  4 event: `ULTI READY` / `ULTI DITERIAK` / `RETRI READY` / `RETRI DITERIAK`.
  Masing-masing punya toggle sendiri di tab ESP > `ULTI & RETRI ALERT`.
- **Bug 1 fix — alert menumpuk.** `DrawSkillAlerts` hitung `y` sekali di luar
  loop lalu tidak pernah di-increment, jadi semua alert gambar di koordinat
  sama dan hanya yang terakhir kelihatan. Sekarang `y += boxH + gap`.
- **Bug 2 fix — state bocor.** `g_ultSeenCd`/`g_retriSeenCd` tidak pernah
  di-reset. Hero mati (di-skip loop) lalu respawn dengan CD=0 memicu alert
  "READY" palsu. Sekarang ada `lastSeenMs` per hero: hilang >3 detik = state
  dibuang tanpa menembak, >60 detik = di-prune.
- **Tidak ada slider size baru.** `Alert_Scale` (tab VISUAL) sudah dipakai
  bareng oleh `DrawGlobalWarning` (Lord/Turtle) dan `SkillAlert::Draw`, jadi
  satu slider itu sudah mengatur keduanya. Slider `Duration (ms)` baru ada
  di tab ESP karena skalanya beda (text vs kotak Lord/Turtle).
- Setting baru: `ultCastAlert`, `retriCastAlert` (bool, default off),
  `skillAlertMs` (int, default 4000 — tadinya hardcode).

### Tambah event notif baru (3 langkah)

1. Tambah field `uint8_t` di `HeroWatch` (batas 2 bit = 4 tahap: unknown /
   siap / cooldown).
2. Panggil `Step(state, now, notifyReady, notifyCast, label, heroId, ...)`.
   `unknown -> *` tidak pernah menembak, jadi anti false-positive gratis.
3. Tambah 1 baris `X("key", var)` di `core/Settings.h`. Warna, gambar, dan
   persist ikut otomatis.
