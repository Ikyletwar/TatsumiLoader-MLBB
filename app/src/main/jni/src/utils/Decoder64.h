#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <GLES3/gl3.h>
#include "imgui.h"
#include "icon/HeroIcons.h"

#include "stb_image.h"

inline std::vector<unsigned char> DecodeBase64(const std::string& input) {
    static const std::string b64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[(unsigned char)b64_chars[i]] = i;

    std::vector<unsigned char> out;
    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (isspace(c) || c == '=') continue;
        if (T[c] == -1) continue;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static std::unordered_map<int, ImTextureID> g_HeroTextureCache;
static std::unordered_map<int, ImTextureID> g_RankTextureCache;

inline ImTextureID CreateTextureFromMemory(const unsigned char* data, int len) {
    if (!data || len <= 0) return nullptr;
    int w, h, ch;
    unsigned char* pixels = stbi_load_from_memory(data, len, &w, &h, &ch, 4);
    if (pixels) {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
        return (ImTextureID)(intptr_t)tex;
    }
    return nullptr;
}

inline ImTextureID CreateTextureFromBase64(const std::string& base64Str) {
    if (base64Str.empty()) return nullptr;

    std::vector<unsigned char> data = DecodeBase64(base64Str);
    int w, h, ch;
    unsigned char* pixels = stbi_load_from_memory(data.data(), (int)data.size(), &w, &h, &ch, 4);
    
    if (pixels) {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
        
        return (ImTextureID)(intptr_t)tex;
    }
    return nullptr;
}

inline ImTextureID GetHeroTexture(int heroId) {
    if (g_HeroTextureCache.count(heroId)) return g_HeroTextureCache[heroId];

    auto it = g_HeroIconsBase64.find(heroId);
    if (it != g_HeroIconsBase64.end()) {
        ImTextureID tex = CreateTextureFromBase64(it->second);
        if (tex) g_HeroTextureCache[heroId] = tex;
        return tex;
    }
    return nullptr;
}

inline void ClearHeroTextureCache() {
    for (auto& pair : g_HeroTextureCache) {
        GLuint tex = (GLuint)(intptr_t)pair.second;
        glDeleteTextures(1, &tex);
    }
    g_HeroTextureCache.clear();
}

inline void ClearRankTextureCache() {
    for (auto& pair : g_RankTextureCache) {
        GLuint tex = (GLuint)(intptr_t)pair.second;
        glDeleteTextures(1, &tex);
    }
    g_RankTextureCache.clear();
}

inline void ClearAllCaches() {
    ClearHeroTextureCache();
    ClearRankTextureCache();
}
