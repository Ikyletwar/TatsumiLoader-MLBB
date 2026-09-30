#pragma once
// TatsumiLoader - format + antrean laporan perangkat.
// main.cpp cukup isi DeviceReport (tanpa urus Telegram/curl) lalu panggil
// QueueDeviceReport(). Pengumpulan info HP (model/android_id) di .cpp.

#include <string>

struct DeviceReport {
  std::string package, pidStr, attachedStr, libbaseHex, offsetBundle;
  bool hasAccount = false;
  std::string accId, nick, region, country, clientVer, gameServer;
};

// Bentuk teks siap kirim (sekaligus isi field perangkat otomatis).
std::string FormatDeviceReport(const DeviceReport &r);

// Kirim di background thread setelah delay (tidak blokir UI).
void QueueDeviceReport(DeviceReport r);
