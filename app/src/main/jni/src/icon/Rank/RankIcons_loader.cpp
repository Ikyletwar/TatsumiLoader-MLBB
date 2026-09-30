#include "RankIcons_loader.h"
#include "RankIcons.h"
#include <unordered_map>
#include <GLES3/gl3.h>
#include "stb_image.h"

static std::unordered_map<std::string, ImTextureID> g_RankIconCache;

ImTextureID GetRankIconTexture(const std::string& name) {
    if (g_RankIconCache.count(name)) return g_RankIconCache[name];

    const unsigned char* data = nullptr;
    size_t len = 0;

    if (name == "warrior") { data = warrior_png; len = sizeof(warrior_png); }
    else if (name == "elite") { data = elite_png; len = sizeof(elite_png); }
    else if (name == "master") { data = master_png; len = sizeof(master_png); }
    else if (name == "grandmaster") { data = grandmaster_png; len = sizeof(grandmaster_png); }
    else if (name == "epic") { data = epic_png; len = sizeof(epic_png); }
    else if (name == "legend") { data = legend_png; len = sizeof(legend_png); }
    else if (name == "mythic") { data = mythic_png; len = sizeof(mythic_png); }
    else if (name == "honor") { data = honor_png; len = sizeof(honor_png); }
    else if (name == "glory") { data = glory_png; len = sizeof(glory_png); }
    else if (name == "immortal") { data = immortal_png; len = sizeof(immortal_png); }
    else if (name == "star") { data = star_png; len = sizeof(star_png); }

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
            g_RankIconCache[name] = res;
            return res;
        }
    }
    return nullptr;
}

void ClearRankIconCache() {
    for (auto& pair : g_RankIconCache) {
        GLuint tex = (GLuint)(intptr_t)pair.second;
        glDeleteTextures(1, &tex);
    }
    g_RankIconCache.clear();
}
