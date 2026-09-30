#pragma once
// TatsumiLoader - override offset tanpa recompile.
// Cara update offset baru:
//   1. Dapat offset baru (misal BattleManager.BasePtr = 0x1234567)
//   2. Tulis ke /data/local/tmp/TatsumiLoader.offsets di HP:
//        BattleManager.BasePtr=0x635b290
//        ShowEntity.Hp=0x1a4
//   3. Restart loader. Nilai file menang atas bawaan.
// Format: KEY=0xHEX, # = komentar, baris kosong diabaikan.
// Daftar KEY valid lihat tabel di bawah (mirror Offsets.h).
//
// NOTE: variabel di Offsets.h sengaja `inline size_t` (bukan const)
// agar bisa di-patch saat startup dan konsisten antar TU.

#include <cctype>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>

#include "config/Offsets.h"

inline bool LoadOffsetOverrides(
    const char *path = "/data/local/tmp/TatsumiLoader.offsets") {
  std::unordered_map<std::string, size_t *> table = {
      {"BattleManager.BasePtr", &Offsets::BattleManager::BasePtr},
      {"BattleManager.LocalPlayerShow", &Offsets::BattleManager::LocalPlayerShow},
      {"BattleManager.ShowPlayers", &Offsets::BattleManager::ShowPlayers},
      {"BattleManager.ShowMonsters", &Offsets::BattleManager::ShowMonsters},
      {"BattleManager.m_RunBullets", &Offsets::BattleManager::m_RunBullets},
      {"BattleManager.PtrChain1", &Offsets::BattleManager::PtrChain1},
      {"BattleManager.ListDataOffset", &Offsets::BattleManager::ListDataOffset},
      {"BattleManager.ListCountOffset", &Offsets::BattleManager::ListCountOffset},
      {"BattleManager.ListArrayOffset", &Offsets::BattleManager::ListArrayOffset},
      {"ShowEntity.iType", &Offsets::ShowEntity::iType},
      {"ShowEntity.Hp", &Offsets::ShowEntity::Hp},
      {"ShowEntity.HpMax", &Offsets::ShowEntity::HpMax},
      {"ShowEntity.bDeath", &Offsets::ShowEntity::bDeath},
      {"ShowEntity.bSameCampType", &Offsets::ShowEntity::bSameCampType},
      {"ShowEntity.vCachePosition", &Offsets::ShowEntity::vCachePosition},
      {"ShowEntity.HeroID", &Offsets::ShowEntity::HeroID},
      {"ShowEntity.m_uGuid", &Offsets::ShowEntity::m_uGuid},
      {"ShowEntity.Level", &Offsets::ShowEntity::Level},
      {"ShowEntity.SummonSkillId", &Offsets::ShowEntity::SummonSkillId},
      {"ShowEntity.m_ShowCoolDownComp", &Offsets::ShowEntity::m_ShowCoolDownComp},
      {"ShowPlayer.m_EntityCampType", &Offsets::ShowPlayer::m_EntityCampType},
      {"Camera.MainCameraPtr", &Offsets::Camera::MainCameraPtr},
      {"Camera.SmoothFollow", &Offsets::Camera::SmoothFollow},
      {"Camera.ViewMatrix", &Offsets::Camera::ViewMatrix},
      {"Camera.v2", &Offsets::Camera::v2},
      {"SystemData.BasePtr", &Offsets::SystemData::BasePtr},
      {"SystemData.m_RoomPlayerInfo", &Offsets::SystemData::m_RoomPlayerInfo},
      {"SystemData.PtrChain1", &Offsets::SystemData::PtrChain1},
      {"SystemData.m_uiID", &Offsets::SystemData::m_uiID},
      {"SystemData.m_bOfflineManagerRun",
       &Offsets::SystemData::m_bOfflineManagerRun},
      {"RoomData.heroid", &Offsets::RoomData::heroid},
      {"RoomData.uiRankLevel", &Offsets::RoomData::uiRankLevel},
      {"RoomData.iCamp", &Offsets::RoomData::iCamp},
      {"LogicBattleManager.BasePtr", &Offsets::LogicBattleManager::BasePtr},
      {"LogicBattleManager.PtrChain1", &Offsets::LogicBattleManager::PtrChain1},
      {"ShowCoolDownComp._dicCD", &Offsets::ShowCoolDownComp::_dicCD},
      {"CoolDownData.uiCoolTime", &Offsets::CoolDownData::uiCoolTime},
      {"CoolDownData.uiStartTime", &Offsets::CoolDownData::uiStartTime},
      {"Bullet.m_Id", &Offsets::Bullet::m_Id},
      {"Bullet._bulletPos", &Offsets::Bullet::_bulletPos},
  };
  std::ifstream in(path);
  if (!in.is_open())
    return false;
  int applied = 0;
  std::string line;
  while (std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    size_t s = line.find_first_not_of(" \t");
    if (s == std::string::npos || line[s] == '#')
      continue;
    size_t eq = line.find('=', s);
    if (eq == std::string::npos)
      continue;
    std::string key = line.substr(s, eq - s);
    while (!key.empty() && (key.back() == ' ' || key.back() == '\t'))
      key.pop_back();
    std::string val = line.substr(eq + 1);
    try {
      size_t v = std::stoul(val, nullptr, 0);
      auto it = table.find(key);
      if (it != table.end()) {
        *it->second = v;
        applied++;
      } else {
        printf("[offsets] unknown key: %s\n", key.c_str());
      }
    } catch (...) {
    }
  }
  printf("[offsets] override %s: %d applied\n", path, applied);
  return applied > 0;
}
