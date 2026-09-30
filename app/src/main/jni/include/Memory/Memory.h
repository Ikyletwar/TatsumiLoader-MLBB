#pragma once
// Made By @AKNoryx28
// Dual-Mode Edition: Direct KernelDriver Integration (Header-Only)

#include <unistd.h>
#include <string>
#include <vector>
#include <inttypes.h>
#include <cstdint>
#include "kernel.h"

#ifdef __LP64__
typedef uint64_t APTR;
#else
typedef uint32_t APTR;
#endif

typedef int Prot;
enum Prot_ {
    Prot_NONE = 0,
    Prot_READ = 1 << 1,
    Prot_WRITE = 1 << 2,
    Prot_EXEC = 1 << 3,
    Prot_PRIV = 1 << 4,
};

inline Prot parsePerms(const char* perms_str) {
    Prot perms = Prot_NONE;
    if (perms_str[0] == 'r') perms |= Prot_READ;
    if (perms_str[1] == 'w') perms |= Prot_WRITE;
    if (perms_str[2] == 'x') perms |= Prot_EXEC;
    if (perms_str[3] == 'p') perms |= Prot_PRIV;
    return perms;
}

struct PMap {
    pid_t pid;
    unsigned long long startAddress;
    unsigned long long endAddress;
    size_t length;
    Prot perms;
    unsigned long long offset;
    std::string dev;
    unsigned long inode;
    std::string pathname;
};

// ============================================================================
// FORWARD DECLARATION - Required for inline DoRead/DoWrite
// ============================================================================
// Declare pvm function (defined in Memory.cpp)
bool pvm(pid_t pid, void* address, void* buffer, size_t size, bool isWrite);

// ============================================================================
// DUAL MODE SUPPORT - Direct KernelDriver Integration
// ============================================================================

namespace Memory {
    enum class ReadMode {
        Userspace,
        KernelDriver
    };
    
    inline ReadMode g_read_mode = ReadMode::Userspace;
    inline pid_t g_pid = -1;
    
    inline bool IsKernelDriverReady() {
        return (g_driver != nullptr);
    }
    
    inline bool KernelRead(uintptr_t addr, void* buf, size_t size) {
        if (!g_driver || !buf || size == 0 || addr == 0) return false;
        return g_driver->read_memory(addr, buf, size);
    }
    
    inline bool KernelWrite(uintptr_t addr, void* buf, size_t size) {
        if (!g_driver || !buf || size == 0 || addr == 0) return false;
        return g_driver->write_memory(addr, buf, size);
    }
    
    inline uintptr_t KernelGetBase(const char* name) {
        if (!g_driver || !name) return 0;
        return g_driver->get_module_base(const_cast<char*>(name));
    }
    
    inline bool DoRead(uintptr_t addr, void* buf, size_t size, pid_t pid = g_pid) {
        if (g_read_mode == ReadMode::KernelDriver && IsKernelDriverReady()) {
            return KernelRead(addr, buf, size);
        }
        return pvm(pid, reinterpret_cast<void*>(addr), buf, size, false);
    }
    
    inline bool DoWrite(uintptr_t addr, void* buf, size_t size, pid_t pid = g_pid) {
        if (g_read_mode == ReadMode::KernelDriver && IsKernelDriverReady()) {
            return KernelWrite(addr, buf, size);
        }
        return pvm(pid, reinterpret_cast<void*>(addr), buf, size, true);
    }
        pid_t FindPid(const char* procName);
    uintptr_t GetBase(const char *libName, pid_t pid = g_pid);
    uintptr_t GetEnd(const char *libName, pid_t pid = g_pid);
    uintptr_t GetBaseInSplit(const char *splitName, const char *libName, pid_t pid = g_pid);
    uintptr_t GetEndInSplit(const char *splitName, const char *libName, pid_t pid = g_pid);
    std::vector<PMap> GetMaps(const char *name, pid_t pid = g_pid);
    
    int ProcessRead(uintptr_t address, void *out, size_t size, pid_t pid = g_pid);
    int ProcessRead(void *address, void *out, size_t size, pid_t pid = g_pid);
    int ProcessWrite(void *address, void *data, size_t size, pid_t pid = g_pid);
    
    bool WriteIO(uintptr_t adresss, void *write_buffer, size_t size, pid_t pid = g_pid);
    bool ReadIO(uintptr_t adresss, void *read_buffer, size_t size, pid_t pid = g_pid);
    void CleanIO();
    
    uintptr_t FindPattern(uintptr_t start, size_t length, const unsigned char* pattern, const char* mask);
    
    std::string Exec(const char *cmd);
    bool IsInvalidAddress(uintptr_t address);
    bool IsInvalidAddress(void *address);
    
    template<typename T> inline T Read(uintptr_t addr);
    template<typename T> T Read(const std::vector<uintptr_t> &address);
    template<typename T> inline T *ReadArray(uintptr_t address, unsigned int size);
    inline uintptr_t ReadPtr(uintptr_t address);
    inline uintptr_t ReadPtr(const std::vector<uintptr_t> &address);
    template<typename T> inline bool Write(uintptr_t addr, T data);
    
    inline void SetReadMode(ReadMode mode) {
        if (mode == ReadMode::KernelDriver && !IsKernelDriverReady()) {
            g_read_mode = ReadMode::Userspace;
        } else {
            g_read_mode = mode;
        }
    }
    
    inline ReadMode GetReadMode() { return g_read_mode; }
    inline bool IsKernelModeActive() { 
        return g_read_mode == ReadMode::KernelDriver && IsKernelDriverReady(); 
    }
}

// ============================================================================
// TEMPLATE IMPLEMENTATIONS
// ============================================================================

template<typename T>
T Memory::Read(uintptr_t addr) {
    T data{};
    if (Memory::DoRead(addr, &data, sizeof(T), Memory::g_pid)) {        return data;
    }
    return {};
}

template<typename T>
T Memory::Read(const std::vector<uintptr_t> &address) {
    uintptr_t tmpAddress = 0;
    T result{};
    for (size_t i = 0; i < address.size(); ++i) {
        auto adr = address[i];
        bool success = false;
        if (i == address.size() - 1) {
            success = Memory::DoRead(tmpAddress + adr, &result, sizeof(T));
        } else {
            success = Memory::DoRead(tmpAddress + adr, &tmpAddress, sizeof(uintptr_t));
        }
        if (!success) return {};
    }
    return result;
}

template<typename T>
bool Memory::Write(uintptr_t addr, T data) {
    return Memory::DoWrite(addr, &data, sizeof(T), Memory::g_pid);
}

template<typename T>
T *Memory::ReadArray(uintptr_t address, unsigned int size) {
    static T data[256];
    if (size > 256) size = 256;
    if (Memory::DoRead(address, data, sizeof(T) * size, Memory::g_pid)) {
        return data;
    }
    return nullptr;
}

// ============================================================================
// INLINE FUNCTION IMPLEMENTATIONS (Header-Only)
// ============================================================================

inline uintptr_t Memory::ReadPtr(uintptr_t address) {
    uintptr_t data = 0;
    if (Memory::DoRead(address, &data, sizeof(uintptr_t), Memory::g_pid)) {
        return data;
    }
    return 0;
}

inline uintptr_t Memory::ReadPtr(const std::vector<uintptr_t> &address) {
    uintptr_t tmpAddress = 0;
    for (const auto adr : address) {
        if (!Memory::DoRead(tmpAddress + adr, &tmpAddress, sizeof(uintptr_t), Memory::g_pid)) {
            return 0;
        }
    }
    return tmpAddress;
}

