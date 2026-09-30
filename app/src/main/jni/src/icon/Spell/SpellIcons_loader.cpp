#include "SpellIcons_loader.h"
#include "SpellIcons.h"
#include <unordered_map>
#include <GLES3/gl3.h>
#include "stb_image.h"

static std::unordered_map<int, ImTextureID> g_LocalSpellCache;

ImTextureID GetSpellTexture(int spellId) {
    if (spellId <= 0) return nullptr;
    if (g_LocalSpellCache.count(spellId)) return g_LocalSpellCache[spellId];

    const unsigned char* data = nullptr;
    size_t len = 0;

    // Normalisasi ID ke 5-digit berakhiran 0 (contoh: 20021 -> 20020)
    int id = spellId;
    if (id > 20000) id = (id / 10) * 10;
    else if (id > 2000) id = id * 10;
    else if (id > 0 && id < 100) id = 20000 + (id * 10);

    switch(id) {
        case 20150: data = execute_png; len = sizeof(execute_png); break;
        case 20020: data = retribution_png; len = sizeof(retribution_png); break;
        case 20030: data = inspire_png; len = sizeof(inspire_png); break;
        case 20040: data = sprint_png; len = sizeof(sprint_png); break;
        case 20050: data = revitalize_png; len = sizeof(revitalize_png); break;
        case 20060: data = aegis_png; len = sizeof(aegis_png); break;
        case 20070: data = petrify_png; len = sizeof(petrify_png); break;
        case 20140: data = flameshot_png; len = sizeof(flameshot_png); break;
        case 20100: data = flicker_png; len = sizeof(flicker_png); break;
        case 20190: data = vengeance_png; len = sizeof(vengeance_png); break;
        case 20080: data = purify_png; len = sizeof(purify_png); break;
        case 20160: data = arrival_png; len = sizeof(arrival_png); break;
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
            g_LocalSpellCache[spellId] = res;
            return res;
        }
    }
    return nullptr;
}

void ClearSpellIconCache() {
    for (auto& pair : g_LocalSpellCache) {
        GLuint tex = (GLuint)(intptr_t)pair.second;
        glDeleteTextures(1, &tex);
    }
    g_LocalSpellCache.clear();
}
