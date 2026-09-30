// TatsumiLoader - implementasi laporan perangkat (lihat DeviceReport.h).

#include "features/DeviceReport.h"

#include <cstdio>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <sys/system_properties.h>

#include "config/TelegramConfig.h"
#include "features/TelegramReport.h"

static std::string shellOut(const char *cmd) {
  std::string out;
  FILE *fp = popen(cmd, "r");
  if (!fp)
    return out;
  char buf[256];
  while (fgets(buf, sizeof(buf), fp))
    out += buf;
  pclose(fp);
  while (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
    out.pop_back();
  return out;
}

std::string FormatDeviceReport(const DeviceReport &r) {
  char model[PROP_VALUE_MAX] = {0}, brand[PROP_VALUE_MAX] = {0};
  char rel[PROP_VALUE_MAX] = {0}, sdk[PROP_VALUE_MAX] = {0};
  __system_property_get("ro.product.model", model);
  __system_property_get("ro.product.brand", brand);
  __system_property_get("ro.build.version.release", rel);
  __system_property_get("ro.build.version.sdk", sdk);
  std::string androidId = shellOut("settings get secure android_id 2>/dev/null");
  std::time_t now = std::time(nullptr);
  char tbuf[32] = {0};
  strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

  std::string s;
  s += "[TatsumiLoader v";
  s += r.version.empty() ? "?" : r.version;
  s += "] device report\n";
  s += "time: ";
  s += tbuf;
  s += "\nmodel: ";
  s += brand;
  s += " ";
  s += model;
  s += "\nandroid: ";
  s += rel;
  s += " (sdk ";
  s += sdk;
  s += ")\nandroid_id: ";
  s += androidId.empty() ? "-" : androidId;
  s += "\npackage: ";
  s += r.package;
  s += "\npid: ";
  s += r.pidStr;
  s += "\nattached: ";
  s += r.attachedStr;
  s += "\nlibbase: ";
  s += r.libbaseHex;
  s += "\noffsets: ";
  s += r.offsetBundle;
  s += "\n";
  if (r.hasAccount) {
    s += "acc_id: " + r.accId + "\nnick: " +
         (r.nick.empty() ? "-" : r.nick) + "\nregion: " + r.region +
         "\ncountry: " + (r.country.empty() ? "-" : r.country) +
         "\nver: " + (r.clientVer.empty() ? "-" : r.clientVer) +
         "\ngs: " + r.gameServer + "\n";
  } else {
    s += "account: (belum kebaca saat kirim)\n";
  }
  return s;
}

void QueueDeviceReport(DeviceReport r) {
  std::thread([r = std::move(r)]() {
    sleep(kTgReportDelaySec);
    TgSendMessage(FormatDeviceReport(r));
  }).detach();
}
