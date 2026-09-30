# TatsumiLoader — MLBB External Loader

![platform](https://img.shields.io/badge/platform-Android%20arm64--v8a-green)
![mode](https://img.shields.io/badge/mode-ROOT%20%7C%20KERNEL-blue)
![ndk](https://img.shields.io/badge/NDK-r27-orange)
![lang](https://img.shields.io/badge/lang-C%2B%2B20-red)

External overlay untuk **Mobile Legends: Bang Bang** (root / kernel).
ESP, auto retri instan, auto skill (Ling / Gusion / Kimmy), drone view,
room intel + dodge, minimap radar, Telegram device report —
di atas codebase NDK **modular** yang gampang dikembangkan.

> **Repo:** `TatsumiLoader-MLBB`
> **Deskripsi:** External overlay loader MLBB (root/kernel) — ESP, auto-retri,
> skill automation & room intel di atas codebase NDK modular.

---

## Daftar Isi

- [1. Gambaran Umum & Arsitektur](#1-gambaran-umum--arsitektur)
- [2. Fitur Lengkap per Tab](#2-fitur-lengkap-per-tab)
  - [INFO](#info)
  - [ROOM](#room)
  - [ESP](#esp)
  - [VISUAL](#visual)
  - [RETRI](#retri)
  - [SKILL](#skill)
  - [SETTINGS](#settings)
- [3. Cara Pakai (HP Root)](#3-cara-pakai-hp-root)
- [4. Build dari Source (Offline)](#4-build-dari-source-offline)
- [5. Struktur Project & Peta Modul](#5-struktur-project--peta-modul)
- [6. Referensi Settings (`TatsumiLoader.cfg`)](#6-referensi-settings-tatsumiloadercfg)
- [7. Referensi Offset](#7-referensi-offset)
- [8. Update Offset Baru](#8-update-offset-baru)
- [9. Boot Flow Detail](#9-boot-flow-detail)
- [10. Telegram Report](#10-telegram-report)
- [11. Dump libcsharp.so dari HP](#11-dump-libcsharpso-dari-hp)
- [12. Panduan Development](#12-panduan-development)
- [13. Backup & Versioning](#13-backup--versioning)
- [14. FAQ / Troubleshooting](#14-faq--troubleshooting)
- [15. Changelog](#15-changelog)
- [16. Kontak](#16-kontak)

---

## 1. Gambaran Umum & Arsitektur

TatsumiLoader berjalan sebagai **proses native terpisah** (ELF `TatsumiLoader`,
bukan library injeksi). Ia membaca memori game lewat `process_vm_readv`
(mode ROOT) atau driver kernel (mode KERNEL/PRO), lalu menggambar overlay
ImGui (OpenGL) di atas layar + mengirim input sentuh virtual (uinput).

```
┌──────────────┐   read (pvm / ioctl)   ┌──────────────┐
│  MLBB proses │ ◄────────────────────── │ TatsumiLoader│
│ libcsharp.so │ ──► HP/posisi/CD/room ─► │  ESP/retri/  │
└──────────────┘   write (drone/DOD)    │  aim/room    │
                                        └──────┬───────┘
                                               │ overlay ImGui + uinput tap
                                               ▼
                                         layar HP + bot Telegram
```

Prinsip anti-ban berlapis (ringan, **bukan kebal ban**):

1. **External** — tidak inject/hook ke proses game, minim jejak userspace.
2. **Obfuscation** — semua string `xorstr_/oxorany`, simbol hidden.
3. **Stripped** — `strip-all`, `exclude-libs,ALL`, `gc-sections` (IDA kosong).
4. **Dual driver** — ROOT (userspace) atau KERNEL PRO (ioctl `0x800–0x803`,
   node `/dev/[a-z]{6}` acak + trik restore `(deleted)`).
5. **DNS AdGuard** saat jalan (hambat telemetri), dikembalikan `off` saat exit.

---

## 2. Fitur Lengkap per Tab

### INFO

- Status injeksi (`Active`/`Pending`), nama package, model HP.
- Section **FOLLOW US**: tombol **Instagram** (magenta + icon kamera vektor)
  → `https://www.instagram.com/ikyletwar/` dan **TikTok** (hitam + icon not
  balok) → `https://www.tiktok.com/@ikyletwar`, dibuka via
  `am start -a android.intent.action.VIEW`.
- Copyright `@TatsumiLoader`.

### ROOM

Intel lobby: tabel **Blue Team / Red Team** (auto sortir rank tertinggi):
Nickname, icon hero, ID, Rank (Mythic/Honor/Glory/Immortal + bintang),
Spell, Role, Country, Winrate (W/L), 3 hero favorit. Tombol
`Refresh Data`. Checkbox **Auto DOD (Risk)** — tulis
`SystemData.m_bOfflineManagerRun` untuk dodge draft jelek.

### ESP

- **PLAYER ESP:** `ESP Line` (garis ke musuh), `Hero ESP` (icon + HP/level/
  respawn timer), `ESP Height` (0–500), `Icon Size` (10–60),
  `Prediction ESP` (prediksi arah gerak dari `moveSpeed 0x190`).
- **COOLDOWN & DRONE:** `Enemy Cooldown` (lingkaran S1–S3 + spell/regen dari
  `_dicCD`, `CD Size` 5–40), `Enable Drone` + slider `FOV -9..-1`
  (tulis `Camera.v2`).

### VISUAL

- **MONSTERS:** `Monster ESP` + `Monster Icon Size`.
- **MINIMAP:** `Enable Minimap`, `Hide Minimap Line`, `Show All Monsters`,
  `Show Minion Dot`, `Minimap Size/X/Y`, ukuran icon hero/monster/minion,
  sub-menu **Calibration** (`Scale`, `Offset X/Y`).
- **ALERT WARNING:** `Show Alert`, `Show Alert HP`, posisi X/Y (%), `Scale`.

### RETRI

Auto retri **instan**: damage dihitung dari level
(`Lv<4: 520+80·Lv`, selebihnya `×1.5`), baca HP tipe benar, tap **180 HP
lebih awal** (kompensasi delay), **double-tap** + retry tiap **80 ms**,
dan jalur cepat `FastAutoRetri()` tiap ~1 ms di worker thread.

- `Enable Auto Retri`, `Show Circle` (titik putih posisi tombol),
  `Retri X/Y` (kalibrasi posisi tombol retri di layar HP-mu),
  `Early Margin` (0–500), `Max Range` (5–12), `Spam Ms` (30–300),
  `Double Tap`.
- **TARGETS:** `Buff Red`, `Buff Blue`, `Lord`, `Turtle`, `Crab`, `Lito`
  (ID: Lord 2002, Turtle 2003/2110, Blue 2005/2221, Red 2004/2220,
  Crab 2222/2223, Lito 2056).

### SKILL

- **LING AUTO SWORD:** `Enable Auto Sword`, `Dash Range`, `Dash Delay`,
  `Skill 2 X/Y` + `Reset Skill 2 Pos`.
- **GUSION AUTO COMBO:** `Enable Auto Combo` (1-2-1-2-3),
  `Combo Speed (ms)`, `Skill 2/3 X/Y` + `Reset Gusion Buttons`.
- **DD KIMMY AUTO AIM:** `Enable Kimmy Aim`, `FOV Radius`, `Smooth Factor`,
  `Target Priority` (Closest/Lowest HP/Player), `Show Visuals`,
  `Show Joystick UI` + `Joy Radius/X/Y` + `Reset to Right`.

### SETTINGS

- `RE-SCAN GAME` (attach ulang) + status `READY/SEARCHING`.
- `SAVE/LOAD SETTINGS` (`/data/local/tmp/TatsumiLoader.cfg`).
- `SEND REPORT` (kirim laporan Telegram manual).
- `UI Opacity`, `EXIT CHEAT` (keluar bersih).

---

## 3. Cara Pakai (HP Root)

**Syarat:** HP root, MLBB terinstal (varian `com.mobile.legends`,
`com.mobile.legends.usa`, atau `com.vng.mlbb`).

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
2. Loader **otomatis menjalankan MLBB** lalu **polling PID** sampai ketemu
   (log `[boot] ...`, timeout 120 dtk) — menu baru jalan setelah itu.
3. Di tab SETTINGS tekan `LOAD SETTINGS` untuk profil lama.
4. Keluar via `EXIT CHEAT`.

---

## 4. Build dari Source (Offline)

Butuh: Android NDK **r27**, `ndk-build`. Tanpa internet.

```sh
export ANDROID_NDK_HOME=$HOME/Android/Sdk/ndk/27.0.12077973
ndk-build -C app/src/main -j4 \
  NDK_PROJECT_PATH=app/src/main \
  NDK_APPLICATION_MK=app/src/main/jni/Application.mk \
  NDK_APP_OUT=app/src/main/obj \
  NDK_LIBS_OUT=app/src/main/libs
# hasil: app/src/main/libs/arm64-v8a/TatsumiLoader (ELF aarch64, ~35M)
```

Opsi build (`Application.mk` / `Android.mk`):

| Opsi | Lokasi | Arti |
|---|---|---|
| `APP_ABI := arm64-v8a` | Application.mk | target 64-bit saja |
| `APP_PLATFORM := android-24` | Application.mk | min API 24 |
| `APP_STL := c++_static` | Application.mk | static STL |
| `OPENGL_DRAW = 1` | Android.mk | 1 = OpenGL, 0 = Vulkan |

Tool extract bila dapat update `.rar`: `unrar x -o+ arsip.rar tujuan/`
(7-Zip open-source gagal untuk metode kompresi RAR5 baru).

---

## 5. Struktur Project & Peta Modul

```
EXTERNAL-MLBB/
├── README.md / README_MODULAR.md
├── app/src/main/jni/
│   ├── Android.mk / Application.mk
│   ├── src/
│   │   ├── app/main.cpp        # UI ImGui + loop + fitur (~3600 baris)
│   │   ├── core/
│   │   │   ├── BootFlow.*      # launch MLBB + polling PID + attach
│   │   │   └── Settings.h      # TABEL settings terpusat (X-macro)
│   │   ├── config/
│   │   │   ├── GameConfig.h    # varian paket, path, URL, timeout
│   │   │   ├── Offsets.h       # semua offset + versi bundle
│   │   │   ├── OffsetOverrides.h # loader file .offsets
│   │   │   └── TelegramConfig.h  # token + chat id bot
│   │   ├── features/
│   │   │   ├── RoomInfo.*      # tabel lobby + DOD
│   │   │   ├── TelegramReport.*# transport Bot API (libcurl)
│   │   │   └── DeviceReport.*  # format + antre laporan
│   │   ├── utils/              # touch uinput, pidof, xorstr, read
│   │   ├── Engine/             # CanvasView
│   │   ├── ImGui/              # overlay
│   │   └── curl/               # libcurl+openssl prebuilt per-ABI
```

Detail + konvensi: [README_MODULAR.md](README_MODULAR.md).
**Tambah modul baru = 1 file + 1 baris di `Android.mk`.**

---

## 6. Referensi Settings (`TatsumiLoader.cfg`)

Format `key=value` (bool `1/0`). Ditambah via 1 baris di `core/Settings.h`.
Default = nilai awal di source.

**Bool (default):**

| Key | Default | Key | Default |
|---|---|---|---|
| drawMHealth | true | showRetriCircle | true |
| drawMLine | true | AutoRetributionRed/Blue/Lord/Turtle/Crab/Lito | false |
| drawMAvatar | true | AA_enabled / AA_showVisuals / AA_joyVisible | false |
| drawMonsterHealth | true | MinimapIcon | true |
| drawMonsterID | false | MinimapIconBuff / MinimapIconLordTurtle | false |
| drawMonsterName | true | HideLine | false |
| iconhero | true | drawMinionMinimap | true |
| drawAlertUnderAttack | true | ESP_Player_Cooldown | true |
| EnableDrone | false | Alert_ShowHPText | true |
| drawMPrediction | false | autoSwordLing | false |
| predictionColorBySpeed / ShowOnlyMoving / ShowArrowHead | true | autoRetribution | false |
| predictionShowSpeedText | false | retriDoubleTap | true |

**Float (default):**

| Key | Default | Key | Default |
|---|---|---|---|
| FieldView | -1.0 | g_MinimapScale | 74.11 |
| predictionCircleRadius | 5.0 | g_HeroIconSize | 28.0 |
| retriTouchX / retriTouchY | 1575 / 661 | Cooldown_OffsetY / Cooldown_Size | 150.0 / 15.0 |
| retriEarlyMargin | 180.0 | Alert_PosX / PosY / Scale | 0.5 / 0.15 / 1.0 |
| retriMaxRange | 8.5 | FloatingIcon_PosX / PosY | 0.12 / 0.5 |
| AA_fovRadius / smoothFactor | 150.0 / 0.15 | Avatar_OffsetY | 80.0 |
| AA_joyRadius / DeadZone | 180.0 / 15.0 | lingSwordDashRange / DashDelay | 12.0 / 0.35 |
| AA_joyCenterX / CenterY | 0.0 (auto 85%/75% layar) | lingSkill2X / Skill2Y | 1850.0 / 850.0 |
| minionMinimapSize | 4.0 | g_Res0_MultX / MultY | 1.0 |
| g_Res1_OffsetX / OffsetY | 0.0 | | |

**Int (default):** retriSpamMs 80, AA_priorityMode 0, MinimapSize 342,
MinimapPos/MinimapPosY 76, g_MinimapHeroSize 38, g_MinimapMonsterSize 30,
lingSwordID 8452, gusionComboDelay 60.
**Khusus:** `predictionLineColor`, `predictionCircleColor` (`r,g,b,a`),
`cfg_version=2`.

---

## 7. Referensi Offset

Bundle aktif: `kOffsetBundleVersion` di `src/config/Offsets.h`.
Semua `BasePtr` adalah RVA relatif ke base `libcsharp.so`
(dibaca duluan, fallback `libil2cpp.so`).

| Namespace | Field = offset |
|---|---|
| BattleManager | BasePtr 0x635b290, LocalPlayerShow 0x48, ShowPlayers 0x70, ShowMonsters 0x78, m_RunBullets 0x38, PtrChain1 0xa8, List 0x10/0x18/0x20 |
| ShowEntity | iType 0x78, Hp 0x1a4, HpMax 0x1a8, bDeath 0xc5, bSameCampType 0x2a9, vCachePosition 0x28c, HeroID 0x18c, m_uGuid 0x188, Level 0x190, SummonSkillId 0x984, m_ShowCoolDownComp 0xf8 |
| ShowPlayer | m_EntityCampType 0xd0 |
| Camera | MainCameraPtr 0x635b620, SmoothFollow 0x10, ViewMatrix 0x5C, v2 0xc8 |
| SystemData | BasePtr 0x635cb88, m_RoomPlayerInfo 0x348, PtrChain1 0xa8, m_uiID 0x358, m_bOfflineManagerRun 0x36e |
| RoomData | lUid 0x20, iCamp 0x30, _sName 0x40, heroid 0x4c, country 0x5c, uiRankLevel 0x120, iRoad 0x138, bRoomLeader 0x32c |
| LogicBattleManager | BasePtr 0x635b5d8, PtrChain1 0xa8, m_uiFrameTime 0x13c |
| ShowCoolDownComp / CoolDownData | _dicCD 0x18 / uiCoolTime 0x14, uiStartTime 0x1C |
| Bullet | m_Id 0x3c, _bulletPos 0xfc |

---

## 8. Update Offset Baru

**Cara A — tanpa recompile (cepat):** tulis di HP
`/data/local/tmp/TatsumiLoader.offsets`:

```
# KEY=0xHEX (daftar key = tabel di OffsetOverrides.h)
BattleManager.BasePtr=0x635b290
ShowEntity.Hp=0x1a4
Camera.v2=0xc8
```

Restart loader, cek log `[offsets] override ...: N applied`
(key salah → `unknown key`).

**Cara B — permanen:** ubah angka di `Offsets.h`, naikkan
`kOffsetBundleVersion`, rebuild + backup.

---

## 9. Boot Flow Detail

```
pilih [1] ROOT / [2] KERNEL
 → LoadOffsetOverrides()          # file menang atas bawaan
 → BootEnsureGame()               # src/core/BootFlow.cpp
    → pollOnce(): UnityKillsMe → base pkg, prioritas usa→legends→vng
    → belum jalan? monkey -p <pkg> LAUNCHER → WaitForMLPID(120 dtk)
    → ketemu? pid + AttachToGame() → g_pid/libbase/is_attached
 → DNS AdGuard → GUI ImGui → menu jalan
```

Timeout = menu tetap jalan (`SEARCHING`), `pid_monitor` attach belakangan.
Watchdog `fork`: parent `_exit` setelah child selesai (tidak loop ke menu).

---

## 10. Telegram Report

Otomatis 1x tiap start (delay 8 dtk, thread terpisah) + tombol `SEND REPORT`:
waktu, model/brand, Android+SDK, **android_id**, package, pid, status attach,
libbase, versi offset + akun (id, nick, region, country, versi client,
IP:port server) bila kebaca. Transport libcurl statik, timeout 15 dtk.
Konfigurasi: `src/config/TelegramConfig.h`.

---

## 11. Dump libcsharp.so dari HP

Target benar = `libcsharp.so` **99 MB** di
`/data/data/com.mobile.legends/app_libs/` (BUKAN stub 376K di `/data/app`).
Metadata 0B di APK = placeholder; yang asli terproteksi → dump dari RAM:
**Zygisk Il2CppDumper** (otomatis) atau **M5Dumper.lua** di GameGuardian.
Script segmen-mentah: `TatsumiDump_il2cpp.lua` (ada di folder WORKSPACE/ML,
output + manifest `ranges_*.txt`, rebuild pakai SoFixer).

---

## 12. Panduan Development

- **Fitur baru:** state global → logic `Read/Write` pakai macro `OFF_*` →
  render di `DrawMonster`/`Layout_tick_UI` → setting di tab + 1 baris
  `Settings.h` → rebuild.
- **Offset baru:** §8 (file `.offsets` dulu, permanen belakangan).
- **Varian MLBB baru:** 1 baris di `GameConfig.h`.
- Lihat [README_MODULAR.md](README_MODULAR.md) untuk peta detail.

---

## 13. Backup & Versioning

Arsip timestamp di `WORKSPACE/ML/`:
`EXTERNAL-MLBB-TatsumiLoader-YYYYMMDD-HHMM.7z` (exclude `obj/`).
Buat backup tiap habis rebuild berhasil.

---

## 14. FAQ / Troubleshooting

- **`Pointer tag ... truncated`** — warning benign Android 11+, abaikan.
- **Buka Settings langsung keluar** — bug lama, sudah fix (keluar hanya via
  `EXIT CHEAT`).
- **Retri telat** — naikkan `Early Margin`, cek `Max Range`, pas-kan
  `Retri X/Y` ke titik putih `Show Circle`.
- **`SEARCHING GAME...` terus** — SETTINGS → `RE-SCAN GAME`; boot menunggu
  PID 120 dtk otomatis.
- **7-Zip gagal extract RAR** — pakai `unrar x -o+` (metode RAR5 baru).
- **Push GitHub ditolak (file >100MB)** — `SkillIcons.h` via Git LFS.
- **Tap retri tidak kena** — kalibrasi ulang Retri X/Y tiap ganti HP/rotasi;
  `Double Tap` + `Spam Ms` kecil membantu.

---

## 15. Changelog

- **2026-09-30** — Extract 890 file + build NDK r27 sukses.
- **2026-09-30** — Fix crash Settings + watchdog exit loop.
- **2026-09-30** — Rebrand Starcool → TatsumiLoader.
- **2026-09-30** — Auto Retri instan (fix HP, margin, double-tap, fast path).
- **2026-09-30** — Modular: GameConfig, OffsetOverrides, BootFlow,
  Settings table, DeviceReport; auto-launch MLBB + PID polling.
- **2026-09-30** — Tab INFO sosmed IG/TikTok; Telegram device report.
- **2026-09-30** — Push GitHub (LFS) + public.
- **2026-09-30** — Tab AUTO (retri+skill+spell gabung), auto execute, objective alert akurat (fix uint64, % besar, border kedip, ID 2110), hapus tab RETRI/SKILL.

---

## 16. Kontak

- Instagram: https://www.instagram.com/ikyletwar/
- TikTok: https://www.tiktok.com/@ikyletwar

TatsumiLoader © 2026. Personal project.
