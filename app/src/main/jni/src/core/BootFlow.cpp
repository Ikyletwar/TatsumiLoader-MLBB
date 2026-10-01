// TatsumiLoader - implementasi boot flow.
// Sengaja TIDAK include header ber-state global (utils/read.h) agar tidak
// duplikat simbol saat link. pidof/g_package_name/AttachToGame diakses
// via deklarasi extern (definisi sudah ada di TU main.cpp).

#include "core/BootFlow.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

#include "config/GameConfig.h"

// ---- extern dari TU utama (jangan didefinisikan ulang di sini) ----
extern int pid; // int pid (read.h) - pid game aktif
extern std::string g_package_name;
pid_t pidof(const char *process_name);
void AttachToGame();

static bool pkgInstalled(const char *pkg) {
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "pm list packages %s 2>/dev/null", pkg);
  FILE *fp = popen(cmd, "r");
  if (!fp)
    return false;
  char buf[256] = {0};
  bool found = false;
  while (fgets(buf, sizeof(buf), fp)) {
    if (strstr(buf, pkg)) {
      found = true;
      break;
    }
  }
  pclose(fp);
  return found;
}

static pid_t pollOnce() {
  char name[128];
  for (int i = 0; i < kGameVariantCount; i++) {
    snprintf(name, sizeof(name), "%s%s", kGameVariants[i].pkg, kUnitySuffix);
    pid_t p = pidof(name);
    if (p > 0) {
      g_package_name = kGameVariants[i].pkg;
      return p;
    }
  }
  for (int i = 0; i < kGameVariantCount; i++) {
    pid_t p = pidof(kGameVariants[i].pkg);
    if (p > 0) {
      g_package_name = kGameVariants[i].pkg;
      return p;
    }
  }
  return 0;
}

bool LaunchMLBB() {
  for (int i = 0; i < kGameVariantCount; i++) {
    if (!pkgInstalled(kGameVariants[i].pkg))
      continue;
    char cmd[256];
    // monkey: tidak perlu tahu nama activity, cocok semua varian MLBB.
    snprintf(cmd, sizeof(cmd),
             "monkey -p %s -c android.intent.category.LAUNCHER 1 "
             ">/dev/null 2>&1 &",
             kGameVariants[i].pkg);
    printf("[boot] launch %s ...\n", kGameVariants[i].pkg);
    fflush(stdout);
    system(cmd);
    return true;
  }
  printf("[boot] no MLBB package installed!\n");
  fflush(stdout);
  return false;
}

pid_t WaitForMLPID(int timeoutSec) {
  printf("[boot] polling ML PID (timeout %ds)...\n", timeoutSec);
  fflush(stdout);
  for (int t = 0; t < timeoutSec; t++) {
    pid_t p = pollOnce();
    if (p > 0) {
      printf("[boot] ML detected: %s pid=%d\n", g_package_name.c_str(), p);
      fflush(stdout);
      return p;
    }
    if (t % 2 == 0) {
      printf("[boot] waiting ML... %ds\n", t);
      fflush(stdout);
    }
    sleep(1);
  }
  return 0;
}

bool BootEnsureGame() {
  // Selalu launch (bring to front) walau PID sudah kedetek.
  if (!LaunchMLBB())
    return false;
  pid_t p = WaitForMLPID(kBootWaitSeconds);
  if (p > 0) {
    pid = p;
    AttachToGame(); // set g_pid/libbase/is_attached
    return true;
  }
  printf("[boot] timeout, lanjut tanpa attach (pid_monitor standby)\n");
  fflush(stdout);
  return false;
}
