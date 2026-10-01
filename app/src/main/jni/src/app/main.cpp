#include "main.h"
#include "Engine/CanvasView.h"
#include "Vector/Matrix4x4.hpp"
#include "Memory/Memory.h"
#include "Memory/PatternScanner.h"
#include "Vector/Quaternion.hpp"
#include "Vector/ToString.h"
#include "Vector/Vector2.hpp"
#include "Vector/Vector3.hpp"
#include "utils/include.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <exception>
#include <fcntl.h>
#include <signal.h>
#include <sys/file.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <linux/input.h>
#include <linux/uinput.h>
#include <malloc.h>
#include <mutex>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/system_properties.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include "config/Offsets.h"
#include "config/GameConfig.h"
#include "config/OffsetOverrides.h"
#include "core/BootFlow.h"
#include "core/Settings.h"
#include "utils/Decoder64.h"
#include "utils/xorstr.hpp"
#include "Includes/GUI_Custom.h"
#include "icon/HeroIcons.h"
#include "icon/Logo.h"
#include "icon/Rank/RankIcons_loader.h"
#include "icon/Spell/SpellIcons_loader.h"
#include "icon/skill_mlbb/SkillIcons_loader.h"
#include "features/RoomInfo.h"
#include <sys/stat.h>




extern bool isBeingBlocked;
#include "ui/DrawIconHero.h"
#include "ui/Toggle.h"

using namespace Memory;
bool is_root_mode = true;
bool main_thread_flag = true;
static bool show_menu = true;
int uiFpsCap = 60;
float uiUserScale = 1.15f;
std::string g_package_name = xorstr_("com.mobile.legends");


std::atomic<bool> g_IsDeployingSkin(false);
std::string g_SkinStatus = xorstr_("Ready");



struct ScanStatus {
    bool scanned = false;
    bool bm_ok = false;
    bool cam_ok = false;
    bool lbm_ok = false;
    bool sd_ok = false;
    uintptr_t bm_addr = 0;
    uintptr_t cam_addr = 0;
    uintptr_t lbm_addr = 0;
    uintptr_t sd_addr = 0;
};
ScanStatus g_ScanStatus;

struct LoginServerData {
    uint64_t AccountId;
    std::string Nickname; 
    std::string ClientRealVersion;
    std::string ClientStreamVersion;
    std::string ClientLocalVersion;
    std::string ClientLoginVersion;
    std::string Channel;
    bool IsNewAccount;
    uint32_t RegionId;
    std::string CountryInfo;
    std::string CountryInfoByIP;
    std::string InternetOperator;
    std::string State;
    uint32_t AccCreateTime;
    uint32_t AccountFlag;
    std::string UserIP;
    std::string GameServerIP;
    int32_t GameServerPort;
    std::string EngineBuildVersion;
    bool IsValid;
    LoginServerData()
            : AccountId(0), IsNewAccount(false), RegionId(0), AccCreateTime(0),
              AccountFlag(0), GameServerPort(0), IsValid(false) {}
};
extern LoginServerData g_LoginServerCache;


int abs_ScreenX = 0;
int abs_ScreenY = 0;
bool drawMAddress;
bool drawMLine = true;
bool drawMHealth = true;
int g_LocalPlayerCamp_UNUSED =
        1; 
bool drawAlertUnderAttack = true;
float Alert_PosX = 0.5f;
float Alert_PosY = 0.15f;
float Alert_Scale = 1.0f;
bool Alert_ShowHPText = true;
bool iconhero = true;
bool drawMAvatar = true;
float Avatar_OffsetY = 80.0f; 
bool drawMinionMinimap = true;
bool drawMonsterHealth = true;
bool drawMonsterID = false;
bool drawMonsterName = true;
bool showRetriCircle = true;
float minionMinimapSize = 4.0f;
int g_ICSize = 4; 
int MinimapSize = 342;
int MinimapPos = 76;
int MinimapPosY = 76;
bool MinimapIcon = true;
bool MinimapIconBuff = false;
bool MinimapIconLordTurtle = true;
bool drawMonsterMinimap = true;

bool HideLine = false;
float RadiusCir = 50.0f;
float g_HeroIconSize = 28.0f;
float g_MonsterIconSize = 22.0f;
bool is_attached = false;
bool EnableDrone = false;
bool useAdGuardDns = false;
bool enableDOD = false;


struct SwordInfo {
    uintptr_t bulletAddr;
    Vector3 position;
    float lastSeenTime;
    bool active;
};
SwordInfo g_SwordCache[4]; 

bool autoSwordLing = false;
bool debugSwordID = false;
bool g_SwordChasing = false;
int lingSwordID = 8452;
float lingSwordDashRange = 12.0f;
float lingDashDelay = 0.35f; 
float lingSkill2X = 1850.0f;
float lingSkill2Y = 850.0f;


bool autoComboGusion = false;
int gusionMarkID = 5610;
int gusionComboDelay = 60; 
float gusionSkill1X = 1850.0f, gusionSkill1Y = 850.0f;
float gusionSkill2X = 1650.0f, gusionSkill2Y = 950.0f;
float gusionSkill3X = 1550.0f, gusionSkill3Y = 650.0f;
int gusionState = 0; 

float FieldView = -1.0f;
long libbase = 0;
void AttachToGame() {
  // Modular: daftar paket terpusat di config/GameConfig.h (tambah varian = 1 baris).
  pid = 0;
  for (int i = 0; i < kGameVariantCount && pid <= 0; i++) {
    std::string proc = std::string(kGameVariants[i].pkg) + kUnitySuffix;
    pid = pidof(proc.c_str());
    if (pid > 0) g_package_name = kGameVariants[i].pkg;
  }
  for (int i = 0; i < kGameVariantCount && pid <= 0; i++) {
    pid = pidof(kGameVariants[i].pkg);
    if (pid > 0) g_package_name = kGameVariants[i].pkg;
  }

  if (pid > 0) {
    g_pid = pid;
    if (g_driver) {
      g_driver->initialize(g_pid);
    }

    
    libbase = GetBase(xorstr_("libcsharp.so"));
    if (libbase == 0) {
      libbase = GetBase(xorstr_("libil2cpp.so"));
    }

    if (libbase != 0) {
      is_attached = true;

    }
  }
}
std::string fshy(uintptr_t address) {
  if (!address)
    return "";
  uint32_t stringLength = Read<uint32_t>(address + OFF_STR(LengthOffset));
  if (stringLength == 0)
    return "";
  if (stringLength > 1024)
    stringLength = 1024;
  std::vector<char16_t> buffer(stringLength + 1, 0);
  uintptr_t stringPtr = address + OFF_STR(BufferOffset);
  if (!stringPtr)
    return "";
  if (!pvm(reinterpret_cast<void *>(stringPtr), buffer.data(),
           static_cast<size_t>(stringLength) * 2, false)) {
    return "";
  }
  return utf16_to_utf8(buffer.data(), stringLength);
}
struct RoomPlayerInfo {
    std::string Name;
    std::string UserID;
    std::string Squad;
    std::string Rank;
    std::string Hero;
    std::string Spell;
};
RoomPlayerInfo PlayerB[5];
RoomPlayerInfo PlayerR[5];
struct String {
    char pad_0000[0x10];
    int length;
    wchar_t buffer[1];
    const char *CString() const {
      static char temp[256];
      wcstombs(temp, buffer, length);
      temp[length] = '\0';
      return temp;
    }
};
uintptr_t GetMainCamera() {
  auto main_cam = Read<uintptr_t>(libbase + OFF_CAM(MainCameraPtr));
  if (!main_cam)
    return 0;
  auto main_cam2 = Read<uintptr_t>(main_cam + OFF_CAM(CameraData));
  if (!main_cam2)
    return 0;
  auto main_cam3 = Read<uintptr_t>(main_cam2 + OFF_CAM(CameraData2));
  if (!main_cam3)
    return 0;
  return main_cam3;
}
struct Camera {
    Matrix4x4 worldToCameraMatrix;
    Matrix4x4 projectionMatrix;
};
Matrix4x4 _vMatrix;
bool WorldToScreen(Vector3 from, Vector2 *to) {
  float w = _vMatrix.m30 * from.X + _vMatrix.m31 * from.Y +
            _vMatrix.m32 * from.Z + _vMatrix.m33;
  if (w < 0.01f)
    return false;

  auto viewMatrix = _vMatrix.MultiplyPoint(from);
  auto screenPos =
          Vector3(viewMatrix.X + 1.0f, viewMatrix.Y + 1.0f, viewMatrix.Z + 1.0f) /
          2.0f;
  *to = Vector2(screenPos.X * abs_ScreenX,
                abs_ScreenY - (screenPos.Y * abs_ScreenY));

  return true;
}
void FindPoint(Vector2 origin, Vector2 &point, int screenwidth,
               int screenheight, int length) {
  float halfScreenWidth = screenwidth / 2.0f;
  float halfScreenHeight = screenheight / 2.0f;
  float halfScreenWidth2 = (screenwidth - length) / 2.0f;
  float halfScreenHeight2 = (screenheight - length) / 2.0f;
  float dx = fabs(origin.X - halfScreenWidth);
  float dy = fabs(origin.Y - halfScreenHeight);
  float rx = (dx != 0) ? halfScreenWidth2 / dx : 0;
  float ry = (dy != 0) ? halfScreenHeight2 / dy : 0;
  float r = fmin(rx, ry);
  point.X = origin.X + (halfScreenWidth - origin.X) * (1.0f - r);
  point.Y = origin.Y + (halfScreenHeight - origin.Y) * (1.0f - r);
}
int ListMonsterId[] = {
        2002, 2003, 2004, 2005, 2006, 2008, 2009, 2011, 2012,
        2013, 2056, 2059, 2072, 2220, 2221, 2222, 2223, 2224,
        2225, 2226, 2227, 2228, 2229, 2230, 2232,
};
bool bMonster(int iValue) {
  return std::find(std::begin(ListMonsterId), std::end(ListMonsterId),
                   iValue) != std::end(ListMonsterId);
}
void Touch_Tap(int x, int y) {
  Touch_Down((float)x, (float)y);
  Touch_Up();
}
bool lastRetriTriggered[20] = {false};
bool autoRetribution = false;
bool AutoRetributionRed = true;
bool AutoRetributionBlue = true;
bool AutoRetributionLord = true;
bool AutoRetributionTurtle = true;
bool AutoRetributionCrab = true;
bool AutoRetributionLito = true;
float retriTouchX = 1575;
float retriTouchY = 661;
// ADVANCE RETRI TUNING (instant mode)
float retriEarlyMargin = 180.0f;   // tap lebih awal utk kompensasi network/tick delay
float retriMaxRange = 8.5f;        // sedikit longgar dari 8.0 bawaan game
int retriSpamMs = 80;              // jeda minimal antar tap (anti flood, tapi retry cepat)
bool retriDoubleTap = true;        // tap 2x per trigger biar tidak miss
bool retriNearbyOnly = false;
int g_retriDbgHP = -1, g_retriDbgDmg = 0, g_retriDbgID = 0, g_retriDbgLvl = 0;
float g_retriDbgDist = -1.0f;
float retriNearbyRange = 12.0f;
bool autoSpellExecute = false;
float spellExecPct = 12.0f;
float spellRange = 5.0f;
float spellX = 1700.0f;
float spellY = 850.0f;
int spellSpamMs = 1000;
bool drawObjectiveAlert = true;
float objectiveLowPct = 20.0f;
void DrawVerticalHealthBar(ImDrawList *Draw, float X, float Y, float Height,
                           float Health, float MaxHealth,
                           bool ShowText = false) {
  if (MaxHealth <= 0) return;
  float HealthPercent = Health / MaxHealth;
  if (HealthPercent < 0.0f) HealthPercent = 0.0f;
  if (HealthPercent > 1.0f) HealthPercent = 1.0f;

  float BarWidth = 5.0f;
  float BarGap = 22.0f; 
  float FilledHeight = Height * HealthPercent;

  
  float BarX0 = X - BarGap - BarWidth;
  float BarX1 = BarX0 + BarWidth;
  float Top = Y - Height * 0.5f;
  float Bottom = Y + Height * 0.5f;

  
  Draw->AddRectFilled(ImVec2(BarX0 - 1.5f, Top - 1.5f),
                      ImVec2(BarX1 + 1.5f, Bottom + 1.5f),
                      IM_COL32(0, 0, 0, 220), 2.0f);

  
  ImU32 HealthColor;
  if (HealthPercent > 0.6f) HealthColor = IM_COL32(0, 255, 120, 255);
  else if (HealthPercent > 0.3f) HealthColor = IM_COL32(255, 210, 0, 255);
  else HealthColor = IM_COL32(255, 50, 50, 255);

  
  if (HealthPercent > 0) {
    Draw->AddRectFilled(ImVec2(BarX0, Bottom - FilledHeight),
                        ImVec2(BarX1, Bottom), HealthColor, 2.0f);

    
    Draw->AddRectFilled(ImVec2(BarX0, Bottom - FilledHeight),
                        ImVec2(BarX0 + 1.5f, Bottom), IM_COL32(255, 255, 255, 60), 2.0f);
  }

  
  if (ShowText) {
    std::string HealthText = std::to_string((int)Health);
    ImVec2 TextSize = ImGui::CalcTextSize(HealthText.c_str());
    ImVec2 TextPos(BarX0 - TextSize.x - 4.0f, Bottom - FilledHeight - TextSize.y * 0.5f);

    
    Draw->AddText(ImVec2(TextPos.x + 1, TextPos.y + 1), IM_COL32(0, 0, 0, 200), HealthText.c_str());
    
    Draw->AddText(TextPos, IM_COL32(255, 255, 255, 230), HealthText.c_str());
  }
}

void DrawPremiumHealthBar(ImDrawList *draw, ImVec2 pos, float hp, float maxHp,
                          float width = 100.0f, float height = 8.0f,
                          bool showText = false) {
  if (maxHp <= 0)
    return;
  float hpPercent = hp / maxHp;
  if (hpPercent > 1.0f)
    hpPercent = 1.0f;
  if (hpPercent < 0.0f)
    hpPercent = 0.0f;

  ImVec2 min = ImVec2(pos.x - width * 0.5f, pos.y - height * 0.5f);
  ImVec2 max = ImVec2(pos.x + width * 0.5f, pos.y + height * 0.5f);

  
  draw->AddRectFilled(ImVec2(min.x - 1, min.y - 1),
                      ImVec2(max.x + 1, max.y + 1), IM_COL32(0, 0, 0, 220),
                      2.0f);
  draw->AddRectFilled(min, max, IM_COL32(50, 50, 50, 255), 2.0f);

  
  ImU32 hpColor;
  bool isEnemy = true; 
  if (isEnemy) {
    if (hpPercent > 0.5f)
      hpColor = IM_COL32(255, 200, 0, 255); 
    else
      hpColor = IM_COL32(255, 50, 50, 255); 
  } else {
    if (hpPercent > 0.7f)
      hpColor = IM_COL32(0, 255, 100, 255); 
    else if (hpPercent > 0.3f)
      hpColor = IM_COL32(255, 220, 0, 255); 
    else
      hpColor = IM_COL32(255, 50, 50, 255); 
  }

  
  if (hpPercent > 0) {
    ImVec2 hpMax = ImVec2(min.x + (width * hpPercent), max.y);
    draw->AddRectFilled(min, hpMax, hpColor, 2.0f);

    
    draw->AddRectFilled(min, ImVec2(hpMax.x, min.y + height * 0.4f),
                        IM_COL32(255, 255, 255, 60), 2.0f);
  }

  
  for (int i = 1; i < 10; i++) {
    float x = min.x + (width * 0.1f * i);
    draw->AddLine(ImVec2(x, min.y), ImVec2(x, max.y), IM_COL32(0, 0, 0, 150),
                  1.0f);
  }

  
  if (showText) {
    char hpBuf[32];
    snprintf(hpBuf, sizeof(hpBuf), "%.0f / %.0f", hp, maxHp);
    ImVec2 textSize = ImGui::CalcTextSize(hpBuf);
    draw->AddText(ImVec2(pos.x - textSize.x * 0.5f + 1,
                         pos.y - height * 0.5f - textSize.y - 1),
                  IM_COL32(0, 0, 0, 200), hpBuf);
    draw->AddText(ImVec2(pos.x - textSize.x * 0.5f,
                         pos.y - height * 0.5f - textSize.y - 2),
                  IM_COL32(255, 255, 255, 255), hpBuf);
  }
}

void DrawGlobalWarning(ImDrawList *draw, const char *label, float hp,
                       float maxHp) {
  if (maxHp <= 0 || !drawAlertUnderAttack)
    return;

  float centerX = abs_ScreenX * Alert_PosX;
  float centerY = abs_ScreenY * Alert_PosY;
  float s = Alert_Scale;

  
  ImVec2 labelSize = ImGui::CalcTextSize(label);
  float boxWidth = 320.0f * s;
  float boxHeight = 70.0f * s;
  float barWidth = 280.0f * s;
  float barHeight = 22.0f * s;

  
  ImVec2 boxMin(centerX - boxWidth * 0.5f, centerY - boxHeight * 0.5f);
  ImVec2 boxMax(centerX + boxWidth * 0.5f, centerY + boxHeight * 0.5f);

  
  draw->AddRectFilled(boxMin, boxMax, IM_COL32(15, 15, 15, 230), 12.0f * s);
  
  float warnPct = (maxHp > 0) ? hp / maxHp : 1.0f;
  bool warnLow = (warnPct * 100.0f) <= objectiveLowPct;
  bool warnBlink = ((std::clock() / (CLOCKS_PER_SEC / 2)) % 2) == 0;
  ImU32 warnBorder = warnLow ? (warnBlink ? IM_COL32(255, 0, 0, 255) : IM_COL32(120, 0, 0, 255))
                             : IM_COL32(230, 230, 100, 255);
  draw->AddRect(boxMin, boxMax, warnBorder, 12.0f * s, 0,
                (warnLow ? 4.0f : 2.0f) * s);

  
  ImVec2 labelPos(centerX - labelSize.x * s * 0.5f, boxMin.y + 12.0f * s);
  draw->AddText(NULL, 19.0f * s, labelPos, IM_COL32(255, 255, 255, 255), label);

  
  ImVec2 barMin(centerX - barWidth * 0.5f, boxMax.y - barHeight - 12.0f * s);
  ImVec2 barMax(centerX + barWidth * 0.5f, boxMax.y - 12.0f * s);

  
  draw->AddRectFilled(barMin, barMax, IM_COL32(45, 45, 45, 255), 8.0f * s);

  float hpPercent = hp / maxHp;
  if (hpPercent > 0) {
    if (hpPercent > 1.0f)
      hpPercent = 1.0f;
    float fillWidth = barWidth * hpPercent;
    
    draw->AddRectFilled(barMin, ImVec2(barMin.x + fillWidth, barMax.y),
                        IM_COL32(230, 0, 0, 255), 8.0f * s);
  }

  
  {
    char pctBuf[16];
    snprintf(pctBuf, sizeof(pctBuf), "%d%%", (int)(warnPct * 100.0f));
    ImVec2 pctSize = ImGui::CalcTextSize(pctBuf);
    ImVec2 pctPos(centerX - pctSize.x * s * 0.9f, boxMin.y - pctSize.y * s * 1.1f);
    draw->AddText(NULL, 30.0f * s, pctPos,
                  warnLow ? IM_COL32(255, 50, 50, 255) : IM_COL32(255, 255, 255, 255), pctBuf);
  }
  if (Alert_ShowHPText) {
    char hpBuf[64];
    snprintf(hpBuf, sizeof(hpBuf), "%.0f / %.0f", hp, maxHp);
    ImVec2 hpSize = ImGui::CalcTextSize(hpBuf);
    ImVec2 hpPos(centerX - hpSize.x * s * 0.45f,
                 barMin.y + (barHeight - hpSize.y * s * 0.8f) * 0.5f);
    draw->AddText(NULL, 14.0f * s, hpPos, IM_COL32(255, 255, 255, 255), hpBuf);
  }
}

namespace Prediction {
    namespace Fields {
        constexpr size_t m_vCachePosition = 0x28c;
        constexpr size_t _MoveDir = 0x2cc;
        constexpr size_t _vecFaceDir = 0x2d8;
        constexpr size_t curSpeed = 0x480;
        constexpr size_t m_dMoveSpeed = 0x190;
        constexpr size_t m_dRunSpeed = 0x1d0;
        constexpr size_t m_bIsInMoveControl = 0x372;
    } 
    struct EntityPrediction {
        Vector3 position;
        Vector3 moveDir;
        float speed;
        bool isValid;
        bool isMoving;
        float lastUpdateTime;
    };
    struct PredictionCache {
        uintptr_t entityAddr;
        EntityPrediction pred;
        bool active;
    };
    PredictionCache predCache[150];
    constexpr int MAX_CACHE = 150;
    constexpr float VISUAL_SCALE = 1.0f;
    constexpr float MIN_SPEED_THRESHOLD = 0.3f;
    constexpr float MAX_VISUAL_LENGTH = 120.0f;
    inline float GetEntitySpeed(uintptr_t entityAddr) {
      float speed = Read<float>(entityAddr + Fields::curSpeed);
      if (speed > 0.01f)
        return speed;
      double moveSpeed = Read<double>(entityAddr + Fields::m_dMoveSpeed);
      if (moveSpeed > 0.01)
        return static_cast<float>(moveSpeed);
      double runSpeed = Read<double>(entityAddr + Fields::m_dRunSpeed);
      return static_cast<float>(runSpeed);
    }
    inline Vector3 GetEntityMoveDir(uintptr_t entityAddr) {
      Vector3 moveDir = Read<Vector3>(entityAddr + Fields::_MoveDir);
      float len = sqrtf(moveDir.X * moveDir.X + moveDir.Y * moveDir.Y +
                        moveDir.Z * moveDir.Z);
      if (len > 0.1f) {
        return {moveDir.X / len, moveDir.Y / len, moveDir.Z / len};
      }
      Vector3 faceDir = Read<Vector3>(entityAddr + Fields::_vecFaceDir);
      len = sqrtf(faceDir.X * faceDir.X + faceDir.Y * faceDir.Y +
                  faceDir.Z * faceDir.Z);
      if (len > 0.1f) {
        return {faceDir.X / len, faceDir.Y / len, faceDir.Z / len};
      }
      return {0, 0, 0};
    }
    void UpdatePrediction(uintptr_t entityAddr, Vector3 currentPos,
                          float currentTime) {
      for (int i = 0; i < MAX_CACHE; i++) {
        if (predCache[i].active && predCache[i].entityAddr == entityAddr) {
          float dt = currentTime - predCache[i].pred.lastUpdateTime;
          Vector3 oldPos = predCache[i].pred.position;

          
          float speed = GetEntitySpeed(entityAddr) * 0.013f;
          Vector3 moveDir = GetEntityMoveDir(entityAddr);

          if (dt > 0.001f && dt < 0.2f && (currentPos.X != oldPos.X || currentPos.Z != oldPos.Z)) {
            Vector3 velocity = { (currentPos.X - oldPos.X) / dt, 0, (currentPos.Z - oldPos.Z) / dt };
            float realSpeed = sqrtf(velocity.X * velocity.X + velocity.Z * velocity.Z);
            if (realSpeed > 0.3f) { 
              speed = realSpeed;
              moveDir = { velocity.X / realSpeed, 0, velocity.Z / realSpeed };
            }
          }

          predCache[i].pred.position = currentPos;
          predCache[i].pred.moveDir = moveDir;
          predCache[i].pred.speed = speed;
          predCache[i].pred.isValid = true;
          predCache[i].pred.isMoving = (speed > 0.1f);
          predCache[i].pred.lastUpdateTime = currentTime;
          return;
        }
      }
      for (int i = 0; i < MAX_CACHE; i++) {
        if (!predCache[i].active) {
          predCache[i].entityAddr = entityAddr;
          predCache[i].pred.position = currentPos;
          predCache[i].pred.moveDir = GetEntityMoveDir(entityAddr);
          predCache[i].pred.speed = GetEntitySpeed(entityAddr) * 0.013f;
          predCache[i].pred.isValid = true;
          predCache[i].pred.isMoving = (predCache[i].pred.speed > 0.1f);
          predCache[i].pred.lastUpdateTime = currentTime;
          predCache[i].active = true;
          return;
        }
      }
    }
    EntityPrediction *GetPrediction(uintptr_t entityAddr) {
      for (int i = 0; i < MAX_CACHE; i++) {
        if (predCache[i].active && predCache[i].entityAddr == entityAddr) {
          return &predCache[i].pred;
        }
      }
      return nullptr;
    }
    void InvalidatePrediction(uintptr_t entityAddr) {
      for (int i = 0; i < MAX_CACHE; i++) {
        if (predCache[i].active && predCache[i].entityAddr == entityAddr) {
          predCache[i].active = false;
          return;
        }
      }
    }
    void CleanupStale(float currentTime, float maxAge = 2.0f) {
      for (int i = 0; i < MAX_CACHE; i++) {
        if (predCache[i].active) {
          float age = currentTime - predCache[i].pred.lastUpdateTime;
          if (age > maxAge) {
            predCache[i].active = false;
          }
        }
      }
    }
    inline float CalcVisualLength(float speed) {
      float raw = speed * 8.0f * VISUAL_SCALE;
      return fminf(raw, MAX_VISUAL_LENGTH);
    }
    inline ImU32 GetSpeedColor(float speed) {
      float clamped = fminf(fmaxf(speed, 1.0f), 20.0f);
      float t = (clamped - 1.0f) / 19.0f;
      if (t < 0.33f) {
        float lt = t / 0.33f;
        return IM_COL32(static_cast<int>(255.0f * lt), 255, 0, 200);
      } else if (t < 0.66f) {
        float lt = (t - 0.33f) / 0.33f;
        return IM_COL32(255, static_cast<int>(255.0f * (1.0f - lt * 0.5f)), 0, 200);
      } else {
        float lt = (t - 0.66f) / 0.34f;
        return IM_COL32(255, static_cast<int>(127.0f * (1.0f - lt)), 0, 200);
      }
    }
} 
bool drawMPrediction = false;
bool predictionColorBySpeed = true;
ImColor predictionLineColor = ImColor(0, 255, 255, 180);
ImColor predictionCircleColor = ImColor(255, 255, 255, 190);
float predictionCircleRadius = 5.0f;
bool predictionShowOnlyMoving = true;
bool predictionShowArrowHead = true;
bool predictionShowSpeedText = false;
void DrawPrediction(ImDrawList *draw, Vector2 screenPos, Vector3 worldPos,
                    uintptr_t entityAddr) {
  using namespace Prediction;
  float currentTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
  UpdatePrediction(entityAddr, worldPos, currentTime);
  EntityPrediction *pred = GetPrediction(entityAddr);
  if (!pred || !pred->isValid)
    return;
  if (predictionShowOnlyMoving && !pred->isMoving)
    return;
  if (pred->moveDir.X == 0 && pred->moveDir.Y == 0 && pred->moveDir.Z == 0)
    return;
  constexpr float TIME_VISUAL_FACTOR = 1.0f;
  Vector3 predictedWorld = {
          worldPos.X + pred->moveDir.X * pred->speed * TIME_VISUAL_FACTOR,
          worldPos.Y + pred->moveDir.Y * pred->speed * TIME_VISUAL_FACTOR,
          worldPos.Z + pred->moveDir.Z * pred->speed * TIME_VISUAL_FACTOR};
  Vector2 predictedScreen;
  if (!WorldToScreen(predictedWorld, &predictedScreen))
    return;
  float lineLength = CalcVisualLength(pred->speed);
  ImU32 lineColor = predictionColorBySpeed ? GetSpeedColor(pred->speed)
                                           : IM_COL32(0, 255, 255, 200);
  float lineThickness = 1.5f + fminf(pred->speed * 0.1f, 2.0f);
  draw->AddLine(ImVec2(screenPos.X, screenPos.Y),
                ImVec2(predictedScreen.X, predictedScreen.Y), lineColor,
                lineThickness);
  if (predictionShowArrowHead && lineLength > 20.0f) {
    float arrowSize = fminf(10.0f, lineLength * 0.3f);
    float angle = atan2f(predictedScreen.Y - screenPos.Y,
                         predictedScreen.X - screenPos.X);
    ImVec2 arrow1 = {predictedScreen.X - arrowSize * cosf(angle - 0.4f),
                     predictedScreen.Y - arrowSize * sinf(angle - 0.4f)};
    ImVec2 arrow2 = {predictedScreen.X - arrowSize * cosf(angle + 0.4f),
                     predictedScreen.Y - arrowSize * sinf(angle + 0.4f)};
    draw->AddLine(ImVec2(predictedScreen.X, predictedScreen.Y), arrow1,
                  lineColor, lineThickness * 0.9f);
    draw->AddLine(ImVec2(predictedScreen.X, predictedScreen.Y), arrow2,
                  lineColor, lineThickness * 0.9f);
  }
  draw->AddCircle(ImVec2(predictedScreen.X, predictedScreen.Y),
                  predictionCircleRadius, predictionCircleColor, 14, 1.2f);
  draw->AddCircleFilled(ImVec2(predictedScreen.X, predictedScreen.Y),
                        predictionCircleRadius * 0.35f, predictionCircleColor);
  if (predictionShowSpeedText && pred->speed > 1.0f) {
    char speedTxt[12];
    snprintf(speedTxt, sizeof(speedTxt), xorstr_("%.1f"), pred->speed);
    ImVec2 txtSize = ImGui::CalcTextSize(speedTxt);
    draw->AddText(
            ImVec2(predictedScreen.X + 6.0f, predictedScreen.Y - txtSize.y * 0.5f),
            IM_COL32(255, 255, 255, 220), speedTxt);
  }
}
uintptr_t GetLogicBattleManagerInstance() {
  auto logicBattleManager = Read<uintptr_t>(libbase + OFF_BLC(BasePtr));
  if (!logicBattleManager)
    return 0;
  auto staticPtr = Read<uintptr_t>(logicBattleManager + OFF_BLC(PtrChain1));
  if (!staticPtr)
    return 0;
  auto instancePtr = Read<uintptr_t>(staticPtr);
  if (!instancePtr)
    return 0;
  return instancePtr;
}
struct CoolDownData {
    int skill1, skill2, skill3, skill4, spell, regen;
    int spellId;
    CoolDownData()
            : skill1(0), skill2(0), skill3(0), skill4(-1), spell(0), regen(0), spellId(0) {}
};
struct CoolDownTimerState {
  uint32_t startTime = 0;
  uint32_t durationMs = 0;
  std::chrono::steady_clock::time_point observedAt{};
};

std::unordered_map<uintptr_t, CoolDownTimerState> g_CoolDownTimers;

bool ESP_Player_Cooldown = true;
bool Cooldown_ShowSpell = true;
bool Cooldown_CompactStyle = false;
ImColor Cooldown_ColorReady = ImColor(0, 255, 0, 255);
ImColor Cooldown_ColorCD = ImColor(255, 80, 80, 255);
float Cooldown_OffsetY = 150.0f;
float Cooldown_Size = 15.0f;
static CoolDownData getCoolDownFresh(uintptr_t show_entity) {
  CoolDownData result;

  auto cdComp = Read<uintptr_t>(show_entity + OFF_SE(m_ShowCoolDownComp));
  if (!cdComp)
    return result;

  auto dicCoolInfoPtr = Read<uintptr_t>(cdComp + OFF_SCC(_dicCD));
  if (!dicCoolInfoPtr)
    return result;

  monoDictionary<int, uintptr_t> dict{};
  dict.klass = reinterpret_cast<void *>(Read<uintptr_t>(dicCoolInfoPtr + 0x0));
  dict.obj = reinterpret_cast<void *>(Read<uintptr_t>(dicCoolInfoPtr + 0x8));
  dict.buckets =
          reinterpret_cast<int32_t *>(Read<uintptr_t>(dicCoolInfoPtr + 0x10));
  dict.arrayEntries = Read<uintptr_t>(dicCoolInfoPtr + 0x18);
  dict.count = Read<int>(dicCoolInfoPtr + 0x20);

  if (!dict.arrayEntries || dict.count <= 0)
    return result;

  
  int playerSpellId = Read<int>(show_entity + OFF_SE(SummonSkillId));

  
  
  if (playerSpellId <= 0 || playerSpellId > 100) {
    int heroId = Read<int>(show_entity + OFF_SE(HeroID));
    uint64_t playerGuid = Read<uint64_t>(show_entity + OFF_SE(m_uGuid));

    std::lock_guard<std::mutex> lock(g_RoomDataMutex);
    bool found = false;
    
    for (auto &p : g_CachedRoomData.TeamA) {
      if (p.HeroId == heroId || p.UserID == std::to_string(playerGuid)) {
        playerSpellId = p.SpellId;
        found = true;
        break;
      }
    }
    if (!found) {
      for (auto &p : g_CachedRoomData.TeamB) {
        if (p.HeroId == heroId || p.UserID == std::to_string(playerGuid)) {
          playerSpellId = p.SpellId;
          break;
        }
      }
    }
  }
  result.spellId = playerSpellId;

  auto items = get_mono_dictionary_vector<int, uintptr_t>(dict);
  for (const auto &entry : items) {
    auto skillID = entry.key;
    auto coolDownDataAddr = entry.value;
    if (skillID <= 0 || !coolDownDataAddr)
      continue;

    
    if (result.spellId == 0 && skillID >= 20000 && skillID < 22000 && skillID != 21010 && skillID != 2101 && skillID != 20010) {
      result.spellId = skillID;
    }

    const uint32_t startTime = Read<uint32_t>(coolDownDataAddr + OFF_CDD(uiStartTime));
    const uint32_t coolTime = Read<uint32_t>(coolDownDataAddr + OFF_CDD(uiCoolTime));
    if (coolTime == 0)
      continue;

    const auto currentTime = std::chrono::steady_clock::now();
    auto &timer = g_CoolDownTimers[coolDownDataAddr];
    if (timer.startTime != startTime ||
      timer.observedAt == std::chrono::steady_clock::time_point{}) {
      timer.startTime = startTime;
      timer.durationMs = coolTime;
      timer.observedAt = currentTime;
    }

    const auto elapsedMs = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - timer.observedAt).count());
    const uint32_t remainingTime = elapsedMs >= timer.durationMs
                                       ? 0
                                       : timer.durationMs - elapsedMs;
    if (remainingTime == 0)
      continue;

    const int timeLeftSec = static_cast<int>((remainingTime + 999u) / 1000u);

    auto fSkillID = skillID % 100;

    if (skillID < 20000) {
      
      switch (fSkillID) {
        case 10:
          if (timeLeftSec > result.skill1)
            result.skill1 = timeLeftSec;
              break;
        case 20:
          if (timeLeftSec > result.skill2)
            result.skill2 = timeLeftSec;
              break;
        case 30:
          if (timeLeftSec > result.skill3)
            result.skill3 = timeLeftSec;
              break;
        case 40:
          if (timeLeftSec > result.skill4)
            result.skill4 = timeLeftSec;
              break;
      }
    } else {
      
      if (skillID == 21010 || skillID == 2101 || skillID == 20010) {
        
        if (timeLeftSec > result.regen)
          result.regen = timeLeftSec;
      } else if (skillID == playerSpellId || (skillID / 10) == playerSpellId) {
        
        if (timeLeftSec > result.spell)
          result.spell = timeLeftSec;
      } else if (fSkillID == 10 || (skillID <= 20500 && (skillID % 10) == 0)) {
        
        if (timeLeftSec > result.spell)
          result.spell = timeLeftSec;
      }
    }
  }

  return result;
}
CoolDownData getCoolDown(uintptr_t show_entity) {
  static std::unordered_map<uintptr_t,
      std::pair<std::chrono::steady_clock::time_point, CoolDownData>> cdCache;
  auto now = std::chrono::steady_clock::now();
  auto it = cdCache.find(show_entity);
  if (it != cdCache.end() &&
      std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.first).count() < 500)
    return it->second.second;
  CoolDownData r = getCoolDownFresh(show_entity);
  cdCache[show_entity] = {now, r};
  if (cdCache.size() > 64) cdCache.clear();
  return r;
}
void DrawCircularIndicator(ImDrawList *Draw, ImVec2 pos, float size, int cdVal, const char* label, ImTextureID icon = nullptr) {
  bool isReady = (cdVal <= 0);
  ImU32 ringColor = isReady ? IM_COL32(0, 255, 150, 200) : IM_COL32(60, 60, 60, 200);

  
  Draw->AddCircleFilled(pos, size, IM_COL32(15, 15, 15, 230), 20);
  Draw->AddCircle(pos, size, ringColor, 20, 2.0f);

  if (icon) {
    Draw->AddImage(icon, ImVec2(pos.x - size + 2, pos.y - size + 2), ImVec2(pos.x + size - 2, pos.y + size - 2));
  } else {
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    Draw->AddText(ImVec2(pos.x - labelSize.x * 0.5f, pos.y - labelSize.y * 0.5f), IM_COL32(200, 200, 200, 255), label);
  }

  if (!isReady) {
    Draw->AddCircleFilled(pos, size - 1.0f, IM_COL32(0, 0, 0, 180), 20);
    std::string txt = std::to_string(cdVal);
    ImVec2 tSize = ImGui::CalcTextSize(txt.c_str());
    Draw->AddText(ImVec2(pos.x - tSize.x * 0.5f + 1, pos.y - tSize.y * 0.5f + 1), IM_COL32(0, 0, 0, 255), txt.c_str());
    Draw->AddText(ImVec2(pos.x - tSize.x * 0.5f, pos.y - tSize.y * 0.5f), IM_COL32(255, 255, 255, 255), txt.c_str());
  }
}

inline float DrawCooldownHorizontal(ImDrawList *Draw, float StartX, float BaselineY, const CoolDownData &cd, int HeroID, int hp, int maxHp, int level, bool showSkills, bool showHero, bool showSpell) {
  float size = Cooldown_Size;
  float hSize = g_HeroIconSize;
  float stepHero = (hSize > size ? hSize : size) * 2.3f;
  float spacing = size * 2.3f;
  float currentX = StartX + (hSize > size ? hSize : size);


  if (showHero) {
    DrawHeroIcon(Draw, ImVec2(currentX, BaselineY - hSize - 2.0f), HeroID, hp, maxHp, hSize, true, level);
    currentX += stepHero;
  }

  
  if (showSkills) {
    DrawCircularIndicator(Draw, ImVec2(currentX, BaselineY - size - 2.0f), size, cd.skill1, xorstr_("S1"), GetSkillIconTexture(HeroID, 1)); currentX += spacing;
    DrawCircularIndicator(Draw, ImVec2(currentX, BaselineY - size - 2.0f), size, cd.skill2, xorstr_("S2"), GetSkillIconTexture(HeroID, 2)); currentX += spacing;
    DrawCircularIndicator(Draw, ImVec2(currentX, BaselineY - size - 2.0f), size, cd.skill3, xorstr_("S3"), GetSkillIconTexture(HeroID, 3)); currentX += spacing;
  }

  
  if (showSkills && showSpell) {
    DrawCircularIndicator(Draw, ImVec2(currentX, BaselineY - size - 2.0f), size, cd.spell, xorstr_("SP"), GetSpellTexture(cd.spellId));
    currentX += spacing;
    if (cd.regen > 0) {
      DrawCircularIndicator(Draw, ImVec2(currentX, BaselineY - size - 2.0f), size, cd.regen, xorstr_("RG"), GetSkillIconTexture(0, 20010));
      currentX += spacing;
    }
  }

  return currentX - StartX - (spacing - size);
}



int CalculateRetriDamage(int m_Level) {
  // Patch 2.1.88+ (konfirmasi 2.2.16): 750 + 150 x Level, true damage.
  // Upgrade blessing (Ice/Flame/Bloody) TIDAK nambah damage ke creep/Lord.
  return 750 + (150 * m_Level);
}

extern uintptr_t Oneself;
// FAST PATH: jalan di game_worker tiap ~1ms, tidak nunggu FPS overlay.
void FastAutoRetri() {
  if (!autoRetribution || !is_attached || libbase == 0 || Oneself == 0)
    return;
  long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
  if (!a1) return;
  long a2 = ReadPtr(a1 + OFF_BM(PtrChain1));
  if (!a2) return;
  long a32 = ReadPtr((a2 << 1) >> 1);
  if (!a32) return;
  uintptr_t selfp = ReadPtr(a32 + OFF_BM(LocalPlayerShow));
  if (!selfp) return;
  long monListBase = ReadPtr(a32 + OFF_BM(ShowMonsters));
  if (!monListBase) return;
  long monsterPtr = ReadPtr(monListBase + OFF_BM(ListDataOffset)) + OFF_BM(ListArrayOffset);
  int stopMonster = Read<int>(monListBase + OFF_BM(ListCountOffset));
  if (stopMonster <= 0 || stopMonster > 64) return;
  int myLevel = Read<int>(selfp + OFF_SE(Level));
  if (myLevel <= 0 || myLevel > 30) myLevel = 12; // fallback: offset Level basi, tetap tembak garis menengah
  g_retriDbgLvl = myLevel;
  int retriDmg = CalculateRetriDamage(myLevel) + (int)retriEarlyMargin;
  Vector3 myPos;
  if (!vm_readv(selfp + OFF_SE(vCachePosition), &myPos, sizeof(myPos))) return;
  // Nearby-enemy-only: skip semua scan monster bila tak ada musuh di radius.
  if (retriNearbyOnly) {
    static auto lastNearCheck = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    static bool lastNearResult = true;
    auto nowQ = std::chrono::steady_clock::now();
    long nearAge = std::chrono::duration_cast<std::chrono::milliseconds>(nowQ - lastNearCheck).count();
    if (nearAge >= 200) {
      lastNearCheck = nowQ;
      lastNearResult = false;
      long pb = ReadPtr(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListDataOffset)) +
                OFF_BM(ListArrayOffset);
      int pc = Read<int>(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListCountOffset));
      if (pc > 0 && pc <= 40) {
        for (int i = 0; i < pc; i++) {
          auto e = ReadPtr(pb + (i << 3));
          if (!e || Read<bool>(e + OFF_SE(bDeath))) continue;
          if (Read<bool>(e + OFF_SE(bSameCampType))) continue;
          Vector3 ep;
          if (!vm_readv(e + OFF_SE(vCachePosition), &ep, sizeof(ep))) continue;
          if (Vector3::Distance(myPos, ep) <= retriNearbyRange) { lastNearResult = true; break; }
        }
      }
    }
    if (!lastNearResult) return;
  }
  static auto lastFastTap = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
  auto now = std::chrono::steady_clock::now();
  long sinceTap = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFastTap).count();
  if (sinceTap < retriSpamMs) return;
  g_retriDbgID = 0; g_retriDbgHP = -1; g_retriDbgDist = -1.0f; g_retriDbgDmg = retriDmg;
  for (int i = 0; i < stopMonster; i++) {
    auto Objaddr = ReadPtr(monsterPtr + (i << 3));
    if (!Objaddr) continue;
    if (Read<bool>(Objaddr + OFF_SE(bDeath))) continue;
    int mHeroID = Read<int>(Objaddr + OFF_SE(HeroID));
    bool anyTarget = AutoRetributionRed || AutoRetributionBlue || AutoRetributionLord ||
                     AutoRetributionTurtle || AutoRetributionCrab || AutoRetributionLito;
    bool isTarget = false;
    if (!anyTarget) isTarget = true; // failsafe: master ON tapi kosong = anggap semua
    else if (AutoRetributionLord && mHeroID == 2002) isTarget = true;
    else if (AutoRetributionTurtle && (mHeroID == 2003 || mHeroID == 2110)) isTarget = true;
    else if (AutoRetributionBlue && (mHeroID == 2005 || mHeroID == 2221)) isTarget = true;
    else if (AutoRetributionLito && mHeroID == 2056) isTarget = true;
    else if (AutoRetributionCrab && (mHeroID == 2223 || mHeroID == 2222)) isTarget = true;
    else if (AutoRetributionRed && (mHeroID == 2004 || mHeroID == 2220)) isTarget = true;
    if (!isTarget) continue;
    Vector3 monPos;
    if (!vm_readv(Objaddr + OFF_SE(vCachePosition), &monPos, sizeof(monPos))) continue;
    float dist = Vector3::Distance(myPos, monPos);
    if (dist > retriMaxRange) continue;
    int hp = Read<int>(Objaddr + OFF_SE(Hp));
    // Prediksi: kalau HP terjun >1500/dtk (dikeroyok), longgarkan garis bunuh 2x margin.
    static std::unordered_map<uintptr_t, std::pair<int, std::chrono::steady_clock::time_point>> hpHist;
    int killLine = retriDmg;
    auto hit = hpHist.find(Objaddr);
    if (hit != hpHist.end()) {
      long dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - hit->second.second).count();
      int drop = hit->second.first - hp;
      if (dt > 0 && dt < 400 && drop > 0 && (float)drop / (float)dt > 1.5f)
        killLine = retriDmg + (int)retriEarlyMargin;
    }
    hpHist[Objaddr] = {hp, now};
    if (hpHist.size() > 128) hpHist.clear();
    if (isTarget && dist < g_retriDbgDist - 0.001f || (g_retriDbgDist < 0 && isTarget)) {
      g_retriDbgID = mHeroID; g_retriDbgHP = hp; g_retriDbgDmg = killLine; g_retriDbgDist = dist;
    }
    if (hp > 0 && hp <= killLine) {
      Touch_Tap((int)retriTouchX, (int)retriTouchY);
      if (retriDoubleTap) Touch_Tap((int)retriTouchX, (int)retriTouchY);
      lastFastTap = now;
      break;
    }
  }
}
void FastAutoSpell() {
  if (!autoSpellExecute || !is_attached || libbase == 0 || Oneself == 0)
    return;
  long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
  if (!a1) return;
  long a2 = ReadPtr(a1 + OFF_BM(PtrChain1));
  if (!a2) return;
  long a32 = ReadPtr((a2 << 1) >> 1);
  if (!a32) return;
  uintptr_t selfp = ReadPtr(a32 + OFF_BM(LocalPlayerShow));
  if (!selfp) return;
  long playerBase = ReadPtr(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListDataOffset)) +
                    OFF_BM(ListArrayOffset);
  int playerCount = Read<int>(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListCountOffset));
  if (playerCount <= 0 || playerCount > 40) return;
  Vector3 myPos;
  if (!vm_readv(selfp + OFF_SE(vCachePosition), &myPos, sizeof(myPos))) return;
  static auto lastSpellTap = std::chrono::steady_clock::now() - std::chrono::milliseconds(5000);
  auto now = std::chrono::steady_clock::now();
  long sinceTap = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSpellTap).count();
  if (sinceTap < spellSpamMs) return;
  for (int i = 0; i < playerCount; i++) {
    auto Objaddr = ReadPtr(playerBase + (i << 3));
    if (!Objaddr) continue;
    if (Read<bool>(Objaddr + OFF_SE(bSameCampType))) continue;
    if (Read<bool>(Objaddr + OFF_SE(bDeath))) continue;
    int hp = Read<int>(Objaddr + OFF_SE(Hp));
    int maxHp = Read<int>(Objaddr + OFF_SE(HpMax));
    if (hp <= 0 || maxHp <= 0) continue;
    float pct = 100.0f * (float)hp / (float)maxHp;
    if (pct > spellExecPct) continue;
    Vector3 epos;
    if (!vm_readv(Objaddr + OFF_SE(vCachePosition), &epos, sizeof(epos))) continue;
    if (Vector3::Distance(myPos, epos) > spellRange) continue;
    Touch_Tap((int)spellX, (int)spellY);
    Touch_Tap((int)spellX, (int)spellY);
    lastSpellTap = now;
    break;
  }
}

void DrawNeedleLine(ImDrawList* draw, ImVec2 p1, ImVec2 p2, ImU32 col, float thickness) {
  ImVec2 dir = { p2.x - p1.x, p2.y - p1.y };
  float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
  if (length < 1.0f) return;
  dir.x /= length;
  dir.y /= length;

  
  ImVec2 norm = { -dir.y * thickness, dir.x * thickness };
  ImVec2 mid = { (p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f };

  ImVec2 pts[4];
  pts[0] = p1;                                 
  pts[1] = { mid.x + norm.x, mid.y + norm.y }; 
  pts[2] = p2;                                 
  pts[3] = { mid.x - norm.x, mid.y - norm.y }; 

  draw->AddConvexPolyFilled(pts, 4, col);
}

void DrawSwordDebug(ImDrawList *draw, uintptr_t selfAddr) {
  










































}

void UpdateAutoSwordLing(uintptr_t selfAddr) {
  if (!autoSwordLing || !is_attached || !selfAddr) {
    g_SwordChasing = false;
    return;
  }

  int myHeroID = Read<int>(selfAddr + OFF_SE(HeroID));
  if (myHeroID != 84) return;

  auto now = std::chrono::steady_clock::now();
  long long currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

  Vector3 myPos;
  vm_readv(selfAddr + OFF_SE(vCachePosition), &myPos, sizeof(myPos));

  
  static uintptr_t currentTargetAddr = 0;
  static long long lastActionTime = 0;
  static Vector3 lastTargetPos = {0,0,0};
  static bool g_SwordsDetected = false;

  
  if (currentTargetAddr != 0) {
    float distToTarget = Vector3::Distance(myPos, lastTargetPos);

    
    if (distToTarget < 0.6f || (currentTimeMs - lastActionTime > 500)) {
      currentTargetAddr = 0; 
      g_SwordChasing = false;
    } else {
      g_SwordChasing = true;
      return; 
    }
  }

  
  uintptr_t typeInfo = ReadPtr(libbase + OFF_BM(BasePtr));
  if (!typeInfo) return;
  uintptr_t staticFields = ReadPtr(typeInfo + OFF_BM(PtrChain1));
  if (!staticFields) return;
  uintptr_t hashSetPtr = ReadPtr(staticFields + OFF_BM(m_RunBullets));
  if (!hashSetPtr) return;
  uintptr_t entriesPtr = ReadPtr(hashSetPtr + 0x18);
  if (!entriesPtr) return;

  int capacity = Read<int>(entriesPtr + 0x18);
  uintptr_t bestBulletAddr = 0;
  Vector3 bestTargetPos = {0,0,0};
  float closestDist = lingSwordDashRange;

  for (int i = 0; i < capacity; i++) {
    uintptr_t entryAddr = entriesPtr + 0x20 + (i * 16);
    if (Read<int>(entryAddr) < 0) continue;
    uintptr_t bulletAddr = ReadPtr(entryAddr + 8);
    if (!bulletAddr) continue;

    if (Read<int>(bulletAddr + OFF_BUL(m_Id)) == lingSwordID) {
      Vector3 bPos = Read<Vector3>(bulletAddr + OFF_BUL(_bulletPos));
      if (bPos.X == 0 && bPos.Z == 0) continue;

      float d = Vector3::Distance(myPos, bPos);
      if (d < closestDist && d > 0.4f) {
        closestDist = d;
        bestTargetPos = bPos;
        bestBulletAddr = bulletAddr;
      }
    }
  }

  
  if (bestBulletAddr != 0) {
    g_SwordChasing = true;
    g_SwordsDetected = true; 
    float diffX = bestTargetPos.X - myPos.X;
    float diffZ = bestTargetPos.Z - myPos.Z;
    float angle = std::atan2(-diffZ, diffX) - 0.7854f;

    float moveX = lingSkill2X + std::cos(angle) * 300.0f;
    float moveY = lingSkill2Y + std::sin(angle) * 300.0f;

    
    Touch_Down(lingSkill2X, lingSkill2Y);
    usleep(20000);
    Touch_Move(moveX, moveY);
    usleep(45000); 

    
    Touch_Up();

    currentTargetAddr = bestBulletAddr;
    lastTargetPos = bestTargetPos;
    lastActionTime = currentTimeMs;
  } else if (g_SwordsDetected) {
    
    long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
    long a2 = ReadPtr(a1 + OFF_BM(PtrChain1));
    long a32 = ReadPtr(a2);

    long playerList = ReadPtr(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListDataOffset)) + OFF_BM(ListArrayOffset);
    uint playerCount = Read<uint>(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListCountOffset));

    uintptr_t nearestEnemy = 0;
    float minEnemyDist = 10.0f; 
    Vector3 enemyPos = {0,0,0};

    for (int i = 0; i < playerCount; i++) {
      uintptr_t obj = ReadPtr(playerList + (i << 3));
      if (!obj || Read<bool>(obj + OFF_SE(bDeath)) || Read<bool>(obj + OFF_SE(bSameCampType))) continue;

      Vector3 pos;
      vm_readv(obj + OFF_SE(vCachePosition), &pos, sizeof(pos));
      float dist = Vector3::Distance(myPos, pos);
      if (dist < minEnemyDist) {
        minEnemyDist = dist;
        nearestEnemy = obj;
        enemyPos = pos;
      }
    }

    if (nearestEnemy != 0) {
      float diffX = enemyPos.X - myPos.X;
      float diffZ = enemyPos.Z - myPos.Z;
      float angle = std::atan2(-diffZ, diffX) - 0.7854f;

      float moveX = lingSkill2X + std::cos(angle) * 300.0f;
      float moveY = lingSkill2Y + std::sin(angle) * 300.0f;

      Touch_Down(lingSkill2X, lingSkill2Y);
      usleep(20000);
      Touch_Move(moveX, moveY);
      usleep(45000);
      Touch_Up();

      lastActionTime = currentTimeMs;
    }
    g_SwordsDetected = false; 
    g_SwordChasing = false;
  } else {
    g_SwordChasing = false;
  }
}

void DrawMonster(ImDrawList *Draw) {
  if (!is_attached || libbase == 0)
    return;

  using namespace Prediction;
  float currentTime = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
  CleanupStale(currentTime);
  if (abs_ScreenX < abs_ScreenY)
    return;
  float lineSize = abs_ScreenY / 432;
  long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
  if (!a1) return;
  long a2 = ReadPtr((a1 + ((0x100 | OFF_BM(PtrChain1)) & 0xFF)));
  if (!a2) return;
  long a32 = ReadPtr((a2 << 1) >> 1);
  if (!a32) return;
  long selfp = ReadPtr(a32 + OFF_BM(LocalPlayerShow));
  if (!selfp) return;

  auto main_cam = GetMainCamera();
  if (!main_cam)
    return;
  auto camera = Read<uintptr_t>(main_cam + OFF_CAM(SmoothFollow));
  if (!camera)
    return;

  if (EnableDrone) {
    Write<float>(camera + OFF_CAM(v2), FieldView);
  }
  auto ViewMatrix = Read<Camera>(camera + OFF_CAM(ViewMatrix));
  _vMatrix = ViewMatrix.projectionMatrix * ViewMatrix.worldToCameraMatrix;

  Vector3 loc_worldPos;
  vm_readv(selfp + OFF_SE(vCachePosition), &loc_worldPos, sizeof(loc_worldPos));
  Vector2 loc_posSc;
  bool loc_isInside = WorldToScreen(loc_worldPos, &loc_posSc);

  long player =
          ReadPtr(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListDataOffset)) +
          OFF_BM(ListArrayOffset);
  uint stop_player =
          Read<uint>(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListCountOffset));

  
  long monster =
          ReadPtr(ReadPtr(a32 + OFF_BM(ShowMonsters)) + OFF_BM(ListDataOffset)) +
          OFF_BM(ListArrayOffset);
  uint stop_monster =
          Read<uint>(ReadPtr(a32 + OFF_BM(ShowMonsters)) + OFF_BM(ListCountOffset));
  // (dihapus, diganti FastAutoRetri/FastAutoSpell di game_worker 5ms)

  for (int i = 0; i < stop_player; i++) {
    auto Objaddr = ReadPtr(player + ((i << 3) / 1));
    if ((Objaddr ^ 0x0) == 0x0)
      continue;
    auto is_team = Read<bool>(Objaddr + OFF_SE(bSameCampType));
    if (is_team)
      continue;
    auto HeroID = Read<int>(Objaddr + OFF_SE(HeroID));
    auto death = Read<bool>(Objaddr + OFF_SE(bDeath));
    if (death) {
      Prediction::InvalidatePrediction(Objaddr);
      continue;
    }
    int Health = Read<int>(Objaddr + OFF_SE(Hp));
    if (Health <= 0)
      continue;
    int maxHealth = Read<int>(Objaddr + OFF_SE(HpMax));
    if (maxHealth <= 0)
      continue;
    int level = Read<int>(Objaddr + OFF_SE(Level));
    Vector3 Z;
    vm_readv(selfp + OFF_SE(vCachePosition), &Z, sizeof(Z));
    Vector3 D;
    vm_readv(Objaddr + OFF_SE(vCachePosition), &D, sizeof(D));
    Vector2 en_posSc;
    bool isInside = WorldToScreen(D, &en_posSc);
    Vector2 loc_posSc = {0, 0};
    bool loc_isInside = WorldToScreen(Z, &loc_posSc);

    if (!isInside && (en_posSc.X < 0 || en_posSc.X > abs_ScreenX ||
                      en_posSc.Y < 0 || en_posSc.Y > abs_ScreenY)) {
      
      
      
    }

    
    
    if (!isInside)
      continue;

    bool isOutScreen;
    float IconSize = 35.0f;
    Vector2 HeroPos = {en_posSc.X, en_posSc.Y};
    Vector2 Res;
    if (HeroPos.X < 0 || HeroPos.X > abs_ScreenX || HeroPos.Y < 0 ||
        HeroPos.Y > abs_ScreenY) {
      isOutScreen = true;
      IconSize = 42.0f;
      FindPoint(HeroPos, Res, abs_ScreenX, abs_ScreenY, (IconSize / 3));
    } else {
      isOutScreen = false;
      Res = HeroPos;
    }
    auto Distance = Vector3::Distance(Z, D);

    
    if (drawMLine && loc_isInside && HeroID < 1000) {
      DrawNeedleLine(ImGui::GetForegroundDrawList(),
                     ImVec2(loc_posSc.X, loc_posSc.Y),
                     ImVec2(en_posSc.X, en_posSc.Y),
                     IM_COL32(255, 0, 0, 150), lineSize * 0.4f);
    }

    float finalY = Res.Y + Avatar_OffsetY;

    
    bool showHeroContent = (iconhero || ESP_Player_Cooldown);
    if (showHeroContent && !isOutScreen && HeroID < 1000) {
      float offsetSide = 80.0f;
      float offsetUp = Avatar_OffsetY; 

      ImVec2 p0 = ImVec2(Res.X, Res.Y); 
      
      ImVec2 p1 = ImVec2(p0.x + offsetSide, p0.y - offsetUp - 30.0f);

      
      auto cd = getCoolDown(Objaddr);
      float totalWidth = DrawCooldownHorizontal(Draw, p1.x, p1.y, cd, HeroID, Health, maxHealth, level,
                                                ESP_Player_Cooldown, 
                                                iconhero,           
                                                Cooldown_ShowSpell);

      ImVec2 p2 = ImVec2(p1.x + totalWidth, p1.y); 

      Draw->AddLine(p0, p1, IM_COL32(255, 255, 255, 200), 1.5f);
      Draw->AddLine(p1, p2, IM_COL32(255, 255, 255, 200), 1.5f);
      Draw->AddCircleFilled(p0, 3.0f, IM_COL32(0, 255, 0, 255), 10);
    }
    if (drawMPrediction) {
      DrawPrediction(Draw, HeroPos, D, Objaddr);
    }
  }
  
  for (int i = 0; i < stop_monster; i++) {
    auto Objaddr = ReadPtr(monster + ((i << 3) / 1));
    if ((Objaddr ^ 0x0) == 0x0)
      continue;
    auto is_team = Read<bool>(Objaddr + OFF_SE(bSameCampType));
    if (is_team)
      continue;
    auto mHeroID = Read<int>(Objaddr + OFF_SE(HeroID));
    auto type = Read<int>(Objaddr + OFF_SE(iType));
    auto death = Read<bool>(Objaddr + OFF_SE(bDeath));
    if (death) {
      Prediction::InvalidatePrediction(Objaddr);
      continue;
    }
    int Health = Read<int>(Objaddr + OFF_SE(Hp));
    if (Health <= 0)
      continue;
    int maxHealth = Read<int>(Objaddr + OFF_SE(HpMax));
    if (maxHealth <= 0)
      continue;
    Vector3 Dm;
    vm_readv(Objaddr + OFF_SE(vCachePosition), &Dm, sizeof(Dm));
    
    Vector2 mon_posSc;
    bool mon_isInside = WorldToScreen(Dm, &mon_posSc);
    Vector2 MonPos = {mon_posSc.X, mon_posSc.Y};

    
    if (!mon_isInside)
      continue;

    
    if (type == 5 && drawObjectiveAlert) {
      float objPct = maxHealth > 0 ? 100.0f * (float)Health / (float)maxHealth : 100.0f;
      bool objLow = objPct <= objectiveLowPct;
      if (mHeroID == 2002 && Health < maxHealth) {
        DrawGlobalWarning(Draw, objLow ? xorstr_("LORD LOW!") : xorstr_("LORD ATTACKED!"), (float)Health,
                          (float)maxHealth);
      }
      if ((mHeroID == 2003 || mHeroID == 2110) && Health < maxHealth) {
        DrawGlobalWarning(Draw, objLow ? xorstr_("TURTLE LOW!") : xorstr_("TURTLE ATTACKED!"), (float)Health,
                          (float)maxHealth);
      }
    }

    if (MonPos.X < 0 || MonPos.X > abs_ScreenX || MonPos.Y < 0 ||
        MonPos.Y > abs_ScreenY)
      continue;

    if (type == 1) {
      float radius = 10.0f;
      ImU32 minionColor = IM_COL32(255, 0, 0, 200);
      Draw->AddCircleFilled(ImVec2(MonPos.X, MonPos.Y), radius, minionColor,
                            12);
      Draw->AddCircle(ImVec2(MonPos.X, MonPos.Y), radius,
                      IM_COL32(0, 0, 0, 255), 12, 1.0f);
    }

    
    if (drawMonsterName && (type == 5 || type == 2 || type == 1)) {
      float offX = 70.0f;
      float offY = Avatar_OffsetY;

      ImVec2 p0 = ImVec2(MonPos.X, MonPos.Y);
      ImVec2 p1 = ImVec2(p0.x + offX, p0.y - offY - 30.0f);

      
      float monsterRadius = (type == 1) ? 1.0f : 2.0f; 
      const int segs = 32;
      Vector2 lastP; bool firstP = true;
      ImU32 circleCol = IM_COL32(0, 255, 150, 150); 
      if (mHeroID == 2002 || mHeroID == 2003) circleCol = IM_COL32(255, 200, 0, 180); 
      else if (type == 1) circleCol = IM_COL32(255, 0, 0, 150); 

      for (int j = 0; j <= segs; j++) {
        float ang = (float)j / (float)segs * 2.0f * M_PI;
        Vector3 wPt = { Dm.X + cosf(ang) * monsterRadius, Dm.Y + 0.1f, Dm.Z + sinf(ang) * monsterRadius };
        Vector2 sPt;
        if (WorldToScreen(wPt, &sPt)) {
          if (!firstP) Draw->AddLine(ImVec2(lastP.X, lastP.Y), ImVec2(sPt.X, sPt.Y), circleCol, 2.0f);
          lastP = sPt; firstP = false;
        } else { firstP = true; }
      }

      
      std::string mName = (type == 1) ? xorstr_("Minion") : MonsterToString(mHeroID);
      char hpBuf[32]; snprintf(hpBuf, sizeof(hpBuf), xorstr_("%d"), (int)Health);
      float monIconSize = g_MonsterIconSize;

      
      ImVec2 nSize = ImGui::CalcTextSize(mName.c_str());
      ImVec2 hSize = ImGui::CalcTextSize(hpBuf);
      float padding = 10.0f;
      float totalW = nSize.x + padding + (monIconSize * 2) + hSize.x + padding;

      ImVec2 p2 = ImVec2(p1.x + totalW, p1.y); 

      
      Draw->AddLine(p0, p1, IM_COL32(255, 255, 255, 180), 1.2f);
      Draw->AddLine(p1, p2, IM_COL32(255, 255, 255, 180), 1.2f);
      Draw->AddCircleFilled(p0, 3.0f, IM_COL32(0, 255, 0, 255), 10);

      
      float curX = p1.x;

      
      if (type != 1) {
        DrawHeroIcon(Draw, ImVec2(curX + monIconSize, p1.y - monIconSize - 2.0f), mHeroID, Health, maxHealth, monIconSize, false);
        curX += monIconSize * 2 + padding;
      }

      
      if (!mName.empty()) {
        Draw->AddText(ImVec2(curX, p1.y - nSize.y - 2.0f), IM_COL32(255, 255, 255, 255), mName.c_str());
        curX += nSize.x + padding;
      }

      
      Draw->AddText(ImVec2(curX, p1.y - hSize.y - 2.0f), IM_COL32(0, 255, 0, 230), hpBuf);
    }
  }
}

struct MonsterData {
    uintptr_t address;
    Vector3 position;
    float distance;
    int health;
    int maxHP;
    bool isDead;
    bool isVisible;
    bool isValid;
    char name[100];
};



MonsterData monster[20];
int MonsterCount = 0;
uintptr_t Oneself;
void MonsterRetribution() {
  long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
  if (!a1) return;
  long a2 = ReadPtr((a1 + ((0x100 | OFF_BM(PtrChain1)) & 0xFF)));
  if (!a2) return;
  long a32 = ReadPtr((a2 << 1) >> 1);
  if (!a32) return;

  Oneself = ReadPtr(a32 + OFF_BM(LocalPlayerShow));
  if (!Oneself) return;

  Vector3 MyPosition;
  vm_readv(Oneself + OFF_SE(vCachePosition), &MyPosition, sizeof(MyPosition));
  MonsterCount = 0;
  uintptr_t Showmonster = ReadPtr(a32 + OFF_BM(ShowMonsters));
  if (Showmonster != 0) {
    int monsterCount = Read<int>(Showmonster + OFF_BM(ListCountOffset));
    uintptr_t monsterDataPtr = ReadPtr(Showmonster + OFF_BM(ListDataOffset));
    if (monsterCount >= 0 && monsterCount <= 100 && monsterDataPtr != 0) {
      uintptr_t monsterDataArray = monsterDataPtr + OFF_BM(ListArrayOffset);
      int monsterfound = 0;
      for (int i = 0; i < monsterCount && monsterfound < 20; i++) {
        uintptr_t currentMonsterPtr = ReadPtr(monsterDataArray + (i * 8));
        if (currentMonsterPtr == 0)
          continue;
        int monsterID = Read<int>(currentMonsterPtr + OFF_SE(HeroID));
        int monsterHP = Read<int>(currentMonsterPtr + OFF_SE(Hp));
        int monsterMaxHP = Read<int>(currentMonsterPtr + OFF_SE(HpMax));
        Vector3 monsterPos =
                Read<Vector3>(currentMonsterPtr + OFF_SE(vCachePosition));
        uint8_t deadFlag = Read<uint8_t>(currentMonsterPtr + OFF_SE(bDeath));
        bool mDead = (deadFlag != 0);
        int mType = Read<int>(currentMonsterPtr + OFF_SE(iType));
        if (mType != 2 && mType != 5)
          continue;
        std::string mName = MonsterToString(monsterID);
        if (mName.empty()) {
          if (monsterID == 2002)
            mName = xorstr_("Lord");
          else if (monsterID == 2003 || monsterID == 2110)
            mName = xorstr_("Turtle");
          else if (monsterID == 2004 || monsterID == 2220)
            mName = xorstr_("Red Buff");
          else if (monsterID == 2005 || monsterID == 2221)
            mName = xorstr_("Blue Buff");
          else if (monsterID == 2222 || monsterID == 2223)
            mName = xorstr_("Crab");
          else if (monsterID == 2056)
            mName = xorstr_("Lito");
            
          else
            mName = xorstr_("Monster");
        }
        monster[monsterfound].address = currentMonsterPtr;
        monster[monsterfound].position = monsterPos;
        monster[monsterfound].distance =
                Vector3::Distance(MyPosition, monsterPos);
        monster[monsterfound].health = monsterHP;
        monster[monsterfound].maxHP = monsterMaxHP;
        monster[monsterfound].isDead = mDead;
        monster[monsterfound].isVisible = true;
        monster[monsterfound].isValid = true;
        strncpy(monster[monsterfound].name, mName.c_str(),
                sizeof(monster[monsterfound].name) - 1);
        monster[monsterfound].name[sizeof(monster[monsterfound].name) - 1] =
                '\0';
        monsterfound++;
      }
      MonsterCount = monsterfound;
    }
  }
}

float g_MinimapScale = 74.11f;
float g_Res0_MultX = 1.0f;
float g_Res0_MultY = 1.0f;
float g_Res1_OffsetX = 0.0f;
float g_Res1_OffsetY = 0.0f;
int g_MinimapHeroSize = 38;
int g_MinimapMonsterSize = 30;
Vector2 WorldToMinimap(Vector3 HeroPosition) {
  int CampType = Read<int>(Oneself + OFF_SP(m_EntityCampType));
  float angle = (CampType == 2 ? 314.60f : 134.76f) * 0.017453292519943295;
  float angleCos = std::cos(angle);
  float angleSin = std::sin(angle);
  Vector2 Res0;
  float worldX = -HeroPosition.X;
  float worldZ = HeroPosition.Y;
  Res0.X =
          ((angleCos * worldX - angleSin * worldZ) / g_MinimapScale) * g_Res0_MultX;
  Res0.Y =
          ((angleSin * worldX + angleCos * worldZ) / g_MinimapScale) * g_Res0_MultY;
  Vector2 Res1;
  Res1.X = (Res0.X * MinimapSize) + MinimapPos + (MinimapSize / 2.0f) +
           g_Res1_OffsetX;
  Res1.Y = (Res0.Y * MinimapSize) + MinimapPosY + (MinimapSize / 2.0f) +
           g_Res1_OffsetY;
  return Res1;
}
struct MmDot {
  float x, y;
  int id, hp, maxHp, level, mType, kind; // kind: 0 hero, 1 monster, 2 minion
};

void DrawMinimapESP(ImDrawList *draw) {
  if (!MinimapIcon)
    return;
  if (!is_attached || libbase == 0)
    return;
  // Scan berat 10Hz, gambar tiap frame dari cache (tidak blink, hemat CPU).
  static std::vector<MmDot> mmCache;
  static auto lastMmScan = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  auto nowMm = std::chrono::steady_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(nowMm - lastMmScan).count() >= 100) {
    lastMmScan = nowMm;
    mmCache.clear();
    long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
    if (a1) {
      long a2 = ReadPtr(a1 + OFF_BM(PtrChain1));
      if (a2) {
        long a32 = ReadPtr(a2);
        if (a32) {
          auto logicMgr = GetLogicBattleManagerInstance();
          uint now = logicMgr ? Read<uint>(logicMgr + OFF_BLC(m_uiFrameTime)) : 0;
          (void)now;
          long showList = ReadPtr(a32 + OFF_BM(ShowPlayers));
          if (showList) {
            long playerList = ReadPtr(showList + OFF_BM(ListDataOffset));
            if (playerList) {
              playerList += OFF_BM(ListArrayOffset);
              uint playerCount = Read<uint>(showList + OFF_BM(ListCountOffset));
              for (int i = 0; i < playerCount; i++) {
                long Objaddr = ReadPtr(playerList + (i << 3));
                if (!Objaddr) continue;
                if (Read<bool>(Objaddr + OFF_SE(bSameCampType))) continue;
                if (Read<bool>(Objaddr + OFF_SE(bDeath))) continue;
                Vector3A pos;
                vm_readv(Objaddr + OFF_SE(vCachePosition), &pos, sizeof(pos));
                if (pos.X == 0 && pos.Y == 0 && pos.Z == 0) continue;
                Vector2 mp = WorldToMinimap({pos.X, pos.Y, pos.Z});
                MmDot dt{mp.X, mp.Y, Read<int>(Objaddr + OFF_SE(HeroID)),
                         Read<int>(Objaddr + OFF_SE(Hp)), Read<int>(Objaddr + OFF_SE(HpMax)),
                         Read<int>(Objaddr + OFF_SE(Level)), 0, 0};
                mmCache.push_back(dt);
              }
            }
          }
          long showMonster = ReadPtr(a32 + OFF_BM(ShowMonsters));
          if (showMonster) {
            long monsterList = ReadPtr(showMonster + OFF_BM(ListDataOffset));
            if (monsterList) {
              monsterList += OFF_BM(ListArrayOffset);
              uint monsterCount = Read<uint>(showMonster + OFF_BM(ListCountOffset));
              for (int i = 0; i < monsterCount; i++) {
                long Objaddr = ReadPtr(monsterList + (i << 3));
                if (!Objaddr) continue;
                int mType = Read<int>(Objaddr + OFF_SE(iType));
                int mHeroID = Read<int>(Objaddr + OFF_SE(HeroID));
                if (Read<bool>(Objaddr + OFF_SE(bDeath))) continue;
                Vector3A pos;
                vm_readv(Objaddr + OFF_SE(vCachePosition), &pos, sizeof(pos));
                if (pos.X == 0 && pos.Y == 0 && pos.Z == 0) continue;
                Vector2 mp = WorldToMinimap({pos.X, pos.Y, pos.Z});
                if (mType == 1) {
                  if (!drawMinionMinimap) continue;
                  if (Read<bool>(Objaddr + OFF_SE(bSameCampType))) continue;
                  MmDot dt{mp.X, mp.Y, 0, 0, 0, 0, 1, 2};
                  mmCache.push_back(dt);
                  continue;
                }
                if (mType == 2 || mType == 5) {
                  bool isObjective = (mHeroID == 2002 || mHeroID == 2003 || mHeroID == 2110);
                  if (isObjective) {
                    if (!MinimapIconLordTurtle) continue;
                  } else {
                    if (!drawMonsterMinimap) continue;
                    if (!(bMonster(mHeroID) || MinimapIconBuff)) continue;
                  }
                  MmDot dt{mp.X, mp.Y, mHeroID, Read<int>(Objaddr + OFF_SE(Hp)),
                           Read<int>(Objaddr + OFF_SE(HpMax)), 0, mType, 1};
                  mmCache.push_back(dt);
                }
              }
            }
          }
        }
      }
    }
  }
  for (auto &dt : mmCache) {
    if (dt.kind == 0) {
      DrawHeroIcon(draw, ImVec2(dt.x, dt.y), dt.id, dt.hp, dt.maxHp,
                   g_MinimapHeroSize / 2.0f, true, dt.level, 0);
    } else if (dt.kind == 1) {
      DrawHeroIcon(draw, ImVec2(dt.x, dt.y), dt.id, dt.hp, dt.maxHp,
                   g_MinimapMonsterSize / 2.0f, false, 0, 0);
    } else {
      draw->AddCircleFilled(ImVec2(dt.x, dt.y), minionMinimapSize, IM_COL32(255, 0, 0, 200), 8);
      draw->AddCircle(ImVec2(dt.x, dt.y), minionMinimapSize, IM_COL32(0, 0, 0, 255), 8, 1.0f);
    }
  }
  if (!HideLine) {
    draw->AddRect(ImVec2(MinimapPos, MinimapPosY),
                  ImVec2(MinimapPos + MinimapSize, MinimapPosY + MinimapSize),
                  IM_COL32(255, 255, 255, 255));
  }
}
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <vector>

namespace AutoAim {
    inline bool enabled = false;
    inline float fovRadius = 150.0f;
    inline float smoothFactor = 0.15f;
    inline int priorityMode = 0;
    inline bool showVisuals = false;
    inline float joyCenterX = 0.0f;
    inline float joyCenterY = 0.0f;
    inline float joyRadius = 180.0f;
    inline float joyDeadZone = 15.0f;
    inline bool joyVisible = false;
    inline bool joyDragging = false;
    inline bool joyActive = false;
    inline float joyAngle = 0.0f;
    inline float joyMagnitude = 0.0f;
    inline bool touchDownSent = false;
    inline float lastTouchX = 0.0f;
    inline float lastTouchY = 0.0f;
    inline bool wasTargeting = false;
    inline bool userTouchActive = false;
    inline bool autoAimPaused = false;

    inline Vector2 targetScreenPos;
    inline bool hasTarget = false;
    inline uintptr_t currentTargetAddr = 0;
    struct TargetInfo {
        uintptr_t address;
        Vector3 worldPos;
        Vector2 screenPos;
        float distanceToPlayer;
        float distanceToCenter;
        int health;
        int maxHP;
        int heroId;
        bool isPlayer;
        bool isValid;
        TargetInfo()
                : address(0), distanceToPlayer(0), distanceToCenter(0), health(0),
                  maxHP(0), heroId(0), isPlayer(false), isValid(false) {}
        float getHPPercent() const {
          return maxHP > 0 ? (float)health / (float)maxHP : 0.0f;
        }
    };
} 
static float CalcScreenDist(float x1, float y1, float x2, float y2) {
  const float dx = x2 - x1;
  const float dy = y2 - y1;
  return std::sqrt(dx * dx + dy * dy);
}
static float LerpAngle(float from, float to, float t) {
  float diff = to - from;
  while (diff > M_PI)
    diff -= 2.0f * M_PI;
  while (diff < -M_PI)
    diff += 2.0f * M_PI;
  return from + diff * t;
}
namespace MobaTouch {
    static void SendTouchUp() {
      if (!AutoAim::touchDownSent)
        return;
      Touch_Up();
      AutoAim::touchDownSent = false;
      AutoAim::wasTargeting = false;
    }
    static void SendTouchDown(float x, float y) {
     
      if (AutoAim::touchDownSent)
        return;
      AutoAim::lastTouchX = x;
      AutoAim::lastTouchY = y;
      Touch_Down(x, y);
      AutoAim::touchDownSent = true;
      AutoAim::wasTargeting = true;
    }
    static void SendTouchMove(float x, float y) {
      if (!AutoAim::touchDownSent) {
        
        SendTouchDown(AutoAim::joyCenterX, AutoAim::joyCenterY);
        usleep(5000); 
        Touch_Move(x, y);
        return;
      }
      AutoAim::wasTargeting = true;
      AutoAim::lastTouchX = x;
      AutoAim::lastTouchY = y;
      Touch_Move(x, y);
    }
} 
namespace AutoAim {
    static bool CollectTargets(AutoAim::TargetInfo *outTargets, size_t maxTargets,
                               uintptr_t selfAddr, long a32, long libbase) {
      if (!selfAddr)
        return false;
      Vector3 myPos;
      vm_readv(selfAddr + OFF_SE(vCachePosition), &myPos, sizeof(myPos));
      const float screenCX = (float)abs_ScreenX * 0.5f;
      const float screenCY = (float)abs_ScreenY * 0.5f;
      size_t idx = 0;
      const long playerBase =
              ReadPtr(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListDataOffset)) +
              OFF_BM(ListArrayOffset);
      const uint playerCount =
              Read<uint>(ReadPtr(a32 + OFF_BM(ShowPlayers)) + OFF_BM(ListCountOffset));
      for (uint i = 0; i < playerCount && idx < maxTargets; ++i) {
        const uintptr_t objAddr = ReadPtr(playerBase + (i << 3));
        if (!objAddr)
          continue;
        if (Read<bool>(objAddr + OFF_SE(bSameCampType)))
          continue;
        if (Read<bool>(objAddr + OFF_SE(bDeath)))
          continue;
        const int hp = Read<int>(objAddr + OFF_SE(Hp));
        const int maxHp = Read<int>(objAddr + OFF_SE(HpMax));
        if (hp <= 0 || maxHp <= 0)
          continue;
        Vector3 entityPos;
        vm_readv(objAddr + OFF_SE(vCachePosition), &entityPos, sizeof(entityPos));
        Vector2 screenPos;
        if (!WorldToScreen(*(::Vector3 *)&entityPos, (::Vector2 *)&screenPos))
          continue;
        if (screenPos.X < 0 || screenPos.X > abs_ScreenX || screenPos.Y < 0 ||
            screenPos.Y > abs_ScreenY)
          continue;
        outTargets[idx].address = objAddr;
        outTargets[idx].worldPos = entityPos;
        outTargets[idx].screenPos = screenPos;
        outTargets[idx].distanceToPlayer = Vector3::Distance(myPos, entityPos);
        outTargets[idx].distanceToCenter =
                CalcScreenDist(screenCX, screenCY, screenPos.X, screenPos.Y);
        outTargets[idx].health = hp;
        outTargets[idx].maxHP = maxHp;
        outTargets[idx].isPlayer = true;
        outTargets[idx].isValid = true;
        ++idx;
      }
      const long monsterBase =
              ReadPtr(ReadPtr(a32 + OFF_BM(ShowMonsters)) + OFF_BM(ListDataOffset)) +
              OFF_BM(ListArrayOffset);
      const uint monsterCount =
              Read<uint>(ReadPtr(a32 + OFF_BM(ShowMonsters)) + OFF_BM(ListCountOffset));
      for (uint i = 0; i < monsterCount && idx < maxTargets; ++i) {
        const uintptr_t objAddr = ReadPtr(monsterBase + (i << 3));
        if (!objAddr)
          continue;
        const int mType = Read<int>(objAddr + OFF_SE(iType));
        if (mType != 2 && mType != 5)
          continue;
        const int mHeroID = Read<int>(objAddr + OFF_SE(HeroID));
        if (!bMonster(mHeroID) && mHeroID != 2002 && mHeroID != 2003)
          continue;
        if (Read<bool>(objAddr + OFF_SE(bSameCampType)))
          continue;
        if (Read<bool>(objAddr + OFF_SE(bDeath)))
          continue;
        const int hp = Read<int>(objAddr + OFF_SE(Hp));
        const int maxHp = Read<int>(objAddr + OFF_SE(HpMax));
        if (hp <= 0 || maxHp <= 0)
          continue;
        Vector3 entityPos;
        vm_readv(objAddr + OFF_SE(vCachePosition), &entityPos, sizeof(entityPos));
        Vector2 screenPos;
        if (!WorldToScreen(*(::Vector3 *)&entityPos, (::Vector2 *)&screenPos))
          continue;

        
        if (screenPos.X < 0 || screenPos.X > abs_ScreenX || screenPos.Y < 0 ||
            screenPos.Y > abs_ScreenY)
          continue;

        outTargets[idx].address = objAddr;
        outTargets[idx].worldPos = entityPos;
        outTargets[idx].screenPos = screenPos;
        outTargets[idx].distanceToPlayer = Vector3::Distance(myPos, entityPos);
        outTargets[idx].distanceToCenter =
                CalcScreenDist(screenCX, screenCY, screenPos.X, screenPos.Y);
        outTargets[idx].health = hp;
        outTargets[idx].maxHP = maxHp;
        outTargets[idx].heroId = mHeroID;
        outTargets[idx].isPlayer = false;
        outTargets[idx].isValid = true;
        ++idx;
      }
      return idx > 0;
    }
    static AutoAim::TargetInfo *SelectBestTarget(AutoAim::TargetInfo *candidates,
                                                 size_t count) {
      AutoAim::TargetInfo *best = nullptr;
      for (size_t i = 0; i < count; ++i) {
        if (!candidates[i].isValid)
          continue;

        
        if (candidates[i].distanceToCenter > fovRadius)
          continue;

        if (!best) {
          best = &candidates[i];
          continue;
        }

        switch (priorityMode) {
          case 0: 
            if (candidates[i].distanceToCenter < best->distanceToCenter)
              best = &candidates[i];
                break;
          case 1: 
            if (candidates[i].getHPPercent() < best->getHPPercent())
              best = &candidates[i];
                break;
          case 2: 
            if (candidates[i].isPlayer && !best->isPlayer)
              best = &candidates[i];
            else if (candidates[i].isPlayer == best->isPlayer &&
                     candidates[i].distanceToCenter < best->distanceToCenter)
              best = &candidates[i];
                break;
        }
      }
      return best;
    }
} 

void UpdateGusionCombo(uintptr_t selfAddr, long a32, long libbase) {
  if (!autoComboGusion || !is_attached || !selfAddr) {
    gusionState = 0;
    return;
  }

  int myHeroID = Read<int>(selfAddr + OFF_SE(HeroID));
  if (myHeroID != 56) return;

  auto now = std::chrono::steady_clock::now();
  long long currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  static long long lastActionTime = 0;

  
  if (currentTimeMs - lastActionTime < gusionComboDelay) return;

  Vector3 myPos;
  vm_readv(selfAddr + OFF_SE(vCachePosition), &myPos, sizeof(myPos));

  
  bool markFound = false;
  Vector3 markPos = {0,0,0};
  uintptr_t typeInfo = ReadPtr(libbase + OFF_BM(BasePtr));
  if (typeInfo) {
    uintptr_t staticFields = ReadPtr(typeInfo + OFF_BM(PtrChain1));
    uintptr_t hashSetPtr = ReadPtr(staticFields + OFF_BM(m_RunBullets));
    if (hashSetPtr) {
      uintptr_t entriesPtr = ReadPtr(hashSetPtr + 0x18);
      if (entriesPtr) {
        int capacity = Read<int>(entriesPtr + 0x18);
        for (int i = 0; i < capacity; i++) {
          uintptr_t entryAddr = entriesPtr + 0x20 + (i * 16);
          if (Read<int>(entryAddr) < 0) continue;
          uintptr_t bulletAddr = ReadPtr(entryAddr + 8);
          if (bulletAddr && Read<int>(bulletAddr + OFF_BUL(m_Id)) == gusionMarkID) {
            markPos = Read<Vector3>(bulletAddr + OFF_BUL(_bulletPos));
            markFound = true;
            break;
          }
        }
      }
    }
  }

  
  if (gusionState == 0) { 
    if (markFound) {
      
      Touch_Tap(gusionSkill2X, gusionSkill2Y);
      usleep(gusionComboDelay * 800);
      Touch_Tap(gusionSkill1X, gusionSkill1Y);
      gusionState = 1;
      lastActionTime = currentTimeMs;
    }
  }
  else if (gusionState == 1) { 
    float dist = Vector3::Distance(myPos, markPos);
    if (!markFound || dist < 1.0f) {
      Touch_Tap(gusionSkill2X, gusionSkill2Y); 
      usleep(gusionComboDelay * 1000);
      Touch_Tap(gusionSkill3X, gusionSkill3Y); 
      gusionState = 2; 
      lastActionTime = currentTimeMs;
    }
  }
  else if (gusionState == 2) { 
    if (markFound) {
      
      Touch_Tap(gusionSkill2X, gusionSkill2Y);
      usleep(gusionComboDelay * 800);
      Touch_Tap(gusionSkill1X, gusionSkill1Y);
      gusionState = 3;
      lastActionTime = currentTimeMs;
    }
  }
  else if (gusionState == 3) { 
    float dist = Vector3::Distance(myPos, markPos);
    if (!markFound || dist < 1.0f) {
      Touch_Tap(gusionSkill2X, gusionSkill2Y); 
      gusionState = 0; 
      lastActionTime = currentTimeMs;
    }
  }
}

namespace Joystick {
    static bool IsUserTouchNearJoy(float touchX, float touchY) {
      const float dx = touchX - AutoAim::joyCenterX;
      const float dy = touchY - AutoAim::joyCenterY;
      const float dist = std::sqrt(dx * dx + dy * dy);
      const float detectRadius = AutoAim::joyRadius + 40.0f;
      return dist < detectRadius;
    }
    static void HandleInput(float touchX, float touchY, bool isDown) {
      if (!AutoAim::enabled) return; 

      if (isDown) {
        if (IsUserTouchNearJoy(touchX, touchY)) {
          AutoAim::userTouchActive = true;
          AutoAim::autoAimPaused = true;
          AutoAim::joyDragging = true;
          AutoAim::joyActive = true;
          const float dx = touchX - AutoAim::joyCenterX;
          const float dy = touchY - AutoAim::joyCenterY;
          const float dist = std::sqrt(dx * dx + dy * dy);
          const float clampedDist = std::fmin(dist, AutoAim::joyRadius);
          AutoAim::joyMagnitude = (clampedDist - AutoAim::joyDeadZone) /
                                  (AutoAim::joyRadius - AutoAim::joyDeadZone);
          if (AutoAim::joyMagnitude < 0.0f)
            AutoAim::joyMagnitude = 0.0f;
          AutoAim::joyAngle = std::atan2(dy, dx);
          const float moveX =
                  AutoAim::joyCenterX + std::cos(AutoAim::joyAngle) * clampedDist;
          const float moveY =
                  AutoAim::joyCenterY + std::sin(AutoAim::joyAngle) * clampedDist;
          MobaTouch::SendTouchMove(moveX, moveY);
          return;
        }
      } else {
        if (AutoAim::userTouchActive) {
          AutoAim::userTouchActive = false;
          AutoAim::autoAimPaused = false;
          AutoAim::joyDragging = false;
          AutoAim::joyActive = false;
          AutoAim::joyMagnitude = 0.0f;
          MobaTouch::SendTouchUp();
          return;
        }
      }
      if (AutoAim::autoAimPaused || AutoAim::enabled)
        return;
      if (!AutoAim::joyVisible) {
        AutoAim::joyActive = false;
        MobaTouch::SendTouchUp();
        return;
      }
    }
    static void Draw(ImDrawList *draw) {
      if (!AutoAim::joyVisible)
        return;

      
      float time = (float)ImGui::GetTime();
      float pulse = sinf(time * 5.0f) * 5.0f;

      
      draw->AddCircleFilled(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY),
                            AutoAim::joyRadius + 8.0f, IM_COL32(20, 20, 40, 100));

      
      ImU32 ringCol = AutoAim::userTouchActive ? IM_COL32(255, 200, 0, 220) : IM_COL32(0, 200, 255, 180);
      draw->AddCircle(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY),
                      AutoAim::joyRadius, ringCol, 64, 3.5f);

      
      if (AutoAim::userTouchActive) {
        const float knobDist = AutoAim::joyMagnitude * (AutoAim::joyRadius - 25.0f);
        const float knobX =
                AutoAim::joyCenterX + std::cos(AutoAim::joyAngle) * knobDist;
        const float knobY =
                AutoAim::joyCenterY + std::sin(AutoAim::joyAngle) * knobDist;

        
        float knobSize = 28.0f + (AutoAim::joyMagnitude * 12.0f);
        draw->AddCircleFilled(ImVec2(knobX, knobY), knobSize + 5.0f, IM_COL32(255, 0, 0, 100)); 
        draw->AddCircleFilled(ImVec2(knobX, knobY), knobSize, IM_COL32(255, 50, 50, 255));      
        draw->AddCircleFilled(ImVec2(knobX, knobY), knobSize * 0.4f, IM_COL32(255, 255, 255, 200)); 

        
        draw->AddLine(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY), ImVec2(knobX, knobY), IM_COL32(255, 255, 255, 150), 2.0f);

        
        draw->AddText(ImVec2(AutoAim::joyCenterX - 45, AutoAim::joyCenterY - AutoAim::joyRadius - 35),
                      IM_COL32(255, 100, 100, 255), xorstr_("MANUAL AIM"));
      }
      
      else if (AutoAim::enabled) {
        
        draw->AddCircleFilled(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY), 25.0f + (pulse * 0.5f), IM_COL32(0, 255, 150, 150));
        draw->AddCircle(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY), 25.0f + (pulse * 0.5f), IM_COL32(255, 255, 255, 200), 32, 1.5f);

        draw->AddText(ImVec2(AutoAim::joyCenterX - 45, AutoAim::joyCenterY - AutoAim::joyRadius - 35),
                      IM_COL32(0, 255, 150, 255), xorstr_("AUTO READY"));
      }
      else {
        
        draw->AddCircleFilled(ImVec2(AutoAim::joyCenterX, AutoAim::joyCenterY), 22.0f, IM_COL32(80, 80, 80, 180));
      }
    }
} 
namespace AutoAim {
    inline void DrawFOV(ImDrawList *draw) {
      
      if (!showVisuals || !enabled)
        return;

      const float centerX = (float)abs_ScreenX * 0.5f;
      const float centerY = (float)abs_ScreenY * 0.5f;

      
      if (Oneself != 0) {
        Vector3 myPos;
        if (vm_readv(Oneself + OFF_SE(vCachePosition), &myPos, sizeof(myPos))) {
          
          float worldRadius = fovRadius / 45.0f;

          const int segments = 64;
          Vector2 lastPt;
          bool first = true;

          
          ImU32 col = hasTarget ? IM_COL32(255, 0, 0, 200) : IM_COL32(0, 255, 255, 150);

          for (int i = 0; i <= segments; i++) {
            float angle = (float)i / (float)segments * 2.0f * M_PI;
            
            Vector3 worldPt = {
                    myPos.X + cosf(angle) * worldRadius,
                    myPos.Y + 0.1f, 
                    myPos.Z + sinf(angle) * worldRadius
            };

            Vector2 screenPt;
            if (WorldToScreen(worldPt, &screenPt)) {
              if (!first) {
                draw->AddLine(ImVec2(lastPt.X, lastPt.Y), ImVec2(screenPt.X, screenPt.Y), col, 2.5f);
              }
              lastPt = screenPt;
              first = false;
            } else {
              first = true;
            }
          }
        }
      } else {
        
        ImU32 fovColor = hasTarget ? IM_COL32(255, 0, 0, 200) : IM_COL32(0, 255, 255, 120);
        draw->AddCircle(ImVec2(centerX, centerY), fovRadius, fovColor, 100, 2.0f);
      }

      
      const float crossSize = 12.0f;
      draw->AddLine(ImVec2(centerX - crossSize, centerY), ImVec2(centerX + crossSize, centerY), IM_COL32(255, 255, 255, 200), 1.5f);
      draw->AddLine(ImVec2(centerX, centerY - crossSize), ImVec2(centerX, centerY + crossSize), IM_COL32(255, 255, 255, 200), 1.5f);

      if (hasTarget && !autoAimPaused) {
        
        draw->AddLine(ImVec2(centerX, centerY), ImVec2(targetScreenPos.X, targetScreenPos.Y), IM_COL32(255, 255, 0, 220), 2.0f);

        
        float time = (float)clock() / CLOCKS_PER_SEC;
        float pulse = 5.0f * sinf(time * 10.0f);
        draw->AddCircle(ImVec2(targetScreenPos.X, targetScreenPos.Y), 35.0f + pulse, IM_COL32(255, 0, 0, 255), 12, 3.0f);
        draw->AddCircleFilled(ImVec2(targetScreenPos.X, targetScreenPos.Y), 6.0f, IM_COL32(255, 255, 255, 255));
      }

      if (autoAimPaused) {
        draw->AddText(ImVec2(centerX - 80, centerY + fovRadius + 20), IM_COL32(255, 100, 100, 255), xorstr_("AUTO AIM PAUSED (MANUAL)"));
      }
    }
} 

static void UpdateAutoAim(uintptr_t selfAddr, long a32, long libbase,
                          float deltaTime) {
  if (AutoAim::autoAimPaused) {
    AutoAim::hasTarget = false;
    return;
  }
  if (!AutoAim::enabled) {
    if (AutoAim::wasTargeting) {
      MobaTouch::SendTouchUp();
    }
    AutoAim::hasTarget = false;
    return;
  }
  Vector3 myPos;
  vm_readv(selfAddr + OFF_SE(vCachePosition), &myPos, sizeof(myPos));
  int CampType = Read<int>(selfAddr + OFF_SP(m_EntityCampType));
  AutoAim::TargetInfo candidates[40];
  memset(candidates, 0, sizeof(candidates));
  if (AutoAim::CollectTargets(candidates, 40, selfAddr, a32, libbase)) {
    AutoAim::TargetInfo *selected = AutoAim::SelectBestTarget(candidates, 40);
    if (selected) {
      AutoAim::hasTarget = true;
      AutoAim::currentTargetAddr = selected->address;
      AutoAim::targetScreenPos = selected->screenPos;

      
      Vector2 locScreen;
      if (WorldToScreen(myPos, &locScreen)) {
        float targetAngle = std::atan2(selected->screenPos.Y - locScreen.Y,
                                       selected->screenPos.X - locScreen.X);

        float dynamicSmooth = AutoAim::smoothFactor * (deltaTime * 60.0f);
        if (dynamicSmooth > 1.0f) dynamicSmooth = 1.0f;

        AutoAim::joyAngle = LerpAngle(AutoAim::joyAngle, targetAngle, dynamicSmooth);
        const float moveDist = AutoAim::joyRadius - 5.0f;
        const float finalX = AutoAim::joyCenterX + std::cos(AutoAim::joyAngle) * moveDist;
        const float finalY = AutoAim::joyCenterY + std::sin(AutoAim::joyAngle) * moveDist;
        MobaTouch::SendTouchMove(finalX, finalY);
      }
    } else {
      if (AutoAim::hasTarget || AutoAim::wasTargeting) {
        AutoAim::hasTarget = false;
        MobaTouch::SendTouchUp();
      }
    }
  } else {
    if (AutoAim::hasTarget || AutoAim::wasTargeting) {
      AutoAim::hasTarget = false;
      MobaTouch::SendTouchUp();
    }
  }
}



CachedRoomData g_CachedRoomData;
std::mutex g_RoomDataMutex;
static std::atomic<bool> g_RoomThreadRunning{false};
static std::atomic<bool> g_RoomThreadShouldStop{false};
static std::thread *g_RoomInfoThread = nullptr;

std::unordered_map<std::string, std::string> countryMap = {
        {"id", "Indonesia"},    {"ph", "Philippines"},
        {"my", "Malaysia"},     {"sg", "Singapore"},
        {"th", "Thailand"},     {"vn", "Vietnam"},
        {"mm", "Myanmar"},      {"kh", "Cambodia"},
        {"la", "Laos"},         {"bn", "Brunei"},
        {"tl", "Timor-Leste"},  {"jp", "Japan"},
        {"kr", "South Korea"},  {"cn", "China"},
        {"tw", "Taiwan"},       {"hk", "Hong Kong"},
        {"mo", "Macau"},        {"in", "India"},
        {"br", "Brazil"},       {"us", "USA"},
        {"ru", "Russia"},       {"tr", "Turkey"},
        {"mx", "Mexico"},       {"ar", "Argentina"},
        {"cl", "Chile"},        {"co", "Colombia"},
        {"pe", "Peru"},         {"es", "Spain"},
        {"it", "Italy"},        {"fr", "France"},
        {"de", "Germany"},      {"gb", "United Kingdom"},
        {"ua", "Ukraine"},      {"pl", "Poland"},
        {"au", "Australia"},    {"nz", "New Zealand"},
        {"sa", "Saudi Arabia"}, {"ae", "UAE"},
        {"eg", "Egypt"},        {"ma", "Morocco"}};

std::string VerifiedParty(bool bRoomLeader) {
  std::string str = std::string(xorstr_("-"));
  if (bRoomLeader) {
    str = std::string(xorstr_("Solo"));
  } else {
    str = std::string(xorstr_("Party"));
  }
  return str;
}
std::string RoadToRole(int roadValue) {
  switch (roadValue) {
    case 0:
      return std::string(xorstr_("All"));
    case 1:
      return std::string(xorstr_("Exp"));
    case 2:
      return std::string(xorstr_("Mid"));
    case 3:
      return std::string(xorstr_("Roam"));
    case 4:
      return std::string(xorstr_("Jungle"));
    case 5:
      return std::string(xorstr_("Gold"));
    default:
      return std::string(xorstr_("-"));
  }
}
std::vector<int> ParseAdditionalHeroes(uintptr_t dictPtr) {
  std::vector<int> heroIds;
  if (!dictPtr)
    return heroIds;
  monoDictionary<uint32_t, uint32_t> heroDict{};
  heroDict.klass = reinterpret_cast<void *>(Read<uintptr_t>(dictPtr + 0x0));
  heroDict.obj = reinterpret_cast<void *>(Read<uintptr_t>(dictPtr + 0x8));
  heroDict.buckets = Read<int32_t *>(dictPtr + 0x10);
  heroDict.arrayEntries = Read<uintptr_t>(dictPtr + 0x18);
  heroDict.count = Read<int>(dictPtr + 0x20);
  if (!heroDict.arrayEntries || heroDict.count <= 0)
    return heroIds;
  auto entries = get_mono_dictionary_vector<uint32_t, uint32_t>(heroDict);
  if (entries.empty())
    return heroIds;
  std::sort(entries.begin(), entries.end(),
            [](const auto &a, const auto &b) { return a.value > b.value; });
  for (const auto &entry : entries) {
    if (entry.key == 0 || entry.value == 0)
      continue;
    heroIds.push_back(static_cast<int>(entry.key));
  }
  return heroIds;
}
float ParsePlayerWinRate(uintptr_t listPtr, int currentRoad, int &outGames,
                         int &outWins) {
  if (!listPtr)
    return -1.0f;
  monoList roadList = Read<monoList>(listPtr);
  auto roads = get_mono_list_vector<uintptr_t>(roadList);
  for (auto roadInfoPtr : roads) {
    if (!roadInfoPtr)
      continue;
    int roadType = Read<int>(roadInfoPtr + 0x10);
    if (roadType != currentRoad)
      continue;
    int totalNum = Read<int>(roadInfoPtr + 0x14);
    int winNum = Read<int>(roadInfoPtr + 0x18);
    outGames = totalNum;
    outWins = winNum;
    if (totalNum <= 0)
      return -1.0f;
    return (static_cast<float>(winNum) / static_cast<float>(totalNum)) * 100.0f;
  }
  return -1.0f;
}
int GetRankValue(const CachedRoomPlayer &p) {
  
  return p.RankLevel;
}
enum ColumnID {
    COL_NICKNAME = 0,
    COL_USERID,
    COL_HERO,
    COL_RANK,
    COL_SPELL,
    COL_ROLE,
    COL_ADDHERO,
    COL_WINRATE,
    COL_PARTY,
    COL_COUNT
};
struct SortSpecs {
    int columnID;
    bool ascending;
    bool valid;
    SortSpecs() : columnID(-1), ascending(true), valid(false) {}
};
static SortSpecs g_SortSpecs[2];
bool ComparePlayers(const CachedRoomPlayer &a, const CachedRoomPlayer &b,
                    int sortCol, bool ascending) {
  int cmp = 0;
  switch (sortCol) {
    case COL_NICKNAME:
      cmp = a.Name.compare(b.Name);
          break;
    case COL_USERID:
      cmp = a.UserID.compare(b.UserID);
          break;
    case COL_HERO:
      cmp = a.Hero.compare(b.Hero);
          break;
    case COL_RANK:
      cmp = GetRankValue(a) - GetRankValue(b);
          break;
    case COL_SPELL:
      cmp = a.Spell.compare(b.Spell);
          break;
    case COL_ROLE:
      cmp = a.Role.compare(b.Role);
          break;
    case COL_ADDHERO:
      cmp = static_cast<int>(a.AddHeroIds.size() - b.AddHeroIds.size());
          break;
    case COL_WINRATE:
      if (a.WinRate < 0 && b.WinRate < 0)
        cmp = 0;
      else if (a.WinRate < 0)
        cmp = -1;
      else if (b.WinRate < 0)
        cmp = 1;
      else
        cmp = static_cast<int>((a.WinRate - b.WinRate) * 10);
          break;
    case COL_PARTY:
      cmp = a.Party.compare(b.Party);
          break;
    default:
      cmp = GetRankValue(a) - GetRankValue(b);
          break;
  }
  return ascending ? (cmp < 0) : (cmp > 0);
}

void RoomInfoReaderThread() {
  g_RoomThreadRunning = true;
  while (!g_RoomThreadShouldStop && main_thread_flag) {
    if (!is_attached || !libbase) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }

    uintptr_t systemBase = Read<uintptr_t>(libbase + OFF_SD(BasePtr));
    if (!systemBase) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }

    
    uintptr_t instancePtr = Read<uintptr_t>(systemBase + OFF_SD(PtrChain1));
    uint64_t myUid = 0;
    if (instancePtr) {
      myUid = Read<uint64_t>(instancePtr + OFF_SD(m_uiID));
    }

    
    if (myUid == 0) {
      myUid = g_LoginServerCache.AccountId;
    }

    uintptr_t dictPtr =
            Read<uintptr_t>(instancePtr + OFF_SD(m_RoomPlayerInfo));
    if (!dictPtr) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    monoDictionary<uint64_t, uintptr_t> dict{};
    dict.klass = reinterpret_cast<void *>(Read<uintptr_t>(dictPtr + 0x0));
    dict.obj = reinterpret_cast<void *>(Read<uintptr_t>(dictPtr + 0x8));
    dict.arrayEntries = Read<uintptr_t>(dictPtr + 0x18);
    dict.count = Read<int>(dictPtr + 0x20);
    if (!dict.arrayEntries || dict.count <= 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    auto entries = get_mono_dictionary_vector<uint64_t, uintptr_t>(dict);
    if (entries.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    const auto &lastEntry = entries.back();
    if (!lastEntry.value) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    auto playerList = Read<monoList>(lastEntry.value);
    auto players = get_mono_list_vector<uintptr_t>(playerList);
    if (players.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }
    std::vector<CachedRoomPlayer> tempTeamA;
    std::vector<CachedRoomPlayer> tempTeamB;
    for (auto infoPtr : players) {
      if (!infoPtr)
        continue;
      CachedRoomPlayer p{};
      p.Address = infoPtr; 
      p.Camp = Read<int>(infoPtr + OFF_RD(iCamp));
      p.IsRoomLeader = Read<bool>(infoPtr + OFF_RD(bRoomLeader));
      p.Name = fshy(Read<uintptr_t>(infoPtr + OFF_RD(_sName)));
      p.HeroId = Read<int>(infoPtr + OFF_RD(heroid));
      p.Hero = HeroToString(p.HeroId);
      p.Rank = RankToString(Read<int>(infoPtr + OFF_RD(uiRankLevel)),
                            Read<int>(infoPtr + OFF_RD(iMythPoint)));
      p.SpellId = Read<int>(infoPtr + OFF_RD(summonSkillId));
      p.Spell = SpellToString(p.SpellId);
      uint64_t playerUid = Read<uint64_t>(infoPtr + OFF_RD(lUid));
      p.UserID = std::to_string(playerUid);

      p.Squad = fshy(Read<uintptr_t>(infoPtr + OFF_RD(_steamSimpleName)));
      p.CountryId = Read<int>(infoPtr + OFF_RD(country));
      p.Country = fshy(Read<uintptr_t>(infoPtr + OFF_RD(sThisLoginCountry)));
      if (p.Country.empty())
        p.Country = "-";
      p.Party = VerifiedParty(p.IsRoomLeader);
      p.Role = RoadToRole(Read<int>(infoPtr + OFF_RD(iRoad)));
      auto heroBattleMapPtr =
              Read<uintptr_t>(infoPtr + OFF_RD(mapHeroBattleNum));
      p.AddHeroIds = ParseAdditionalHeroes(heroBattleMapPtr);
      auto roadInfoListPtr =
              Read<uintptr_t>(infoPtr + OFF_RD(vCurSeasonRealRoadInfo));
      int currentRoad = Read<int>(infoPtr + OFF_RD(iRoad));
      p.WinRate =
              ParsePlayerWinRate(roadInfoListPtr, currentRoad, p.RoadGames, p.Wins);
      p.RankLevel = Read<int>(infoPtr + OFF_RD(uiRankLevel));
      p.ZoneId = Read<int>(infoPtr + OFF_RD(uiZoneId));
      p.SkinID = Read<int>(infoPtr + OFF_RD(heroskin));
      p.IsValid = true;

      if (p.Camp == 1)
        tempTeamA.push_back(p);
      else if (p.Camp == 2)
        tempTeamB.push_back(p);
    }

    int foundCamp = 0;
    std::string myUidStr = std::to_string(myUid);
    for (auto &p : tempTeamA) {
      if (p.UserID == myUidStr) {
        foundCamp = 1;
        
        if (!p.Name.empty()) g_LoginServerCache.Nickname = p.Name;
        g_LoginServerCache.AccountId = myUid;
        g_LoginServerCache.IsValid = true;
        break;
      }
    }
    if (foundCamp == 0) {
      for (auto &p : tempTeamB) {
        if (p.UserID == myUidStr) {
          foundCamp = 2;
          
          if (!p.Name.empty()) g_LoginServerCache.Nickname = p.Name;
          g_LoginServerCache.AccountId = myUid;
          g_LoginServerCache.IsValid = true;
          break;
        }
      }
    }

    {
      std::lock_guard<std::mutex> lock(g_RoomDataMutex);
      g_CachedRoomData.TeamA = std::move(tempTeamA);
      g_CachedRoomData.TeamB = std::move(tempTeamB);
      g_CachedRoomData.GroupID = lastEntry.key;
      g_CachedRoomData.HasData = true;
      if (foundCamp != 0) {
        g_CachedRoomData.LocalPlayerCamp = foundCamp;
      }
      g_CachedRoomData.LastUpdate = std::time(nullptr);
    }
    {
      static bool lastDODState = false;
      bool currentDODState = enableDOD;
      if (currentDODState != lastDODState) {
        auto chainPtr = Read<uintptr_t>(systemBase + OFF_SD(PtrChain1));
        if (chainPtr) {
          Write<bool>(chainPtr + OFF_SD(m_bOfflineManagerRun), currentDODState);
        }
        lastDODState = currentDODState;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
  g_RoomThreadRunning = false;
}

#define GAMESERVERCONFIG_TYPEINFO 0x635bcf8
#define GAMECOMMON_TYPEINFO 0x760cdc0
#define STATIC_FIELD 0xa8
#define GAMESERVERCONFIG_INSTANCE 0xa8
#define GAMESERVERCONFIG_LOGININFO 0x250
#define GAMECOMMON_INST 0x0
#define GAMECOMMON_LOGININFO 0x58
#define LOGININFO_ACCOUNT_ID 0x28
#define LOGININFO_ACC_CREATE_TIME 0x88
LoginServerData ReadLoginServerInfo(uintptr_t libbase) {
  LoginServerData data{};
  uintptr_t loginInfoPtr = 0;
  if (GAMESERVERCONFIG_TYPEINFO != 0) {
    uintptr_t typeInfo = ReadPtr(libbase + GAMESERVERCONFIG_TYPEINFO);
    if (typeInfo) {
      uintptr_t staticField = ReadPtr(typeInfo + STATIC_FIELD);
      if (staticField) {
        uintptr_t instance = ReadPtr(staticField + GAMESERVERCONFIG_INSTANCE);
        if (instance) {
          loginInfoPtr = ReadPtr(instance + GAMESERVERCONFIG_LOGININFO);
        }
      }
    }
  }
  if (loginInfoPtr == 0 && GAMECOMMON_TYPEINFO != 0) {
    uintptr_t typeInfo = ReadPtr(libbase + GAMECOMMON_TYPEINFO);
    if (typeInfo) {
      uintptr_t staticField = ReadPtr(typeInfo + STATIC_FIELD);
      if (staticField) {
        uintptr_t inst = ReadPtr(staticField + GAMECOMMON_INST);
        if (inst) {
          loginInfoPtr = ReadPtr(inst + GAMECOMMON_LOGININFO);
        }
      }
    }
  }
  if (!loginInfoPtr)
    return data;

  data.AccountId = Read<uint64_t>(loginInfoPtr + LOGININFO_ACCOUNT_ID);

  
  uintptr_t nickPtr = Read<uintptr_t>(loginInfoPtr + 0x18);
  if (nickPtr) data.Nickname = fshy(nickPtr);
  if (data.Nickname.empty()) {
    nickPtr = Read<uintptr_t>(loginInfoPtr + 0x10);
    if (nickPtr) data.Nickname = fshy(nickPtr);
  }

  data.ClientRealVersion = fshy(Read<uintptr_t>(loginInfoPtr + 0x30));
  data.ClientStreamVersion = fshy(Read<uintptr_t>(loginInfoPtr + 0x38));
  data.ClientLocalVersion = fshy(Read<uintptr_t>(loginInfoPtr + 0x40));
  data.ClientLoginVersion = fshy(Read<uintptr_t>(loginInfoPtr + 0x50));
  data.Channel = fshy(Read<uintptr_t>(loginInfoPtr + 0x58));
  data.IsNewAccount = Read<bool>(loginInfoPtr + 0x60);
  data.RegionId = Read<uint32_t>(loginInfoPtr + 0x64);
  data.CountryInfo = fshy(Read<uintptr_t>(loginInfoPtr + 0x68));
  data.CountryInfoByIP = fshy(Read<uintptr_t>(loginInfoPtr + 0x70));
  data.InternetOperator = fshy(Read<uintptr_t>(loginInfoPtr + 0x78));
  data.State = fshy(Read<uintptr_t>(loginInfoPtr + 0x80));
  data.AccCreateTime = Read<uint32_t>(loginInfoPtr + LOGININFO_ACC_CREATE_TIME);
  data.AccountFlag = Read<uint32_t>(loginInfoPtr + 0x8c);
  data.UserIP = fshy(Read<uintptr_t>(loginInfoPtr + 0x90));
  data.GameServerIP = fshy(Read<uintptr_t>(loginInfoPtr + 0xb8));
  data.GameServerPort = Read<int32_t>(loginInfoPtr + 0xc0);
  data.EngineBuildVersion = fshy(Read<uintptr_t>(loginInfoPtr + 0xf8));
  data.IsValid = true;
  return data;
}
LoginServerData g_LoginServerCache{};
std::time_t g_LoginServerLastUpdate = 0;
constexpr float LOGIN_INFO_CACHE_DURATION = 2.0f;
void RenderLoginServerInfo() {
  auto now = std::time(nullptr);
  ImGui::Separator();
  ImGui::BeginGroup();
  ImGui::Text(xorstr_("Account ID:"));
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.0f, 1.0f), xorstr_("%llu"),
                     g_LoginServerCache.AccountId); 
  ImGui::Text(xorstr_("Region ID:"));
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.0f, 0.4f, 0.7f, 1.0f), xorstr_("%u"),
                     g_LoginServerCache.RegionId); 
  ImGui::Text(xorstr_("New Account:"));
  ImGui::SameLine();
  ImGui::TextColored(g_LoginServerCache.IsNewAccount
                     ? ImVec4(0.0f, 0.6f, 0.0f, 1.0f)
                     : ImVec4(0.8f, 0.0f, 0.0f, 1.0f),
                     xorstr_("%s"), g_LoginServerCache.IsNewAccount ? xorstr_("Yes") : xorstr_("No"));
  ImGui::EndGroup();
  ImGui::SameLine();
  ImGui::BeginGroup();
  ImGui::Text(xorstr_("Channel:"));
  ImGui::SameLine();
  ImGui::Text(xorstr_("%s"), g_LoginServerCache.Channel.empty()
                             ? xorstr_("-")
                             : g_LoginServerCache.Channel.c_str());
  ImGui::Text(xorstr_("State:"));
  ImGui::SameLine();
  ImGui::Text(xorstr_("%s"), g_LoginServerCache.State.empty()
                             ? xorstr_("-")
                             : g_LoginServerCache.State.c_str());
  ImGui::EndGroup();
  ImGui::Separator();
  if (ImGui::BeginTable(xorstr_("##VersionTable"), 2,
                        ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn(xorstr_("Key"), ImGuiTableColumnFlags_WidthFixed, 180.0f);
    ImGui::TableSetupColumn(xorstr_("Value"), ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    auto AddRow = [](const char *key, const std::string &value) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(xorstr_("%s"), key);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(xorstr_("%s"), value.empty() ? xorstr_("-") : value.c_str());
    };
    AddRow(xorstr_("Real Version"), g_LoginServerCache.ClientRealVersion);
    AddRow(xorstr_("Stream Version"), g_LoginServerCache.ClientStreamVersion);
    AddRow(xorstr_("Local Version"), g_LoginServerCache.ClientLocalVersion);
    AddRow(xorstr_("Login Version"), g_LoginServerCache.ClientLoginVersion);
    AddRow(xorstr_("Engine Build"), g_LoginServerCache.EngineBuildVersion);
    ImGui::EndTable();
  }
  ImGui::Separator();
  if (ImGui::BeginTable(xorstr_("##NetworkTable"), 2,
                        ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg)) {
    ImGui::TableSetupColumn(xorstr_("Key"), ImGuiTableColumnFlags_WidthFixed, 180.0f);
    ImGui::TableSetupColumn(xorstr_("Value"), ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(xorstr_("User IP"));
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(xorstr_("%s"), g_LoginServerCache.UserIP.empty()
                               ? xorstr_("-")
                               : g_LoginServerCache.UserIP.c_str());
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(xorstr_("Game Server"));
    ImGui::TableSetColumnIndex(1);
    if (!g_LoginServerCache.GameServerIP.empty()) {
      ImGui::Text(xorstr_("%s:%d"), g_LoginServerCache.GameServerIP.c_str(),
                  g_LoginServerCache.GameServerPort);
    } else {
      ImGui::Text(xorstr_("-"));
    }
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(xorstr_("Country (Info)"));
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(xorstr_("%s"), g_LoginServerCache.CountryInfo.empty()
                               ? xorstr_("-")
                               : g_LoginServerCache.CountryInfo.c_str());
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(xorstr_("Country (IP)"));
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(xorstr_("%s"), g_LoginServerCache.CountryInfoByIP.empty()
                               ? xorstr_("-")
                               : g_LoginServerCache.CountryInfoByIP.c_str());
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(xorstr_("ISP"));
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(xorstr_("%s"), g_LoginServerCache.InternetOperator.empty()
                               ? xorstr_("-")
                               : g_LoginServerCache.InternetOperator.c_str());
    ImGui::EndTable();
  }
  ImGui::Separator();
  ImGui::Text(xorstr_("Account Created:"));
  ImGui::SameLine();
  if (g_LoginServerCache.AccCreateTime > 0) {
    std::time_t createT =
            static_cast<std::time_t>(g_LoginServerCache.AccCreateTime);
    char timeBuf[64];
    std::strftime(timeBuf, sizeof(timeBuf), xorstr_("%Y-%m-%d %H:%M:%S"),
                  std::localtime(&createT));
    ImGui::Text(xorstr_("%s (Unix: %u)"), timeBuf, g_LoginServerCache.AccCreateTime);
  } else {
    ImGui::Text(xorstr_("-"));
  }
  ImGui::Text(xorstr_("Account Flag:"));
  ImGui::SameLine();
  ImGui::Text(xorstr_("0x%X (%u)"), g_LoginServerCache.AccountFlag,
              g_LoginServerCache.AccountFlag);
  ImGui::Spacing();
  if (ImGui::Button(xorstr_("Refresh Data"))) {
    g_LoginServerCache = ReadLoginServerInfo(libbase);
    g_LoginServerLastUpdate = std::time(nullptr);
  }
}
bool fakerank = false;

static int currentRankIndex = 0;
static bool writePending = false;
static int tap_count = 0;
static float last_tap_time = 0.0f;
float FloatingIcon_PosX = 0.12f;
float FloatingIcon_PosY = 0.5f;
bool ForceUpdateIconPos = false;

void DrawFloatingLogo() {
  static ImTextureID logoTex = nullptr;
  if (!logoTex) {
    int w, h, ch;
    unsigned char *pixels =
            stbi_load_from_memory(icon, sizeof(icon), &w, &h, &ch, 4);
    if (pixels) {
      GLuint tex;
      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   pixels);
      stbi_image_free(pixels);
      logoTex = (ImTextureID)(intptr_t)tex;
    }
  }

  if (!logoTex)
    return;

  
  float iconSize = 125.0f;

  
  ImGui::SetNextWindowPos(
          ImVec2(abs_ScreenX * FloatingIcon_PosX, abs_ScreenY * FloatingIcon_PosY),
          ImGuiCond_Always, ImVec2(0.5f, 0.5f));

  ImGui::Begin("##FloatingLogo", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
               ImGuiWindowFlags_AlwaysAutoResize |
               ImGuiWindowFlags_NoSavedSettings |
               ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

  
  ImGui::Image(logoTex, ImVec2(iconSize, iconSize));

  
  ImGui::SetCursorScreenPos(ImGui::GetItemRectMin());
  ImGui::InvisibleButton("##LogoBtn", ImVec2(iconSize, iconSize));

  
  if (ImGui::IsItemClicked()) {
    show_menu = !show_menu;
  }

  
  if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    FloatingIcon_PosX += ImGui::GetIO().MouseDelta.x / abs_ScreenX;
    FloatingIcon_PosY += ImGui::GetIO().MouseDelta.y / abs_ScreenY;

    
    if (FloatingIcon_PosX < 0.05f)
      FloatingIcon_PosX = 0.05f;
    if (FloatingIcon_PosX > 0.95f)
      FloatingIcon_PosX = 0.95f;
    if (FloatingIcon_PosY < 0.05f)
      FloatingIcon_PosY = 0.05f;
    if (FloatingIcon_PosY > 0.95f)
      FloatingIcon_PosY = 0.95f;
  }

  ImGui::End();
}
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define SETTINGS_PATH xorstr_("/data/local/tmp/TatsumiLoader.cfg")
inline void SaveTatsumiSettings() {
  std::unordered_map<std::string, std::string> old;
  { std::ifstream in(SETTINGS_PATH);
    std::string line;
    while (std::getline(in, line)) {
      size_t eq = line.find('=');
      if (eq != std::string::npos) old[line.substr(0, eq)] = line.substr(eq + 1);
    } }
  std::ofstream out(SETTINGS_PATH);
  if (!out.is_open())
    return;
  out << xorstr_("cfg_version=2\n");
#define X(k, v) out << xorstr_(k) << "=" << ((v) ? 1 : 0) << "\n";
  TATSUMI_BOOL_SETTINGS
#undef X
#define X(k, v) out << xorstr_(k) << "=" << (v) << "\n";
  TATSUMI_FLOAT_SETTINGS
  TATSUMI_INT_SETTINGS
#undef X
  out << xorstr_("predictionLineColor=") << predictionLineColor.Value.x << ","
      << predictionLineColor.Value.y << "," << predictionLineColor.Value.z
      << "," << predictionLineColor.Value.w << "\n";
  out << xorstr_("predictionCircleColor=") << predictionCircleColor.Value.x << ","
      << predictionCircleColor.Value.y << "," << predictionCircleColor.Value.z
      << "," << predictionCircleColor.Value.w << "\n";
  static const char *kKnown[] = {
#define X(k, v) k,
    TATSUMI_BOOL_SETTINGS TATSUMI_FLOAT_SETTINGS TATSUMI_INT_SETTINGS
    "predictionLineColor", "predictionCircleColor", "cfg_version",
#undef X
  };
  for (auto &kv : old) {
    bool known = false;
    for (auto *kk : kKnown)
      if (kv.first == kk) { known = true; break; }
    if (!known) out << kv.first << "=" << kv.second << "\n";
  }
  out.close();
}

inline void LoadTatsumiSettings() {
  std::ifstream in(SETTINGS_PATH);
  if (!in.is_open())
    return;
  std::unordered_map<std::string, std::string> cfg;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    size_t eq = line.find('=');
    if (eq != std::string::npos) {
      cfg[line.substr(0, eq)] = line.substr(eq + 1);
    }
  }
  in.close();
  auto getVal = [&](const std::string &key) -> const std::string & {
      static std::string empty;
      auto it = cfg.find(key);
      return it != cfg.end() ? it->second : empty;
  };
  try {
#define LOAD_BOOL(k, v)                                                        \
  if (!getVal(k).empty())                                                      \
    v = (getVal(k) == "1");
#define LOAD_FLOAT(k, v)                                                       \
  if (!getVal(k).empty())                                                      \
    v = std::stof(getVal(k));
#define LOAD_INT(k, v)                                                         \
  if (!getVal(k).empty())                                                      \
    v = std::stoi(getVal(k));
#define X(k, v) LOAD_BOOL(xorstr_(k), v);
    TATSUMI_BOOL_SETTINGS
#undef X
#define X(k, v) LOAD_FLOAT(xorstr_(k), v);
    TATSUMI_FLOAT_SETTINGS
#undef X
#define X(k, v) LOAD_INT(xorstr_(k), v);
    TATSUMI_INT_SETTINGS
#undef X
    if (!getVal(xorstr_("predictionLineColor")).empty()) {
      std::istringstream iss(getVal(xorstr_("predictionLineColor")));
      char c;
      iss >> predictionLineColor.Value.x >> c >> predictionLineColor.Value.y >>
          c >> predictionLineColor.Value.z >> c >> predictionLineColor.Value.w;
    }
    if (!getVal(xorstr_("predictionCircleColor")).empty()) {
      std::istringstream iss(getVal(xorstr_("predictionCircleColor")));
      char c;
      iss >> predictionCircleColor.Value.x >> c >>
          predictionCircleColor.Value.y >> c >> predictionCircleColor.Value.z >>
          c >> predictionCircleColor.Value.w;
    }
#undef LOAD_BOOL
#undef LOAD_FLOAT
#undef LOAD_INT
  } catch (...) {
  }
}



inline void OpenURL(const char *url) {
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "am start -a android.intent.action.VIEW -d '%s' >/dev/null 2>&1 &", url);
  system(cmd);
}

void Layout_tick_UI() {
  ImGuiIO &io = ImGui::GetIO();

  
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::StyleColorsDark();
  style.WindowPadding = ImVec2(22, 22);
  style.WindowRounding = 12.0f;
  style.ChildRounding = 8.0f;
  style.FramePadding = ImVec2(16, 12);
  style.FrameRounding = 10.0f;
  style.ItemSpacing = ImVec2(16, 14);
  style.ItemInnerSpacing = ImVec2(10, 8);
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 32.0f;
  style.ScrollbarRounding = 15.0f;
  style.GrabMinSize = 28.0f;
  style.TouchExtraPadding = ImVec2(12, 12);
  style.GrabRounding = 6.0f;
  style.TabRounding = 6.0f;
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowBorderSize = 1.0f;


  
  ImVec4* colors = style.Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.98f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.12f, 0.12f, 0.12f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.18f, 0.18f, 0.18f, 0.40f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.24f, 0.24f, 0.24f, 0.67f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  colors[ImGuiCol_Button]                 = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.20f, 0.20f, 0.31f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
  colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.30f, 0.30f, 0.30f, 0.78f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.25f, 0.25f, 0.25f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.35f, 0.35f, 0.35f, 0.67f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.45f, 0.45f, 0.45f, 0.95f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
  colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
  colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
  colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.35f, 0.35f, 0.35f, 0.35f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  // Mobile-first: skala font + ukuran window mengikuti layar HP.
  {
    float uiScale = io.DisplaySize.y > 0 ? io.DisplaySize.y / 1080.0f : 1.0f;
    if (uiScale < 0.8f) uiScale = 0.8f;
    if (uiScale > 1.4f) uiScale = 1.4f;
    io.FontGlobalScale = uiScale * uiUserScale;
    // Resize via tepi + sudut (gampang diraih jempol), bukan cuma grip kecil.
    io.ConfigWindowsResizeFromEdges = true;
  }
  if (is_root_mode) {
    DrawFloatingLogo();
    if (show_menu) {
        float winW = io.DisplaySize.x * 0.92f;
        float winH = io.DisplaySize.y * 0.88f;
        if (winW > 900.0f) winW = 900.0f;
        if (winH > 700.0f) winH = 700.0f;
        if (winW < 480.0f) winW = 480.0f;
        if (winH < 560.0f) winH = 560.0f;
        ImGui::SetNextWindowSize(ImVec2(winW, winH), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(xorstr_("TatsumiLoader-MLBB"), &show_menu)) {
          if (ImGui::BeginTabBar(xorstr_("##MainTabs"))) {
            if (ImGui::BeginTabItem(xorstr_("AUTO"))) {
              if (ImGui::CollapsingHeader(xorstr_("AUTO RETRI"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Auto Retri"), &autoRetribution);
                ImGui::CustomCheckbox(xorstr_("Show Circle"), &showRetriCircle);
                ImGui::SliderFloat(xorstr_("Retri X"), &retriTouchX, 0.0f, 3000.0f);
                ImGui::SliderFloat(xorstr_("Retri Y"), &retriTouchY, 0.0f, 1500.0f);
                ImGui::SliderFloat(xorstr_("Early Margin"), &retriEarlyMargin, 0.0f, 500.0f);
                ImGui::SliderFloat(xorstr_("Max Range"), &retriMaxRange, 5.0f, 12.0f);
                ImGui::SliderInt(xorstr_("Spam Ms"), &retriSpamMs, 30, 300);
                ImGui::CustomCheckbox(xorstr_("Double Tap"), &retriDoubleTap);
                ImGui::CustomCheckbox(xorstr_("Nearby Enemy Only"), &retriNearbyOnly);
                if (retriNearbyOnly) ImGui::SliderFloat(xorstr_("Nearby Range"), &retriNearbyRange, 3.0f, 30.0f);
              }
              if (ImGui::CollapsingHeader(xorstr_("TARGETS"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Buff Red"), &AutoRetributionRed);
                ImGui::CustomCheckbox(xorstr_("Buff Blue"), &AutoRetributionBlue);
                ImGui::CustomCheckbox(xorstr_("Lord"), &AutoRetributionLord);
                ImGui::CustomCheckbox(xorstr_("Turtle"), &AutoRetributionTurtle);
                ImGui::CustomCheckbox(xorstr_("Crab"), &AutoRetributionCrab);
                ImGui::CustomCheckbox(xorstr_("Lito"), &AutoRetributionLito);
              }
              if (ImGui::CollapsingHeader(xorstr_("AUTO SPELL (EXECUTE)"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Auto Execute"), &autoSpellExecute);
                if (autoSpellExecute) {
                  ImGui::SliderFloat(xorstr_("Execute HP %"), &spellExecPct, 1.0f, 40.0f);
                  ImGui::SliderFloat(xorstr_("Execute Range"), &spellRange, 1.0f, 10.0f);
                  ImGui::SliderFloat(xorstr_("Spell X"), &spellX, 0.0f, 3000.0f);
                  ImGui::SliderFloat(xorstr_("Spell Y"), &spellY, 0.0f, 1500.0f);
                  ImGui::SliderInt(xorstr_("Spell Spam Ms"), &spellSpamMs, 300, 5000);
                }
              }
              if (ImGui::CollapsingHeader(xorstr_("OBJECTIVE ALERT"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Lord/Turtle Alert"), &drawObjectiveAlert);
                ImGui::SliderFloat(xorstr_("Low HP %"), &objectiveLowPct, 5.0f, 50.0f);
              }
              if (ImGui::CollapsingHeader(xorstr_("LING AUTO SWORD"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Auto Sword"), &autoSwordLing);

                if (autoSwordLing ) {

                  ImGui::SliderFloat(xorstr_("Dash Range"), &lingSwordDashRange, 1.0f, 20.0f);
                  ImGui::SliderFloat(xorstr_("Dash Delay"), &lingDashDelay, -0.1f, 1.0f);
                  ImGui::SliderFloat(xorstr_("Skill 2 X"), &lingSkill2X, 0.0f, (float)abs_ScreenX);
                  ImGui::SliderFloat(xorstr_("Skill 2 Y"), &lingSkill2Y, 0.0f, (float)abs_ScreenY);

                  if (ImGui::Button(xorstr_("Reset Skill 2 Pos"), ImVec2(-1, 0))) {
                    lingSkill2X = abs_ScreenX * 0.85f;
                    lingSkill2Y = abs_ScreenY * 0.75f;
                  }
                }
              }
              if (ImGui::CollapsingHeader(xorstr_("GUSION AUTO COMBO"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Auto Combo"), &autoComboGusion);
                if (autoComboGusion) {
                  ImGui::TextDisabled(xorstr_("Combo: 1-2-1-2-3 (Auto Reset)"));
                  ImGui::SliderInt(xorstr_("Combo Speed (ms)"), &gusionComboDelay, 10, 500);

                  ImGui::SliderFloat(xorstr_("Skill 2 X"), &gusionSkill2X, 0.0f, (float)abs_ScreenX);
                  ImGui::SliderFloat(xorstr_("Skill 2 Y"), &gusionSkill2Y, 0.0f, (float)abs_ScreenY);
                  ImGui::SliderFloat(xorstr_("Skill 3 X"), &gusionSkill3X, 0.0f, (float)abs_ScreenX);
                  ImGui::SliderFloat(xorstr_("Skill 3 Y"), &gusionSkill3Y, 0.0f, (float)abs_ScreenY);

                  if (ImGui::Button(xorstr_("Reset Gusion Buttons"), ImVec2(-1, 0))) {
                    gusionSkill1X = abs_ScreenX * 0.85f; gusionSkill1Y = abs_ScreenY * 0.75f;
                    gusionSkill2X = abs_ScreenX * 0.75f; gusionSkill2Y = abs_ScreenY * 0.85f;
                    gusionSkill3X = abs_ScreenX * 0.70f; gusionSkill3Y = abs_ScreenY * 0.65f;
                  }
                }
              }
              if (ImGui::CollapsingHeader(xorstr_("DD KIMMY AUTO AIM"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Kimmy Aim"), &AutoAim::enabled);
                if (AutoAim::enabled) {
                  ImGui::SliderFloat(xorstr_("FOV Radius"), &AutoAim::fovRadius, 50.0f, 1500.0f);
                  ImGui::SliderFloat(xorstr_("Smooth Factor"), &AutoAim::smoothFactor, 0.01f, 0.5f);
                  const char* priorities[] = { "Closest", "Lowest HP", "Player Priority" };
                  ImGui::Combo(xorstr_("Target Priority"), &AutoAim::priorityMode, priorities, 3);
                  ImGui::CustomCheckbox(xorstr_("Show Visuals"), &AutoAim::showVisuals);
                }
                ImGui::Separator();
                ImGui::CustomCheckbox(xorstr_("Show Joystick UI"), &AutoAim::joyVisible);
                if (AutoAim::joyVisible || AutoAim::enabled) {
                  ImGui::SliderFloat(xorstr_("Joy Radius"), &AutoAim::joyRadius, 100.0f, 300.0f);
                  ImGui::SliderFloat(xorstr_("Joy X"), &AutoAim::joyCenterX, 0.0f, (float)abs_ScreenX);
                  ImGui::SliderFloat(xorstr_("Joy Y"), &AutoAim::joyCenterY, 0.0f, (float)abs_ScreenY);
                  if (ImGui::Button(xorstr_("Reset to Right"), ImVec2(-1, 0))) {
                    AutoAim::joyCenterX = abs_ScreenX * 0.85f;
                    AutoAim::joyCenterY = abs_ScreenY * 0.75f;
                  }
                }
              }
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(xorstr_("ESP"))) {
              if (ImGui::CollapsingHeader(xorstr_("PLAYER ESP"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("ESP Line"), &drawMLine);
                ImGui::CustomCheckbox(xorstr_("Hero ESP"), &iconhero);
                if (iconhero || ESP_Player_Cooldown) {
                  ImGui::SliderFloat(xorstr_("ESP Height"), &Avatar_OffsetY, 0.0f, 500.0f);
                  ImGui::SliderFloat(xorstr_("Icon Size"), &g_HeroIconSize, 10.0f, 60.0f);
                }
                ImGui::CustomCheckbox(xorstr_("Prediction ESP"), &drawMPrediction);
              }
              if (ImGui::CollapsingHeader(xorstr_("COOLDOWN & DRONE"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enemy Cooldown"), &ESP_Player_Cooldown);
                if (ESP_Player_Cooldown) {
                  ImGui::SliderFloat(xorstr_("CD Size"), &Cooldown_Size, 5.0f, 40.0f);
                }
                ImGui::Separator();
                ImGui::CustomCheckbox(xorstr_("Enable Drone"), &EnableDrone);
                if (EnableDrone) ImGui::SliderFloat(xorstr_("FOV"), &FieldView, -9.0f, -1.0f);
              }
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(xorstr_("VISUAL"))) {
              if (ImGui::CollapsingHeader(xorstr_("MONSTERS"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Monster ESP"), &drawMonsterName);
                if (drawMonsterName) {
                  ImGui::SliderFloat(xorstr_("Monster Icon Size"), &g_MonsterIconSize, 10.0f, 60.0f);
                }
              }
              if (ImGui::CollapsingHeader(xorstr_("MINIMAP"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Enable Minimap"), &MinimapIcon);
                ImGui::CustomCheckbox(xorstr_("Show Monsters"), &drawMonsterMinimap);
                ImGui::CustomCheckbox(xorstr_("Show Lord/Turtle"), &MinimapIconLordTurtle);
                ImGui::CustomCheckbox(xorstr_("Hide Minimap Line"), &HideLine);
                ImGui::CustomCheckbox(xorstr_("Show All Monsters"), &MinimapIconBuff);
                ImGui::CustomCheckbox(xorstr_("Show Minion Dot"), &drawMinionMinimap);
                ImGui::SliderInt(xorstr_("Minimap Size"), &MinimapSize, 100, 600);
                ImGui::SliderInt(xorstr_("Minimap X"), &MinimapPos, 0, 1000);
                ImGui::SliderInt(xorstr_("Minimap Y"), &MinimapPosY, 0, 1000);
                ImGui::SliderInt(xorstr_("Hero Icon Size"), &g_MinimapHeroSize, 10, 100);
                ImGui::SliderInt(xorstr_("Monster Icon Size"), &g_MinimapMonsterSize, 10, 100);
                ImGui::SliderInt(xorstr_("Minion Dot Size"), &g_ICSize, 2, 20); 
                if (ImGui::TreeNode(xorstr_("Calibration"))) {
                  ImGui::SliderFloat(xorstr_("Scale"), &g_MinimapScale, 10.0f, 200.0f);
                  ImGui::SliderFloat(xorstr_("Offset X"), &g_Res1_OffsetX, -100.0f, 100.0f);
                  ImGui::SliderFloat(xorstr_("Offset Y"), &g_Res1_OffsetY, -100.0f, 100.0f);
                  ImGui::TreePop();
                }
              }
              if (ImGui::CollapsingHeader(xorstr_("ALERT WARNING"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::CustomCheckbox(xorstr_("Show Alert"), &drawAlertUnderAttack);
                ImGui::CustomCheckbox(xorstr_("Show Alert HP"), &Alert_ShowHPText);
                ImGui::SliderFloat(xorstr_("Alert X (%)"), &Alert_PosX, 0.0f, 1.0f);
                ImGui::SliderFloat(xorstr_("Alert Y (%)"), &Alert_PosY, 0.0f, 1.0f);
                ImGui::SliderFloat(xorstr_("Alert Scale"), &Alert_Scale, 0.5f, 3.0f);
              }
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(xorstr_("ROOM"))) {
              RenderRoomPlayerInfoImGui();
              ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(xorstr_("INFO"))) {
              ImGui::BeginChild(xorstr_("##InfoBox"), ImVec2(0, 0), false);

              
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
              ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(xorstr_("--- PREMIUM SUBSCRIPTION ---")).x) * 0.5f);
              ImGui::Text(xorstr_("--- PREMIUM SUBSCRIPTION ---"));
              ImGui::PopStyleColor();
              ImGui::Separator();
              ImGui::Spacing();

              auto InfoRow = [](const char* label, const char* val, ImVec4 col = ImVec4(1,1,1,1)) {
                  ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), xorstr_("%s"), label);
                  ImGui::SameLine(160);
                  ImGui::TextColored(col, xorstr_(": %s"), val);
                  ImGui::Spacing();
              };


              
              ImGui::Spacing();
              ImGui::Separator();
              ImGui::Spacing();

              
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
              ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(xorstr_("--- DEVICE INFORMATION ---")).x) * 0.5f);
              ImGui::Text(xorstr_("--- DEVICE INFORMATION ---"));
              ImGui::PopStyleColor();
              ImGui::Spacing();

              char model[PROP_VALUE_MAX] = {0};
              __system_property_get(xorstr_("ro.product.model"), model);
              InfoRow(xorstr_("Model"), model);

              InfoRow(xorstr_("Package"), g_package_name.c_str());
              InfoRow(xorstr_("Injection"), is_attached ? xorstr_("Active") : xorstr_("Pending"),
                      is_attached ? ImVec4(0,1,0,1) : ImVec4(1,0,0,1));
              InfoRow(xorstr_("Version"), kTatsumiVersion);

              ImGui::Spacing();
              ImGui::Separator();
              ImGui::Spacing();
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
              ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(xorstr_("--- FOLLOW US ---")).x) * 0.5f);
              ImGui::Text(xorstr_("--- FOLLOW US ---"));
              ImGui::PopStyleColor();
              ImGui::Spacing();
              {
                float bw = (ImGui::GetContentRegionAvail().x - 8.0f) * 0.5f;
                ImDrawList *fg = ImGui::GetWindowDrawList();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.15f, 0.45f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.55f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.65f, 0.10f, 0.38f, 1.00f));
                if (ImGui::Button(xorstr_("  Instagram @ikyletwar"), ImVec2(bw, 40))) OpenURL(xorstr_("https://www.instagram.com/ikyletwar/"));
                ImVec2 igMn = ImGui::GetItemRectMin(), igMx = ImGui::GetItemRectMax();
                ImVec2 igC = ImVec2(igMn.x + 22.0f, (igMn.y + igMx.y) * 0.5f);
                fg->AddRect(ImVec2(igC.x - 8.0f, igC.y - 8.0f), ImVec2(igC.x + 8.0f, igC.y + 8.0f), IM_COL32(255,255,255,255), 3.0f, 0, 2.0f);
                fg->AddCircle(igC, 3.5f, IM_COL32(255,255,255,255), 16, 2.0f);
                fg->AddCircleFilled(ImVec2(igC.x + 5.5f, igC.y - 5.5f), 1.6f, IM_COL32(255,255,255,255));
                ImGui::PopStyleColor(3);
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.08f, 0.08f, 0.10f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.20f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.05f, 1.00f));
                if (ImGui::Button(xorstr_("  TikTok @ikyletwar"), ImVec2(bw, 40))) OpenURL(xorstr_("https://www.tiktok.com/@ikyletwar"));
                ImVec2 ttMn = ImGui::GetItemRectMin(), ttMx = ImGui::GetItemRectMax();
                ImVec2 ttC = ImVec2(ttMn.x + 22.0f, (ttMn.y + ttMx.y) * 0.5f);
                fg->AddLine(ImVec2(ttC.x + 3.0f, ttC.y - 8.0f), ImVec2(ttC.x + 3.0f, ttC.y + 6.0f), IM_COL32(0,242,234,255), 2.5f);
                fg->AddCircleFilled(ImVec2(ttC.x - 1.0f, ttC.y + 6.0f), 4.0f, IM_COL32(255,255,255,255));
                fg->AddLine(ImVec2(ttC.x + 3.0f, ttC.y - 8.0f), ImVec2(ttC.x + 8.0f, ttC.y - 5.0f), IM_COL32(254,44,85,255), 2.5f);
                ImGui::PopStyleColor(3);
              }
              ImGui::Spacing();

              
              ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 45);
              float creditWidth = ImGui::CalcTextSize(xorstr_("Copyright (c) 2026 @TatsumiLoader ")).x;
              ImGui::SetCursorPosX((ImGui::GetWindowWidth() - creditWidth) * 0.5f);
              ImGui::TextDisabled(xorstr_("Copyright (c) 2026 @TatsumiLoader "));

              ImGui::EndChild();
              ImGui::EndTabItem();
            }


            if (ImGui::BeginTabItem(xorstr_("SETTINGS"))) {
              if (ImGui::CollapsingHeader(xorstr_("CONFIG"), ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Button(xorstr_("RE-SCAN GAME"), ImVec2(-1, 0))) AttachToGame();
                ImGui::Text(xorstr_("Status: %s"), is_attached ? xorstr_("READY (Auto-Attached)") : xorstr_("SEARCHING GAME..."));
                ImGui::Spacing();
                if (ImGui::Button(xorstr_("SAVE SETTINGS"), ImVec2(-1, 0))) SaveTatsumiSettings();
                if (ImGui::Button(xorstr_("LOAD SETTINGS"), ImVec2(-1, 0))) LoadTatsumiSettings();
              }
              if (ImGui::CollapsingHeader(xorstr_("SYSTEM"), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat(xorstr_("UI Scale"), &uiUserScale, 0.8f, 1.6f);
                static float opacity = 1.0f;
                {
                  const char* fpsItems[] = { "30 FPS", "60 FPS", "90 FPS", "120 FPS" };
                  const int fpsVals[] = { 30, 60, 90, 120 };
                  int fpsIdx = 1;
                  for (int i = 0; i < 4; i++) if (uiFpsCap == fpsVals[i]) fpsIdx = i;
                  if (ImGui::Combo(xorstr_("UI FPS Cap"), &fpsIdx, fpsItems, 4)) uiFpsCap = fpsVals[fpsIdx];
                }
                if (ImGui::SliderFloat(xorstr_("UI Opacity"), &opacity, 0.1f, 1.0f)) {
                  ImGui::GetStyle().Alpha = opacity;
                }
                ImGui::CustomCheckbox(xorstr_("AdGuard DNS"), &useAdGuardDns);
                if (ImGui::Button(xorstr_("EXIT CHEAT"), ImVec2(-1, 0))) {
                  main_thread_flag = false;
                }
                }
              ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
          }
          ImGui::End();
        }
    }
  }

  abs_ScreenX = (int)io.DisplaySize.x;
  abs_ScreenY = (int)io.DisplaySize.y;

  if (MinimapIcon) {
    DrawMinimapESP(ImGui::GetForegroundDrawList());
  }

  DrawMonster(ImGui::GetForegroundDrawList());

}
__attribute__((visibility("default"))) void *pid_monitor(void *) {
  while (main_thread_flag) {
    pid_t pid = 0;
    if (g_package_name == xorstr_("com.mobile.legends"))
      pid = pidof(xorstr_("com.mobile.legends:UnityKillsMe"));
    else if (g_package_name == xorstr_("com.mobile.legends.usa"))
      pid = pidof(xorstr_("com.mobile.legends.usa:UnityKillsMe"));
    else if (g_package_name == xorstr_("com.vng.mlbb"))
      pid = pidof(xorstr_("com.vng.mlbb:UnityKillsMe"));
    if (pid <= 0) {
      pid = pidof(xorstr_("com.mobile.legends:UnityKillsMe"));
      if (pid <= 0)
        pid = pidof(xorstr_("com.mobile.legends.usa:UnityKillsMe"));
      if (pid <= 0)
        pid = pidof(xorstr_("com.vng.mlbb:UnityKillsMe"));
    }
    if (pid == 0) {
      is_attached = false;
    } else if (!is_attached) {
      AttachToGame(); 
    }
    usleep(500000);
  }
  return nullptr;
}
__attribute__((visibility("default"))) void *game_worker(void *) {
  auto lastUpdate = std::chrono::steady_clock::now();
  while (main_thread_flag) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastUpdate)
            .count();
    static auto lastFast = lastUpdate;
      static auto lastMonster = lastUpdate;
      static auto lastLogin = lastUpdate - std::chrono::seconds(5);
      long msFast = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFast).count();
      long msMon = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMonster).count();
      long msLog = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastLogin).count();
      if (elapsedMs >= 1 && (msFast >= 5 || msMon >= 50 || msLog >= 5000)) {
      if (msMon >= 50) { MonsterRetribution(); lastMonster = currentTime; }
      if (msFast >= 5) {
      FastAutoRetri();
      FastAutoSpell(); 
      

      if (is_attached) {
        long a1 = ReadPtr(libbase + OFF_BM(BasePtr));
        if (a1) {
          long a2 = ReadPtr(a1 + OFF_BM(PtrChain1));
          if (a2) {
            long a32 = ReadPtr(a2); 
            uintptr_t selfAddr = ReadPtr(a32 + OFF_BM(LocalPlayerShow));

            
            bool swordNearby = false;
            if (autoSwordLing) {
              UpdateAutoSwordLing(selfAddr);
              
              extern bool g_SwordChasing;
              if (g_SwordChasing) swordNearby = true;
            }

            if (autoComboGusion) {
              UpdateGusionCombo(selfAddr, a32, libbase);
            }

            if (AutoAim::enabled && !swordNearby) {
              UpdateAutoAim(selfAddr, a32, libbase, (float)msFast / 1000.0f);
            }

          }
        }
      }
      lastFast = currentTime;
      }
      if (msLog >= 5000) {
      LoginServerData newData = ReadLoginServerInfo(libbase);
      if (newData.IsValid) {
        if (g_LoginServerCache.Nickname.empty()) g_LoginServerCache.Nickname = newData.Nickname;
        if (g_LoginServerCache.AccountId == 0) g_LoginServerCache.AccountId = newData.AccountId;
        g_LoginServerCache.ClientRealVersion = newData.ClientRealVersion;
        g_LoginServerCache.RegionId = newData.RegionId;
        
      }
      lastLogin = currentTime;
      }

      lastUpdate = currentTime;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return nullptr;
}

#include <sys/wait.h>

static int g_lockFd = -1;
static bool AcquireSingleInstance() {
  // Bunuh sisa binary lama (nama beda, aman dari self-kill).
  system("pkill -9 -f 'Starcool' >/dev/null 2>&1");
  // Bunuh duplikat diri sendiri (lewati PID sendiri).
  {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "pidof TatsumiLoader");
    FILE *fp = popen(cmd, "r");
    if (fp) {
      char buf[512] = {0};
      size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
      (void)n;
      pclose(fp);
      pid_t self = getpid();
      char *tok = strtok(buf, " \t\r\n");
      while (tok) {
        pid_t p = (pid_t)strtoul(tok, nullptr, 10);
        if (p > 0 && p != self) kill(p, SIGKILL);
        tok = strtok(nullptr, " \t\r\n");
      }
      usleep(300000);
    }
  }
  // Kunci file: instans kedua langsung keluar dengan pesan.
  g_lockFd = open("/data/local/tmp/TatsumiLoader.lock", O_RDWR | O_CREAT, 0600);
  if (g_lockFd < 0) return true;
  if (flock(g_lockFd, LOCK_EX | LOCK_NB) != 0) {
    printf("TatsumiLoader sudah jalan (lock aktif). Keluar.\n");
    fflush(stdout);
    return false;
  }
  return true;
}

__attribute__((visibility("default"))) int main(int argc, char *argv[]) {
  if (!AcquireSingleInstance()) return 1;
  pid_t watchdog_pid = fork();
  if (watchdog_pid < 0) {
    
  } else if (watchdog_pid > 0) {
    int status;
    waitpid(watchdog_pid, &status, 0);
    _exit(WIFEXITED(status) ? WEXITSTATUS(status) : 0);
  }


  
  

  is_root_mode = true;
  bool use_kernel = false;
  printf(xorstr_("==================================================\n"));
  printf(xorstr_("   EXTERNAL MLBB - TatsumiLoader\n"));
  printf(xorstr_("   Developer: @TatsumiLoader\n"));
  printf(xorstr_("==================================================\n"));
  printf(xorstr_("Select Driver Mode:\n"));
  printf(xorstr_("[1] NORMAL (ROOT)\n"));
  printf(xorstr_("[2] KERNEL (PRO)\n"));
  printf(xorstr_("--------------------------------------------------\n"));
  printf(xorstr_("Input: "));
  fflush(stdout);

  char input[8] = {0};
  if (fgets(input, sizeof(input), stdin)) {
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input, xorstr_("2")) == 0)
      use_kernel = true;
  }
  if (use_kernel) {
    g_driver = new KernelDriver();
    Memory::SetReadMode(Memory::ReadMode::KernelDriver);
  } else {
    Memory::SetReadMode(Memory::ReadMode::Userspace);
  }


  // TatsumiLoader modular boot: offset override -> launch ML -> tunggu PID -> menu.
  LoadOffsetOverrides(kOffsetOverrideFile);
  BootEnsureGame();

  

  if (useAdGuardDns) {
    system(xorstr_("settings put global private_dns_mode hostname"));
    system(xorstr_("settings put global private_dns_specifier dns.adguard.com"));
  }

  
  screen_config();
  ::abs_ScreenX = (displayInfo.height > displayInfo.width ? displayInfo.height
                                                          : displayInfo.width);
  ::abs_ScreenY = (displayInfo.height < displayInfo.width ? displayInfo.height
                                                          : displayInfo.width);
  ::native_window_screen_x = ::abs_ScreenX;
  ::native_window_screen_y = ::abs_ScreenY;
  if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true))
    return -1;
  Touch_Init(displayInfo.width, displayInfo.height, displayInfo.orientation,
             false);
  ImGui::StyleColorsDark();
  {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.00f, 0.00f, 0.67f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.80f, 0.00f, 0.00f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.00f, 0.00f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.00f, 0.00f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.80f, 0.00f, 0.00f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.80f, 0.00f, 0.00f, 0.86f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.00f, 0.00f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.60f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.00f, 0.00f, 0.35f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
  }
  ImGui::GetStyle().WindowRounding = 8.0f;
  if (AutoAim::joyCenterX == 0 && AutoAim::joyCenterY == 0) {
    AutoAim::joyCenterX = ::abs_ScreenX * 0.85f;
    AutoAim::joyCenterY = ::abs_ScreenY * 0.75f;
  }
  pthread_t pid_tid, worker_tid;
  pthread_create(&pid_tid, nullptr, pid_monitor, nullptr);
  pthread_create(&worker_tid, nullptr, game_worker, nullptr);

  pthread_detach(pid_tid);
  pthread_detach(worker_tid);
  g_RoomThreadShouldStop = false;
  g_RoomInfoThread = new std::thread(RoomInfoReaderThread);
  struct timespec lastTime;
  clock_gettime(CLOCK_MONOTONIC, &lastTime);
  LoadTatsumiSettings();
  struct timespec lastFrame = {0, 0};
  while (main_thread_flag) {
    struct timespec nowTs;
    clock_gettime(CLOCK_MONOTONIC, &nowTs);
    int fpsCap = uiFpsCap < 15 ? 15 : (uiFpsCap > 240 ? 240 : uiFpsCap);
    long frameBudget = 1000 / fpsCap;
    if (lastFrame.tv_sec != 0) {
      long fms = (nowTs.tv_sec - lastFrame.tv_sec) * 1000 +
                 (nowTs.tv_nsec - lastFrame.tv_nsec) / 1000000;
      if (fms < frameBudget) {
        usleep((frameBudget - fms) * 1000);
        continue;
      }
    }
    lastFrame = nowTs;
    drawBegin();
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    float deltaTime = (currentTime.tv_sec - lastTime.tv_sec) +
                      (currentTime.tv_nsec - lastTime.tv_nsec) / 1e9f;
    lastTime = currentTime;
    ImDrawList *draw = ImGui::GetForegroundDrawList();
    if (is_root_mode) {
      if (is_attached && libbase && Oneself) {
        
      }
      AutoAim::DrawFOV(draw);
      Joystick::Draw(draw);

      
      if (debugSwordID || autoSwordLing) {
        draw->AddCircle(ImVec2(lingSkill2X, lingSkill2Y), 35.0f, IM_COL32(255, 0, 0, 200), 20, 2.5f);
        draw->AddCircleFilled(ImVec2(lingSkill2X, lingSkill2Y), 10.0f, IM_COL32(255, 0, 0, 255));
        draw->AddText(ImVec2(lingSkill2X - 30, lingSkill2Y - 50), IM_COL32_WHITE, xorstr_("LING SKILL 2 POS"));
      }


      
      if (autoComboGusion) {
        ImU32 touchCol = IM_COL32(255, 0, 0, 180);
        draw->AddCircleFilled(ImVec2(gusionSkill1X, gusionSkill1Y), 15.0f, touchCol);
        draw->AddCircleFilled(ImVec2(gusionSkill2X, gusionSkill2Y), 15.0f, touchCol);
        draw->AddCircleFilled(ImVec2(gusionSkill3X, gusionSkill3Y), 15.0f, touchCol);
        draw->AddText(ImVec2(gusionSkill1X - 20, gusionSkill1Y - 40), IM_COL32_WHITE, xorstr_("G_S1"));
        draw->AddText(ImVec2(gusionSkill2X - 20, gusionSkill2Y - 40), IM_COL32_WHITE, xorstr_("G_S2"));
        draw->AddText(ImVec2(gusionSkill3X - 20, gusionSkill3Y - 40), IM_COL32_WHITE, xorstr_("G_S3"));
      }

      if (showRetriCircle && autoRetribution) {
        draw->AddCircleFilled(ImVec2(retriTouchX, retriTouchY), 18.0f, IM_COL32(255, 255, 255, 180), 16);
        draw->AddCircle(ImVec2(retriTouchX, retriTouchY), 18.0f, IM_COL32(0, 0, 0, 255), 16, 2.0f);
        draw->AddText(ImVec2(retriTouchX - 18, retriTouchY - 34), IM_COL32_WHITE, xorstr_("RETRI"));
        {
          char rdbg[96];
          snprintf(rdbg, sizeof(rdbg), "T:%d HP:%d/%d D:%.1f LV:%d", g_retriDbgID, g_retriDbgHP, g_retriDbgDmg, g_retriDbgDist, g_retriDbgLvl);
          draw->AddText(ImVec2(retriTouchX - 60, retriTouchY + 24), IM_COL32(0, 255, 150, 255), rdbg);
        }
      }
      if (autoSpellExecute) {
        draw->AddCircleFilled(ImVec2(spellX, spellY), 18.0f, IM_COL32(0, 200, 255, 180), 16);
        draw->AddCircle(ImVec2(spellX, spellY), 18.0f, IM_COL32(0, 0, 0, 255), 16, 2.0f);
        draw->AddText(ImVec2(spellX - 18, spellY - 34), IM_COL32_WHITE, xorstr_("SPELL"));
      }


    }
    Layout_tick_UI();

    drawEnd();
    usleep(1000);
  }
  MobaTouch::SendTouchUp();
 
  g_RoomThreadShouldStop = true;
  if (g_RoomInfoThread && g_RoomInfoThread->joinable()) {
    g_RoomInfoThread->join();
    delete g_RoomInfoThread;
    g_RoomInfoThread = nullptr;
  }



  if (useAdGuardDns)
    system(xorstr_("settings put global private_dns_mode off"));

  shutdown();
  Touch_Close();
  return 0;
}