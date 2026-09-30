#pragma once
// TatsumiLoader - sentral config varian game.
// Tambah varian baru cukup tambah 1 baris di kGameVariants.

struct GameVariant {
  const char *pkg;
  const char *activity; // UnityPlayerActivity default MLBB
};

// Urutan = prioritas attach (disamakan dgn AttachToGame lama: usa dulu).
inline const GameVariant kGameVariants[] = {
    {"com.mobile.legends.usa", "com.unity3d.player.UnityPlayerActivity"},
    {"com.mobile.legends", "com.unity3d.player.UnityPlayerActivity"},
    {"com.vng.mlbb", "com.unity3d.player.UnityPlayerActivity"},
};
inline const int kGameVariantCount =
    sizeof(kGameVariants) / sizeof(kGameVariants[0]);

inline const char *kUnitySuffix = ":UnityKillsMe";

// Boot flow: tunggu PID sampai timeout ini sebelum menu jalan.
constexpr int kBootWaitSeconds = 120;

// Versi loader (tampil di INFO + laporan Telegram + release). Konvensi semver.
inline const char *kTatsumiVersion = "1.2.1";

// Path file (satu tempat, gampang ganti).
inline const char *kSettingsCfg = "/data/local/tmp/TatsumiLoader.cfg";
inline const char *kOffsetOverrideFile = "/data/local/tmp/TatsumiLoader.offsets";

// Sosmed INFO tab.
inline const char *kInstagramURL = "https://www.instagram.com/ikyletwar/";
inline const char *kTikTokURL = "https://www.tiktok.com/@ikyletwar";
