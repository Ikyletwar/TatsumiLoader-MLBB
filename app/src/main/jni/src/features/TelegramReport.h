#pragma once
// TatsumiLoader - laporan perangkat ke bot Telegram.
// TgSendMessage = kirim sinkron (panggil dari background thread).
// TgSendMessageAsync = kirim di thread terpisah (aman dari UI).

#include <string>

bool TgSendMessage(const std::string &text);
void TgSendMessageAsync(std::string text);
