# TatsumiLoader — MLBB External Loader

External overlay untuk Mobile Legends: Bang Bang (root / kernel).
ESP, auto retri instan, auto skill (Ling/Gusion/Kimmy), drone, room intel,
Telegram device report — dengan codebase NDK modular yang gampang dikembangkan.

> **Nama repo:** `TatsumiLoader-MLBB`
> **Deskripsi:** External overlay loader MLBB (root/kernel) — ESP, auto-retri,
> skill automation & room intel di atas codebase NDK modular.

---

## Daftar Isi

- [Fitur](#fitur)
- [Cara Pakai (HP root)](#cara-pakai-hp-root)
- [Build dari Source (offline)](#build-dari-source-offline)
- [Struktur Project](#struktur-project)
- [Update Offset Baru](#update-offset-baru)
- [Boot Flow](#boot-flow)
- [Telegram Report](#telegram-report)
- [Dump libcsharp.so dari HP](#dump-libcsharpso-dari-hp)
- [Backup & Versioning](#backup--versioning)
- [FAQ / Troubleshooting](#faq--troubleshooting)
- [Kontak](#kontak)

---

## Fitur

| Tab | Isi |
|---|---|
| INFO | Status injeksi, info device, tombol Instagram + TikTok `@ikyletwar` |
| ROOM | Tabel Blue/Red Team: nickname, hero, ID, rank, spell, role, country, winrate, hero favorit + Auto DOD |
| ESP | ESP Line, Hero ESP (height/size), Prediction ESP, Enemy Cooldown (+CD size), Drone (FOV) |
| VISUAL | Monster ESP, Minimap ESP (hero/monster/minion + kalibrasi), Alert warning + HP |
| RETRI | Auto Retri instan (Red/Blue/Lord/Turtle/Crab/Lito) + tuning Early Margin, Max Range, Spam Ms, Double Tap |
| SKILL | Ling Auto Sword, Gusion Auto Combo (1-2-1-2-3), Kimmy Auto Aim (FOV/smooth/priority + joystick UI) |
| SETTINGS | Re-scan game, save/load settings, UI opacity, send report, exit cheat |

**Auto Retri instan:** baca HP tipe benar, tap 180 HP lebih awal (kompensasi
delay), double-tap + retry 80 ms, dan jalur cepat `FastAutoRetri()` tiap ~1 ms
di worker thread (tidak nunggu FPS overlay).

**Anti-ban berlapis (ringan):** external (tanpa inject, baca via
`process_vm_readv`), string obfuscation (`xorstr_/oxorany`), binary stripped
(`strip-all`, `exclude-libs`, hidden visibility), dual driver ROOT/KERNEL,
DNS AdGuard saat jalan. Tetap ada risiko — DOD/write memory terdeteksi server.

---

## Cara Pakai (HP root)

```sh
# 1. Push binary
adb push libs/arm64-v8a/TatsumiLoader /data/local/tmp/
adb shell "chmod +x /data/local/tmp/TatsumiLoader"

# 2. Jalankan sebagai root
adb shell
su
/data/local/tmp/TatsumiLoader
```

1. Pilih `[1] NORMAL (ROOT)` atau `[2] KERNEL (PRO)`.
2. Loader **otomatis menjalankan MLBB** (`monkey`) lalu **polling PID**
   sampai game ketemu (timeout 120 dtk, log `[boot] ...`).
3. Menu overlay muncul. Tab SETTINGS → `LOAD SETTINGS` untuk profil lama.
4. Keluar pakai tombol `EXIT CHEAT` (keluar bersih, tidak balik ke menu).

> Config tersimpan di `/data/local/tmp/TatsumiLoader.cfg`.
> Override offset di `/data/local/tmp/TatsumiLoader.offsets` (lihat bawah).

---

## Build dari Source (offline)

Butuh: Android NDK r27, `ndk-build` di PATH. Tanpa internet.

```sh
export ANDROID_NDK_HOME=$HOME/Android/Sdk/ndk/27.0.12077973
ndk-build -C app/src/main -j4 \
  NDK_PROJECT_PATH=app/src/main \
  NDK_APPLICATION_MK=app/src/main/jni/Application.mk \
  NDK_APP_OUT=app/src/main/obj \
  NDK_LIBS_OUT=app/src/main/libs
# hasil: app/src/main/libs/arm64-v8a/TatsumiLoader
```

Tool extract bila dapat `.rar` update: `~/.local/bin/unrar`
(`unrar x -o+ arsip.rar tujuan/`).

---

## Struktur Project

```
EXTERNAL-MLBB/
├── README.md                  # file ini
├── README_MODULAR.md          # peta modul + cara nambah fitur (detail)
├── app/src/main/jni/
│   ├── Android.mk             # modul TatsumiLoader + daftar source
│   ├── Application.mk         # arm64-v8a, c++_static, android-24
│   ├── src/
│   │   ├── app/main.cpp       # UI ImGui + loop + fitur (~3600 baris)
│   │   ├── core/              # BootFlow, Settings (tabel X-macro)
│   │   ├── config/            # GameConfig, Offsets, OffsetOverrides, Telegram
│   │   ├── features/          # RoomInfo, TelegramReport, DeviceReport
│   │   ├── utils/             # touch (uinput), read/pidof, xorstr
│   │   ├── Engine/            # CanvasView
│   │   ├── ImGui/             # overlay
│   │   └── curl/              # libcurl+openssl prebuilt per-ABI
```

Detail modul + konvensi ada di [README_MODULAR.md](README_MODULAR.md).

**Tambah setting baru:** 1 baris di `src/core/Settings.h`, otomatis ke-save/load.

---

## Update Offset Baru

Lihat versi bundle aktif di `src/config/Offsets.h` (`kOffsetBundleVersion`).

**Cara A — tanpa recompile (cepat):** tulis di HP:

```
# /data/local/tmp/TatsumiLoader.offsets
BattleManager.BasePtr=0x635b290
ShowEntity.Hp=0x1a4
Camera.v2=0xc8
```

Restart loader, cek log `[offsets] override ...: N applied`.
Daftar key valid ada di `src/config/OffsetOverrides.h`.

**Cara B — permanen:** ubah angka di `Offsets.h`, naikkan versi bundle,
rebuild + backup.

**Dapat offset dari mana:** `libcsharp.so` (99 MB, di
`/data/data/com.mobile.legends/app_libs/`, BUKAN stub 376K di `/data/app`)
+ `global-metadata.dat` → Il2CppDumper → `dump.cs` → class
`BattleManager/ShowEntity/ShowPlayer/Camera`. Kalau metadata tidak ada di
disk (terproteksi), dump dari RAM saat game jalan (Zygisk Il2CppDumper /
M5Dumper.lua, lihat [Dump](#dump-libcsharpso-dari-hp)).

---

## Boot Flow

```
[1] ROOT / [2] KERNEL
 → LoadOffsetOverrides()
 → BootEnsureGame()            # src/core/BootFlow.cpp
    → game sudah jalan? langsung attach
    → belum? monkey launch → WaitForMLPID (polling pidof tiap 1 dtk)
 → AttachToGame()              # libbase libcsharp.so → is_attached
 → GUI ImGui + menu jalan
```

`pid_monitor` tetap standby kalau game restart.

---

## Telegram Report

Setiap start otomatis kirim 1x ke bot (delay 8 dtk, thread terpisah):
model/brand, versi Android+SDK, **android_id**, package, pid, status attach,
libbase, versi offset + akun (id, nick, region, country, versi client,
IP:port server) kalau sudah kebaca. Tombol `SEND REPORT` di SETTINGS untuk
kirim manual.

Konfigurasi: `src/config/TelegramConfig.h` (token + chat id).
Isi token bot + chat id di `src/config/TelegramConfig.h` (lihat BotFather).
Jangan commit token asli ke publik; kalau bocor, revoke via BotFather.

---

## Dump libcsharp.so dari HP

Script GameGuardian: [`../TatsumiDump_il2cpp.lua`](../TatsumiDump_il2cpp.lua)
(dump `libil2cpp.so`/`libcsharp.so` dari RAM + manifest `ranges_*.txt`).
Hasil mentah → rebuild pakai SoFixer sebelum Il2CppDumper.

---

## Backup & Versioning

Backup timestamp ada di folder `WORKSPACE/ML/`:
`EXTERNAL-MLBB-TatsumiLoader-YYYYMMDD-HHMM.7z` (exclude `obj/`).

---

## FAQ / Troubleshooting

**`Pointer tag ... was truncated`?**
Warning benign Android 11+ (tagged pointer `xorstr_`/`open`/`process_vm_readv`
di-strip kernel). Bukan crash — abaikan.

**Buka tab Settings langsung keluar?**
Bug lama (`main_thread_flag` nyangkut di header SYSTEM), sudah fix —
sekarang keluar hanya via tombol `EXIT CHEAT`.

**Auto retri telat?**
Naikkan `Early Margin` (kompensasi delay), cek `Max Range`, pastikan posisi
tombol retri (`Retri X/Y`, titik putih `Show Circle`) pas dengan layar HP.

**Menu `SEARCHING GAME...` terus?**
Pastikan pilih proses yang benar, atau SETTINGS → `RE-SCAN GAME`.
Boot otomatis menunggu PID sampai 120 dtk.

---

## Kontak

- Instagram: https://www.instagram.com/ikyletwar/
- TikTok: https://www.tiktok.com/@ikyletwar

Personal project — TatsumiLoader © 2026.
