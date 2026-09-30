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
ndk-build -C app/src/main -j4 \
  NDK_PROJECT_PATH=app/src/main \
  NDK_APPLICATION_MK=app/src/main/jni/Application.mk \
  NDK_APP_OUT=app/src/main/obj NDK_LIBS_OUT=app/src/main/libs
```

## Update 2026-09-30 (rapi + advance)

- `src/core/Settings.h` — tabel settings terpusat (X-macro BOOL/FLOAT/INT).
  Tambah setting = 1 baris, save/load otomatis ikut. Key cfg tidak berubah
  (kompatibel file lama) + `cfg_version=2` untuk migrasi ke depan.
- `src/features/DeviceReport.{h,cpp}` — format + antre laporan perangkat.
  `main.cpp` tinggal isi struct polos (tanpa urus curl/Telegram).
- `src/features/TelegramReport.{h,cpp}` — transport Bot API (tidak berubah).
- `main.cpp` 3769 -> ~3600 baris (save/load 200+ baris jadi ~30 baris).
