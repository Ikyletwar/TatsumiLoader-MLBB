#include "SkillIcons_loader.h"
#include "SkillIcons.h"
#include <unordered_map>
#include <GLES3/gl3.h>
#include "stb_image.h"

// Key menggunakan long long agar heroId dan skillIndex besar (seperti 20010) tidak bentrok
static std::unordered_map<long long, ImTextureID> g_SkillIconCache;

ImTextureID GetSkillIconTexture(int heroId, int skillIndex) {
    long long key = ((long long)heroId << 32) | (unsigned int)skillIndex;
    if (g_SkillIconCache.count(key)) return g_SkillIconCache[key];

    const unsigned char* data = nullptr;
    unsigned int len = 0;

    // 1. Cari kecocokan tepat (termasuk index 11-13 untuk Morph hasil script Python)
    for (int i = 0; i < TOTAL_SKILL_ICONS; i++) {
        if (skill_icon_registry[i].heroId == heroId && skill_icon_registry[i].skillIdx == skillIndex) {
            data = skill_icon_registry[i].data;
            len = skill_icon_registry[i].len;
            break;
        }
    }

    // 2. Fallback: Jika tidak ketemu dan index > 10, coba cari skill dasarnya (mod 10)
    if (!data && skillIndex > 10) {
        int fallbackIdx = skillIndex % 10;
        for (int i = 0; i < TOTAL_SKILL_ICONS; i++) {
            if (skill_icon_registry[i].heroId == heroId && skill_icon_registry[i].skillIdx == fallbackIdx) {
                data = skill_icon_registry[i].data;
                len = skill_icon_registry[i].len;
                break;
            }
        }
    }

    if (data && len > 0) {
        int w, h, ch;
        unsigned char* pixels = stbi_load_from_memory(data, (int)len, &w, &h, &ch, 4);
        if (pixels) {
            GLuint gtex;
            glGenTextures(1, &gtex);
            glBindTexture(GL_TEXTURE_2D, gtex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            stbi_image_free(pixels);
            ImTextureID res = (ImTextureID)(intptr_t)gtex;
            g_SkillIconCache[key] = res;
            return res;
        }
    }
    return nullptr;
}

void ClearSkillIconCache() {
    for (auto& pair : g_SkillIconCache) {
        GLuint tex = (GLuint)(intptr_t)pair.second;
        glDeleteTextures(1, &tex);
    }
    g_SkillIconCache.clear();
}
