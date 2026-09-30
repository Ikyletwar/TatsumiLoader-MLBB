#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "imgui.h"

struct CachedRoomPlayer {
  uintptr_t Address;
  std::string Name;
  std::string UserID;
  std::string Squad;
  std::string Rank;
  std::string Hero;
  int HeroId;
  int SpellId;
  std::string Spell;
  std::string Party;
  std::string Role;
  std::vector<int> AddHeroIds;
  float WinRate;
  int Wins;
  int RoadGames;
  int Camp;
  int RankLevel;
  int ZoneId;
  int SkinID;
  int CountryId;
  std::string Country;
  bool IsRoomLeader;
  bool IsValid;

  CachedRoomPlayer()
      : Address(0), HeroId(0), Camp(0), RankLevel(0), ZoneId(0), SkinID(0), CountryId(0),
        WinRate(-1.0f), Wins(0), RoadGames(0), IsRoomLeader(false),
        IsValid(false) {}
};

struct CachedRoomData {
  std::vector<CachedRoomPlayer> TeamA;
  std::vector<CachedRoomPlayer> TeamB;
  uintptr_t GroupID;
  int LocalPlayerCamp;
  bool HasData;
  std::time_t LastUpdate;

  CachedRoomData()
      : GroupID(0), LocalPlayerCamp(1), HasData(false), LastUpdate(0) {}
};

// Fungsi yang akan dipanggil di main.cpp
void RenderRoomPlayerInfoImGui();

// Data yang diisi oleh thread di main.cpp
extern CachedRoomData g_CachedRoomData;
extern std::mutex g_RoomDataMutex;
extern bool enableDOD;
