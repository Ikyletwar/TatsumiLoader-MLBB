// TatsumiLoader - pengirim pesan Telegram via Bot API (libcurl statik).
// Semua pengiriman jalan di thread pemanggil; gunakan TgSendMessageAsync
// agar tidak blokir UI.

#include "features/TelegramReport.h"

#include <curl/curl.h>
#include <cstdio>
#include <string>
#include <thread>
#include <android/log.h>
#define LOGT(...) __android_log_print(ANDROID_LOG_DEBUG, "TatsumiLoader", __VA_ARGS__)

#include "config/TelegramConfig.h"

static size_t tgCapture(char *ptr, size_t size, size_t nmemb, void *ud) {
  auto *s = static_cast<std::string *>(ud);
  s->append(ptr, size * nmemb);
  if (s->size() > 512) s->resize(512);
  return size * nmemb;
}

bool TgSendMessage(const std::string &text) {
  if (!kTgBotToken[0] || !kTgChatId[0])
    return false;
  std::string url =
      std::string("https://api.telegram.org/bot") + kTgBotToken + "/sendMessage";

  // escape JSON minimal untuk text
  std::string esc;
  esc.reserve(text.size() + 16);
  for (char c : text) {
    if (c == '\\' || c == '"')
      esc.push_back('\\');
    esc.push_back(c);
    if (c == '\n') {
      // biarkan newline asli (valid di JSON string)
    }
  }
  std::string body =
      std::string("{\"chat_id\":\"") + kTgChatId + "\",\"text\":\"" + esc + "\"}";

  CURL *curl = curl_easy_init();
  if (!curl)
    return false;
  struct curl_slist *hdr = nullptr;
  hdr = curl_slist_append(hdr, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
  curl_easy_setopt(curl, CURLOPT_CAPATH, "/system/etc/security/cacerts");
  std::string resp;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tgCapture);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  CURLcode rc = curl_easy_perform(curl);
  long http = 0;
  if (rc == CURLE_OK)
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http);
  LOGT("[tg] send rc=%d (%s) http=%ld resp=%.200s", (int)rc,
       rc == CURLE_OK ? "ok" : curl_easy_strerror(rc), http, resp.c_str());
  curl_slist_free_all(hdr);
  curl_easy_cleanup(curl);
  return rc == CURLE_OK && http == 200;
}

void TgSendMessageAsync(std::string text) {
  std::thread([t = std::move(text)]() { TgSendMessage(t); }).detach();
}
