//Offsets
namespace SystemData {
    uintptr_t m_bOfflineManagerRun = 0x32c;
}

//Tambahkan di roominfo thread
static bool lastState = false;
bool currentState = gReq.memory.DOD; // (gReq.Memory.DOD) Ganti checkbox imgui
if (currentState != lastState) {
    Write<bool>(system_data + Offsets::SystemData::m_bOfflineManagerRun, currentState);
    lastState = currentState;
}