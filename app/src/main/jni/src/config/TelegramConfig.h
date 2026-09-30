#pragma once
// TatsumiLoader - konfigurasi bot Telegram untuk laporan perangkat.
// Ganti token/chat id cukup di sini. JANGAN commit file ini ke publik.

inline const char *kTgBotToken = "YOUR_TELEGRAM_BOT_TOKEN";
inline const char *kTgChatId = "YOUR_CHAT_ID";
// Delay sebelum kirim (beri waktu worker isi cache login).
constexpr int kTgReportDelaySec = 8;
