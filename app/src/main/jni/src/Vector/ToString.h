#pragma once


std::string strRank[] = {
        "Warrior III * 1",
        "Warrior III * 2",
        "Warrior III * 3",
        "Warrior II * 0",
        "Warrior II * 1",
        "Warrior II * 2",
        "Warrior II * 3",
        "Warrior I * 0",
        "Warrior I * 1",
        "Warrior I * 2",
        "Warrior I * 3",
        "Elite III * 0",
        "Elite III * 1",
        "Elite III * 2",
        "Elite III * 3",
        "Elite III * 4",
        "Elite II * 0",
        "Elite II * 1",
        "Elite II * 2",
        "Elite II * 3",
        "Elite II * 4",
        "Elite I * 0",
        "Elite I * 1",
        "Elite I * 2",
        "Elite I * 3",
        "Elite I * 4",
        "Master IV * 0",
        "Master IV * 1",
        "Master IV * 2",
        "Master IV * 3",
        "Master IV * 4",
        "Master III * 0",
        "Master III * 1",
        "Master III * 2",
        "Master III * 3",
        "Master III * 4",
        "Master II * 0",
        "Master II * 1",
        "Master II * 2",
        "Master II * 3",
        "Master II * 4",
        "Master I * 0",
        "Master I * 1",
        "Master I * 2",
        "Master I * 3",
        "Master I * 4",
        "Grandmaster V * 0",
        "Grandmaster V * 1",
        "Grandmaster V * 2",
        "Grandmaster V * 3",
        "Grandmaster V * 4",
        "Grandmaster V * 5",
        "Grandmaster IV * 0",
        "Grandmaster IV * 1",
        "Grandmaster IV * 2",
        "Grandmaster IV * 3",
        "Grandmaster IV * 4",
        "Grandmaster IV * 5",
        "Grandmaster III * 0",
        "Grandmaster III * 1",
        "Grandmaster III * 2",
        "Grandmaster III * 3",
        "Grandmaster III * 4",
        "Grandmaster III * 5",
        "Grandmaster II * 0",
        "Grandmaster II * 1",
        "Grandmaster II * 2",
        "Grandmaster II * 3",
        "Grandmaster II * 4",
        "Grandmaster II * 5",
        "Grandmaster I * 0",
        "Grandmaster I * 1",
        "Grandmaster I * 2",
        "Grandmaster I * 3",
        "Grandmaster I * 4",
        "Grandmaster I * 5",
        "Epic V * 0",
        "Epic V * 1",
        "Epic V * 2",
        "Epic V * 3",
        "Epic V * 4",
        "Epic V * 5",
        "Epic IV * 0",
        "Epic IV * 1",
        "Epic IV * 2",
        "Epic IV * 3",
        "Epic IV * 4",
        "Epic IV * 5",
        "Epic III * 0",
        "Epic III * 1",
        "Epic III * 2",
        "Epic III * 3",
        "Epic III * 4",
        "Epic III * 5",
        "Epic II * 0",
        "Epic II * 1",
        "Epic II * 2",
        "Epic II * 3",
        "Epic II * 4",
        "Epic II * 5",
        "Epic I * 0",
        "Epic I * 1",
        "Epic I * 2",
        "Epic I * 3",
        "Epic I * 4",
        "Epic I * 5",
        "Legend V * 0",
        "Legend V * 1",
        "Legend V * 2",
        "Legend V * 3",
        "Legend V * 4",
        "Legend V * 5",
        "Legend IV * 0",
        "Legend IV * 1",
        "Legend IV * 2",
        "Legend IV * 3",
        "Legend IV * 4",
        "Legend IV * 5",
        "Legend III * 0",
        "Legend III * 1",
        "Legend III * 2",
        "Legend III * 3",
        "Legend III * 4",
        "Legend III * 5",
        "Legend II * 0",
        "Legend II * 1",
        "Legend II * 2",
        "Legend II * 3",
        "Legend II * 4",
        "Legend II * 5",
        "Legend I * 0",
        "Legend I * 1",
        "Legend I * 2",
        "Legend I * 3",
        "Legend I * 4",
        "Legend I * 5"
};

std::string RankToString(int uiRankLevel, int iMythPoint) {
    std::string nameMythic = "Mythic * ";
    if (uiRankLevel > 136) {
        int Star = uiRankLevel - 136;
        if ((int) Star > 24) nameMythic = "Mythical Honor * ";
        if ((int) Star > 49) nameMythic = "Mythical Glory * ";
        if ((int) Star > 99) nameMythic = "Mythical Immortal * ";
        return nameMythic + std::to_string(Star);
    } else {
        return strRank[uiRankLevel];
    }
}

#include <string>
#include <utility>

std::pair<int, std::string> strHero[] = {
    {0, "-"},
    {1, "Miya"},
    {2, "Balmond"},
    {3, "Saber"},
    {4, "Alice"},
    {5, "Nana"},
    {6, "Tigreal"},
    {7, "Alucard"},
    {8, "Karina"},
    {9, "Akai"},
    {10, "Franco"},
    {11, "Bane"},
    {12, "Bruno"},
    {13, "Clint"},
    {14, "Rafaela"},
    {15, "Eudora"},
    {16, "Zilong"},
    {17, "Fanny"},
    {18, "Layla"},
    {19, "Minotaur"},
    {20, "Lolita"},
    {21, "Hayabusa"},
    {22, "Freya"},
    {23, "Gord"},
    {24, "Natalia"},
    {25, "Kagura"},
    {26, "Chou"},
    {27, "Sun"},
    {28, "Alpha"},
    {29, "Ruby"},
    {30, "Yi Sun-shin"},
    {31, "Moskov"},
    {32, "Johnson"},
    {33, "Cyclops"},
    {34, "Estes"},
    {35, "Hilda"},
    {36, "Aurora"},
    {37, "Lapu-Lapu"},
    {38, "Vexana"},
    {39, "Roger"},
    {40, "Karrie"},
    {41, "Gatotkaca"},
    {42, "Harley"},
    {43, "Irithel"},
    {44, "Grock"},
    {45, "Argus"},    {46, "Odette"},
    {47, "Lancelot"},
    {48, "Diggie"},
    {49, "Hylos"},
    {50, "Zhask"},
    {51, "Helcurt"},
    {52, "Pharsa"},
    {53, "Lesley"},
    {54, "Jawhead"},
    {55, "Angela"},
    {56, "Gusion"},
    {57, "Valir"},
    {58, "Martis"},
    {59, "Uranus"},
    {60, "Hanabi"},
    {61, "Chang'e"},
    {62, "Kaja"},
    {63, "Selena"},
    {64, "Aldous"},
    {65, "Claude"},
    {66, "Vale"},
    {67, "Leomord"},
    {68, "Lunox"},
    {69, "Hanzo"},
    {70, "Belerick"},
    {71, "Kimmy"},
    {72, "Thamuz"},
    {73, "Harith"},
    {74, "Minsitthar"},
    {75, "Kadita"},
    {76, "Faramis"},
    {77, "Badang"},
    {78, "Khufra"},
    {79, "Granger"},
    {80, "Guinevere"},
    {81, "Esmeralda"},
    {82, "Terizla"},
    {83, "X.Borg"},
    {84, "Ling"},
    {85, "Dyrroth"},
    {86, "Lylia"},
    {87, "Baxia"},
    {88, "Masha"},
    {89, "Wanwan"},
    {90, "Silvanna"},
    {91, "Cecilion"},
    {92, "Carmilla"},
    {93, "Atlas"},
    {94, "Popol and Kupa"},
    {95, "Yu Zhong"},    {96, "Luo Yi"},
    {97, "Benedetta"},
    {98, "Khaleed"},
    {99, "Barats"},
    {100, "Brody"},
    {101, "Yve"},
    {102, "Mathilda"},
    {103, "Paquito"},
    {104, "Gloo"},
    {105, "Beatrix"},
    {106, "Phoveus"},
    {107, "Natan"},
    {108, "Aulus"},
    {109, "Aamon"},
    {110, "Valentina"},
    {111, "Edith"},
    {112, "Floryn"},
    {113, "Yin"},
    {114, "Melissa"},
    {115, "Xavier"},
    {116, "Julian"},
    {117, "Fredrinn"},
    {118, "Joy"},
    {119, "Novaria"},
    {120, "Arlott"},
    {121, "Ixia"},
    {122, "Nolan"},
    {123, "Cici"},
    {124, "Chip"},
    {125, "Zhuxin"},
    {126, "Suyou"},
    {127, "Lukas"},
    {128, "Kalea"},
    {129, "Zetian"},
    {130, "Obsidia"},
    {131, "Sora"},
    {132, "Marcel"}
};

constexpr size_t HERO_COUNT = sizeof(strHero) / sizeof(strHero[0]);

std::string HeroToString(int heroid) {
    for (size_t i = 0; i < HERO_COUNT; ++i) {
        if (strHero[i].first == heroid) {
            return strHero[i].second;
        }
    }
    return "Hero ID: " + std::to_string(heroid);
}
std::string CountryToString(int countryId) {
    switch (countryId) {
    case 0: return "-";
    // Southeast Asia
    case 62: return "Indonesia";
    case 60: return "Malaysia";
    case 63: return "Philippines";
    case 65: return "Singapore";
    case 66: return "Thailand";
    case 95: return "Myanmar";
    case 855: return "Cambodia";
    case 84: return "Vietnam";
    case 856: return "Laos";
    case 673: return "Brunei";
    case 670: return "Timor-Leste";
    // East Asia
    case 81: return "Japan";
    case 82: return "South Korea";
    case 86: return "China";
    case 886: return "Taiwan";
    case 852: return "Hong Kong";
    case 853: return "Macau";
    case 976: return "Mongolia";
    // South Asia
    case 91: return "India";
    case 880: return "Bangladesh";
    case 977: return "Nepal";
    case 94: return "Sri Lanka";
    case 92: return "Pakistan";
    case 93: return "Afghanistan";
    // Middle East
    case 966: return "Saudi Arabia";
    case 971: return "UAE";
    case 964: return "Iraq";
    case 98: return "Iran";
    case 90: return "Turkey";
    case 972: return "Israel";
    case 962: return "Jordan";
    case 961: return "Lebanon";
    case 968: return "Oman";
    case 974: return "Qatar";
    case 965: return "Kuwait";
    case 973: return "Bahrain";
    case 967: return "Yemen";
    case 963: return "Syria";
    // Africa
    case 20: return "Egypt";
    case 212: return "Morocco";
    case 213: return "Algeria";
    case 216: return "Tunisia";
    case 218: return "Libya";
    case 234: return "Nigeria";
    case 27: return "South Africa";
    // Americas
    case 1: return "USA/Canada";
    case 52: return "Mexico";
    case 55: return "Brazil";
    case 54: return "Argentina";
    case 56: return "Chile";
    case 57: return "Colombia";
    case 51: return "Peru";
    case 58: return "Venezuela";
    case 593: return "Ecuador";
    case 591: return "Bolivia";
    case 595: return "Paraguay";
    case 598: return "Uruguay";
    case 502: return "Guatemala";
    case 503: return "El Salvador";
    case 504: return "Honduras";
    case 507: return "Panama";
    case 506: return "Costa Rica";
    // Europe
    case 7: return "Russia";
    case 44: return "UK";
    case 49: return "Germany";
    case 33: return "France";
    case 34: return "Spain";
    case 39: return "Italy";
    case 31: return "Netherlands";
    case 32: return "Belgium";
    case 41: return "Switzerland";
    case 43: return "Austria";
    case 48: return "Poland";
    case 46: return "Sweden";
    case 47: return "Norway";
    case 45: return "Denmark";
    case 358: return "Finland";
    case 351: return "Portugal";
    case 30: return "Greece";
    case 36: return "Hungary";
    case 40: return "Romania";
    case 380: return "Ukraine";
    case 375: return "Belarus";
    case 420: return "Czech Republic";
    case 421: return "Slovakia";
    case 385: return "Croatia";
    case 381: return "Serbia";
    case 359: return "Bulgaria";
    // Oceania
    case 61: return "Australia";
    case 64: return "New Zealand";
    default: return "ID:" + std::to_string(countryId);
    }
}

std::string SpellToString(int summonSkillId) {
    std::string strSpell;
    switch(summonSkillId) {
	case 0:
		strSpell += "-";
        break;
    case 20150:
        strSpell += "Execute";
        break;
	case 20020:
        strSpell += "Retribution";
        break;
	case 20030:
        strSpell += "Inspire";
        break;
	case 20040:
        strSpell += "Sprint";
        break;
	case 20050:
        strSpell += "Revitalize";
        break;
	case 20060:
         strSpell += "Aegis";
        break;
	case 20070:
        strSpell += "Petrify";
        break;
	case 20080:
        strSpell += "Purify";
        break;
	case 20140:
        strSpell += "Flameshot";
        break;
	case 20100:
        strSpell += "Flicker";
        break;
	case 20160:
        strSpell += "Arrival";
        break;
	case 20190:
        strSpell += "Vengeance";
        break;
	default:
	    strSpell += to_string(summonSkillId);
	}
	return strSpell;
}

std::string MonsterToString(int hero_id) {
    std::string strMonster;
    switch(hero_id) {
	case 2002:
        strMonster += "Lord";
        break;
	case 2003:
        strMonster += "Turtle";
        break;
	case 2004:
        strMonster += "Fiend";
        break;
    case 2005:
        strMonster += "Serpent";
        break;
	case 2006:
        strMonster += "Scaled Lizard";
        break;
	case 2008:
        strMonster += "Crammer";
        break;
	case 2009:
        strMonster += "Rockursa";
        break;
	case 2011:
		strMonster += "Crab";
		break;
    case 2012:
        strMonster += "Serpent kids";
        break;
    case 2013:
        strMonster += "Crab";
        break;
    case 2056:
        strMonster += "Lithowanderer";
        break;
	case 2059:
		strMonster += "Crammer";
		break;
	case 2072:
		strMonster += "Lithowanderer";
		break;
    default:
        strMonster += ""/*to_string(m_id)*/;
    }
    return strMonster;
}

int ListSummonSkillId[] = {
    20150,
    20020,
    20030,
    20040,
    20050,
    20060,
    20070,
    20080,
    20140,
    20100,
    20160,
    20190
};

bool SpellIdExist(int iValue) {
	return std::find(std::begin(ListSummonSkillId), std::end(ListSummonSkillId), iValue) != std::end(ListSummonSkillId);
}

enum SPELL_CAST_RESULT {
	SPELL_CAST_NOCARE = -100,
	SPELL_CAST_NO_BASE_SPELL = -99,
	SPELL_CAST_NO_OWNER = -98,
	SPELL_CAST_IN_COOLDOWN = -97,
	SPELL_CAST_ALLOC_MEM = -96,
	SPELL_CAST_BULLET_NO_SPEED = -95,
	SPELL_CAST_NO_ATT_SPEED = -94,
	SPELL_CAST_NO_BASE_EFFECT = -93,
	SPELL_CAST_WRONG_STATUS = -92,
	SPELL_CAST_WRONG_STATUS_NOT_ATTACK = -91,
	SPELL_CAST_OUT_DIS = -90,
	SPELL_CAST_DEATH = -89,
	SPELL_CAST_TRANSFER_ERROR = -88,
	SPELL_CAST_CONTROL_NO_ADDTION = -87,
	SPELL_CAST_TRANSER_SINGLE = -86,
	SPELL_CAST_NO_ENOUGH_HP = -85,
	SPELL_CAST_NO_ENOUGH_MP = -84,
	SPELL_CAST_NO_ENOUGH_XP = -83,
	SPELL_CAST_NO_UNIT = -82,
	SPELL_CAST_WRONG_POS = -81,
	SPELL_CAST_LOOP_CAST = -80,
	SPELL_CAST_NO_SPELL = -79,
	SPELL_CAST_NO_ENOUGH_LEVEL = -78,
	SPELL_CAST_NO_LOCKTARGET = -77,
	SPELL_LOCK_CATCHMOVE = -76,
	SPELL_SEVER_BUSY = -75,
	SPELL_MOVE_NO_LOCKTARGET = -74,
	SPELL_CAST_WRONG_BEHAVIOR = -73,
	SPELL_CAST_WRONG_LOCKSKILLID = -72,
	SPELL_CAST_WRONG_LOCKSKILLBUFFID = -71,
	SPELL_Cheack_Target_Inevitable = -70,
	SPELL_Cheack_OneFrameSkillMaxNum = -69,
	SPELL_IN_AUTO_ATTACK_AI = -68,
	SPELL_Cheack_TranSkillID_Inevitable = -67,
	SPELL_NO_FIGHTER = -66,
	SPELL_FAIL_NOT_QUICK_MOVE = -65,
	SPELL_CAST_ALREADY_LEARNED = -64,
	SPELL_Cheack_Xuli_Inevitable = -50,
	SPELL_Cheack_SkillOvelap = -51,
	SPELL_IsStoreSkill = -52,
	SPELL_IsStoreSkill_Inevitable = -53,
	SPELL_CAST_SUCCESS = 0,
};
