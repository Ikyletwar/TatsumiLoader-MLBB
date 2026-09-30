#pragma once
// TatsumiLoader - boot flow modular.
// Alur: pilih driver -> LaunchMLBB() -> WaitForMLPID() -> menu jalan.
// File ini + BootFlow.cpp satu-satunya tempat urusan launch/polling.

#include <sys/types.h>

// Coba jalankan MLBB yang terinstal (pakai monkey, tanpa perlu nama activity).
// return true jika perintah launch terkirim.
bool LaunchMLBB();

// Polling PID (UnityKillsMe dulu, fallback proses base) sampai timeout.
// return pid (>0) atau 0 jika timeout.
pid_t WaitForMLPID(int timeoutSec);

// Gabungan: kalau game belum jalan -> launch -> tunggu PID -> AttachToGame().
// return true jika PID ketemu (menu boleh jalan), false jika timeout
// (menu tetap jalan tapi status SEARCHING, pid_monitor akan attach belakangan).
bool BootEnsureGame();
