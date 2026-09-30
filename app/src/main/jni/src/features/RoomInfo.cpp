#include "RoomInfo.h"
#include <algorithm>
#include <sstream>
#include "icon/Rank/RankIcons_loader.h"
#include "icon/Spell/SpellIcons_loader.h"
#include "Decoder64.h" // Untuk GetHeroTexture
#include "oxorany.h"
#include <unordered_map>

extern std::unordered_map<std::string, std::string> countryMap;

static int GetRankValue(const CachedRoomPlayer &p) {
    return p.RankLevel;
}

static void RenderPlayerTable(const char *label, const std::vector<CachedRoomPlayer> &players) {
    if (players.empty()) {
        ImGui::TextDisabled("No players in this camp.");
        return;
    }

    constexpr ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable(label, 9, flags, ImVec2(0, 0))) {
        ImGui::TableSetupColumn("Nickname", ImGuiTableColumnFlags_WidthStretch, 0.22f);
        ImGui::TableSetupColumn("Hero", ImGuiTableColumnFlags_WidthFixed, 35.0f);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch, 0.13f);
        ImGui::TableSetupColumn("Rank", ImGuiTableColumnFlags_WidthStretch, 0.18f);
        ImGui::TableSetupColumn("Spell", ImGuiTableColumnFlags_WidthStretch, 0.10f);
        ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_WidthStretch, 0.08f);
        ImGui::TableSetupColumn("Country", ImGuiTableColumnFlags_WidthStretch, 0.12f);
        ImGui::TableSetupColumn("Winrate", ImGuiTableColumnFlags_WidthStretch, 0.13f);
        ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_WidthStretch, 0.15f);

        ImGui::TableHeadersRow();

        std::vector<CachedRoomPlayer> sortedPlayers = players;
        std::sort(sortedPlayers.begin(), sortedPlayers.end(),
                  [](const CachedRoomPlayer &a, const CachedRoomPlayer &b) {
                    return GetRankValue(a) > GetRankValue(b);
                  });

        for (const auto &player : sortedPlayers) {
            if (!player.IsValid) continue;
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 46.0f);

            // 1. Nickname
            if (ImGui::TableSetColumnIndex(0)) {
                ImGui::Text("%s", player.Name.empty() ? "-" : player.Name.c_str());
            }

            // 2. Hero Icon
            if (ImGui::TableSetColumnIndex(1)) {
                ImTextureID tex = GetHeroTexture(player.HeroId);
                if (tex) ImGui::Image(tex, ImVec2(34, 34));
                else ImGui::Text("?");
            }

            // 3. ID
            if (ImGui::TableSetColumnIndex(2)) {
                ImGui::Text("%s", player.UserID.c_str());
            }

            // 4. Rank Style
            if (ImGui::TableSetColumnIndex(3)) {
                std::string rText = player.Rank;
                if (rText.find("Mythic") != std::string::npos) {
                    std::string rankType = "mythic";
                    if (rText.find("Honor") != std::string::npos) rankType = "honor";
                    else if (rText.find("Glory") != std::string::npos) rankType = "glory";
                    else if (rText.find("Immortal") != std::string::npos) rankType = "immortal";

                    ImTextureID rIcon = GetRankIconTexture(rankType);
                    if (rIcon) ImGui::Image(rIcon, ImVec2(34, 34));

                    size_t lastStar = rText.find_last_of(' ');
                    if (lastStar != std::string::npos) {
                        ImGui::SameLine(0, 4);
                        ImGui::Text("%s", rText.substr(lastStar + 1).c_str());
                    }
                } else {
                    std::stringstream ss(rText);
                    std::string name, romawi, aster, starCount;
                    ss >> name >> romawi >> aster >> starCount;
                    std::string lowerName = name;
                    for(auto &c : lowerName) c = (char)tolower(c);

                    ImTextureID rIcon = GetRankIconTexture(lowerName);
                    if (rIcon) ImGui::Image(rIcon, ImVec2(34, 34));
                    ImGui::SameLine(0, 8);
                    ImGui::Text("%s", romawi.c_str());

                    ImTextureID sIcon = GetRankIconTexture("star");
                    if (sIcon) {
                        ImGui::SameLine(0, 12);
                        float currentY = ImGui::GetCursorPosY();
                        ImGui::SetCursorPosY(currentY + 4.0f);
                        ImGui::Image(sIcon, ImVec2(22, 22));
                        ImGui::SetCursorPosY(currentY);
                        ImGui::SameLine(0, 4);
                        ImGui::Text("%s", starCount.c_str());
                    }
                }
            }

            // 5. Spell
            if (ImGui::TableSetColumnIndex(4)) {
                ImTextureID sTex = GetSpellTexture(player.SpellId);
                if (sTex) ImGui::Image(sTex, ImVec2(34, 34));
                else ImGui::TextDisabled("-");
            }

            // 6. Role
            if (ImGui::TableSetColumnIndex(5)) {
                ImGui::Text("%s", player.Role.c_str());
            }

            // 7. Country
            if (ImGui::TableSetColumnIndex(6)) {
                std::string lowerCode = player.Country;
                for (auto &ch : lowerCode) ch = (char)tolower(ch);
                auto it = countryMap.find(lowerCode);
                if (it != countryMap.end()) ImGui::TextColored(ImVec4(0.0f, 0.7f, 1.0f, 1.0f), "%s", it->second.c_str());
                else ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.8f, 1.0f), "%s", player.Country.c_str());
            }

            // 8. Winrate
            if (ImGui::TableSetColumnIndex(7)) {
                if (player.WinRate < 0) ImGui::TextDisabled("-");
                else ImGui::Text("%.1f%% (%d/%d)", player.WinRate, player.Wins, player.RoadGames);
            }

            // 9. Added (Fav Heroes)
            if (ImGui::TableSetColumnIndex(8)) {
                if (player.AddHeroIds.empty()) ImGui::TextDisabled("-");
                else {
                    int count = 0;
                    for (int heroId : player.AddHeroIds) {
                        ImTextureID tex = GetHeroTexture(heroId);
                        if (tex) {
                            ImGui::Image(tex, ImVec2(30, 30));
                            if (++count >= 3) break;
                            ImGui::SameLine(0, 2);
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }
}

void RenderRoomPlayerInfoImGui() {
    static int currentTab = 0;
    float windowWidth = ImGui::GetWindowWidth();

    ImGui::Checkbox(oxorany("Auto DOD (Risk)"), &enableDOD);
    ImGui::Separator();

    if (ImGui::Button("Blue Team", ImVec2(windowWidth * 0.45f, 58))) currentTab = 0;
    ImGui::SameLine();
    if (ImGui::Button("Red Team", ImVec2(windowWidth * 0.45f, 58))) currentTab = 1;

    ImGui::Spacing();

    if (ImGui::BeginChild("##TableScrollArea", ImVec2(0, ImGui::GetContentRegionAvail().y - 8), true)) {
        std::lock_guard<std::mutex> lock(g_RoomDataMutex);
        if (!g_CachedRoomData.HasData) {
            ImGui::Text("Waiting for Lobby data...");
        } else {
            int myCamp = g_CachedRoomData.LocalPlayerCamp;
            bool isTeamA = (myCamp == 1);
            if (currentTab == 0) RenderPlayerTable("##TeamTable", isTeamA ? g_CachedRoomData.TeamA : g_CachedRoomData.TeamB);
            else RenderPlayerTable("##EnemyTable", isTeamA ? g_CachedRoomData.TeamB : g_CachedRoomData.TeamA);
        }
        ImGui::EndChild();
    }
}
