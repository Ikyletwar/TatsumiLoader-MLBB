#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <string>
#include <vector>

#include "../Memory/Memory.h"

using namespace Memory;




struct monoArray {
    void *klass;
    void *obj;
    void *bounds;
    size_t max_length;
    uintptr_t m_Items;
};

struct monoList {
    void *klass;
    void *obj;
    uintptr_t monoArray;
    int pSize;
};




template <typename TKey, typename TValue> struct monoDictionaryEntry {
    int hashCode;
    int next;
    TKey key;
    TValue value;
};

template <typename TKey, typename TValue> struct monoDictionary {
    void *klass;
    void *obj;
    int32_t *buckets;
    uintptr_t arrayEntries;
    int count;
    int version;
    int freeList;
    int freeCount;
    uintptr_t comparer;
    uintptr_t keys;
    uintptr_t values;
};




template <typename T>
inline std::vector<T> get_mono_list_vector(monoList array) {
    std::vector<T> output(array.pSize);
    if (array.monoArray == 0 || array.pSize <= 0)
        return output;

    auto address = array.monoArray + offsetof(monoArray, m_Items);
    for (int i = 0; i < array.pSize; i++) {
        output[i] = Memory::Read<T>(address + i * sizeof(T));
    }
    return output;
}

template <typename TKey, typename TValue>
inline std::vector<monoDictionaryEntry<TKey, TValue>>
get_mono_dictionary_vector(monoDictionary<TKey, TValue> dict) {
    std::vector<monoDictionaryEntry<TKey, TValue>> output(dict.count);
    if (dict.arrayEntries == 0 || dict.count <= 0)
        return output;

    auto address = dict.arrayEntries + offsetof(monoArray, m_Items);
    for (int i = 0; i < dict.count; i++) {
        output[i] = Memory::Read<monoDictionaryEntry<TKey, TValue>>(
                address + i * sizeof(monoDictionaryEntry<TKey, TValue>));
    }
    return output;
}




namespace Offsets {

    // Bundle offset ini. Naikkan tiap update MLBB + catat di README_MODULAR.md.
    // Override runtime via /data/local/tmp/TatsumiLoader.offsets (lihat OffsetOverrides.h).
    inline const char *kOffsetBundleVersion = "2026-09-30-tatsumi-1";
    inline const char *kMlbbTarget = "MLBB 2.x (il2cpp/csharp)";






    namespace BattleManager {
                inline size_t BasePtr = 0x635b290;
                inline size_t LocalPlayerShow = 0x48;
                inline size_t ShowPlayers = 0x70;
                inline size_t ShowMonsters = 0x78;
                inline size_t m_RunBullets = 0x38;
                inline size_t PtrChain1 = 0xa8;
                inline size_t ListDataOffset = 0x10;
                inline size_t ListCountOffset = 0x18;
                inline size_t ListArrayOffset = 0x20;
    } 




    namespace ShowEntity {
        inline size_t iType = 0x78;
        inline size_t Hp = 0x1a4;
        inline size_t HpMax = 0x1a8;
        inline size_t bDeath = 0xc5;
        inline size_t bSameCampType = 0x2a9;
        inline size_t vCachePosition = 0x28c;
        inline size_t HeroID = 0x18c;
        inline size_t m_uGuid = 0x188;
        inline size_t Level = 0x190;
        inline size_t SummonSkillId = 0x984;
        inline size_t m_ShowCoolDownComp = 0xf8;
    } 




    namespace ShowPlayer {
        inline size_t m_EntityCampType = 0xd0;
    } 




    namespace Camera {
        inline size_t MainCameraPtr = 0x635b620;
        inline size_t CameraData = 0xa8;
        inline size_t CameraData2 = 0x08;
        inline size_t SmoothFollow = 0x10;
        inline size_t ViewMatrix = 0x5C;
        inline size_t v2 = 0xc8;
    } 




    namespace String {
        inline size_t LengthOffset = 0x10;
        inline size_t BufferOffset = 0x14;
    } 




    namespace SystemData {
        inline size_t BasePtr = 0x635cb88;
        inline size_t m_RoomPlayerInfo = 0x348;
        inline size_t PtrChain1 = 0xa8;
        inline size_t m_uiID = 0x358;
        inline size_t m_bOfflineManagerRun = 0x36e;
    } 




    namespace RoomData {
        inline size_t lUid = 0x20;
        inline size_t iCamp = 0x30;
        inline size_t _sName = 0x40;
        inline size_t heroid = 0x4c;
        inline size_t heroskin = 0x50;
        inline size_t country = 0x5c;
        inline size_t uiZoneId = 0x60;
        inline size_t summonSkillId = 0x64;
        inline size_t sThisLoginCountry = 0xd0;
        inline size_t _steamSimpleName = 0x100;
        inline size_t uiRankLevel = 0x120;
        inline size_t iRoad = 0x138;
        inline size_t iMythPoint = 0x1cc;
        inline size_t bRoomLeader = 0x32c;
        inline size_t mapHeroBattleNum = 0x3e8;
        inline size_t vCurSeasonRealRoadInfo = 0x3f0;
    } 




    namespace LogicBattleManager {
        inline size_t BasePtr = 0x635b5d8;
        inline size_t PtrChain1 = 0xa8;
        inline size_t m_uiFrameTime = 0x13c;
    } 




    namespace ShowCoolDownComp {
        inline size_t _dicCD = 0x18;
    }




    namespace CoolDownData {
        inline size_t uiCoolTime = 0x14;
        inline size_t uiStartTime = 0x1C;
    } 




    namespace ShowSkillData {
        inline size_t config = 0x10;
    } 




    namespace Bullet {
        inline size_t m_Id = 0x3c;
        inline size_t _bulletPos = 0xfc;
    } 

} 




#define OFF_STR(field) Offsets::String::field
#define OFF_CAM(field) Offsets::Camera::field
#define OFF_BM(field) Offsets::BattleManager::field
#define OFF_SE(field) Offsets::ShowEntity::field
#define OFF_SP(field) Offsets::ShowPlayer::field
#define OFF_SD(field) Offsets::SystemData::field
#define OFF_RD(field) Offsets::RoomData::field
#define OFF_BLC(field) Offsets::LogicBattleManager::field
#define OFF_SCC(field) Offsets::ShowCoolDownComp::field
#define OFF_CDD(field) Offsets::CoolDownData::field
#define OFF_BUL(field) Offsets::Bullet::field
