#include "Define.h"
#include "Player.h"
#include "Config.h"
#ifndef TALENT_FUNCTIONS_H
#define TALENT_FUNCTIONS_H

int32 Boost_Costs;

void MindsearIsMyHero2()
{
	Boost_Costs = sConfigMgr->GetIntDefault("TemplateNPC.BoostCost", 250);
}


enum Spells
{
    Amani_War_Bear = 43688,
    Artisan_Riding = 34091,
    Cold_Weather_Flying = 54197,
    Teach_Learn_Talent_Specialization_Switches = 63680,
    Learn_a_Second_Talent_Specialization = 63624
};

enum templateSpells
{
    PLATE_MAIL = 750,
    MAIL = 8737
};

enum WeaponProficiencies
{
    BLOCK = 107,
    BOWS = 264,
    CROSSBOWS = 5011,
    DAGGERS = 1180,
    FIST_WEAPONS = 15590,
    GUNS = 266,
    ONE_H_AXES = 196,
    ONE_H_MACES = 198,
    ONE_H_SWORDS = 201,
    POLEARMS = 200,
    SHOOT = 5019,
    STAVES = 227,
    TWO_H_AXES = 197,
    TWO_H_MACES = 199,
    TWO_H_SWORDS = 202,
    WANDS = 5009,
    THROW_WAR = 2567
};

static void LearnWeaponSkills(Player* player)
{
    WeaponProficiencies wepSkills[] = {
        BLOCK, BOWS, CROSSBOWS, DAGGERS, FIST_WEAPONS, GUNS, ONE_H_AXES, ONE_H_MACES,
        ONE_H_SWORDS, POLEARMS, SHOOT, STAVES, TWO_H_AXES, TWO_H_MACES, TWO_H_SWORDS, WANDS, THROW_WAR
    };

    uint32 size = 17;

    for (uint32 i = 0; i < size; ++i)
        if (player->HasSpell(wepSkills[i]))
            continue;

    switch (player->GetClass())
    {
    case CLASS_WARRIOR:
        player->LearnSpell(THROW_WAR, false);
        player->LearnSpell(TWO_H_SWORDS, false);
        player->LearnSpell(TWO_H_MACES, false);
        player->LearnSpell(TWO_H_AXES, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(POLEARMS, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(ONE_H_AXES, false);
        player->LearnSpell(GUNS, false);
        player->LearnSpell(FIST_WEAPONS, false);
        player->LearnSpell(DAGGERS, false);
        player->LearnSpell(CROSSBOWS, false);
        player->LearnSpell(BOWS, false);
        player->LearnSpell(BLOCK, false);
        break;
    case CLASS_PRIEST:
        player->LearnSpell(WANDS, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(SHOOT, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(DAGGERS, false);
        break;
    case CLASS_PALADIN:
        player->LearnSpell(TWO_H_SWORDS, false);
        player->LearnSpell(TWO_H_MACES, false);
        player->LearnSpell(TWO_H_AXES, false);
        player->LearnSpell(POLEARMS, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(ONE_H_AXES, false);
        player->LearnSpell(BLOCK, false);
        break;
    case CLASS_ROGUE:
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(ONE_H_AXES, false);
        player->LearnSpell(GUNS, false);
        player->LearnSpell(FIST_WEAPONS, false);
        player->LearnSpell(DAGGERS, false);
        player->LearnSpell(CROSSBOWS, false);
        player->LearnSpell(BOWS, false);
        break;
    case CLASS_DEATH_KNIGHT:
        player->LearnSpell(TWO_H_SWORDS, false);
        player->LearnSpell(TWO_H_MACES, false);
        player->LearnSpell(TWO_H_AXES, false);
        player->LearnSpell(POLEARMS, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(ONE_H_AXES, false);
        break;
    case CLASS_MAGE:
        player->LearnSpell(WANDS, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(SHOOT, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(DAGGERS, false);
        break;
    case CLASS_SHAMAN:
        player->LearnSpell(TWO_H_MACES, false);
        player->LearnSpell(TWO_H_AXES, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(ONE_H_AXES, false);
        player->LearnSpell(FIST_WEAPONS, false);
        player->LearnSpell(DAGGERS, false);
        player->LearnSpell(BLOCK, false);
        break;
    case CLASS_HUNTER:
        player->LearnSpell(THROW_WAR, false);
        player->LearnSpell(TWO_H_SWORDS, false);
        player->LearnSpell(TWO_H_AXES, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(POLEARMS, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(ONE_H_AXES, false);
        player->LearnSpell(GUNS, false);
        player->LearnSpell(FIST_WEAPONS, false);
        player->LearnSpell(DAGGERS, false);
        player->LearnSpell(CROSSBOWS, false);
        player->LearnSpell(BOWS, false);
        break;
    case CLASS_DRUID:
        player->LearnSpell(TWO_H_MACES, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(POLEARMS, false);
        player->LearnSpell(ONE_H_MACES, false);
        player->LearnSpell(FIST_WEAPONS, false);
        player->LearnSpell(DAGGERS, false);
        break;
    case CLASS_WARLOCK:
        player->LearnSpell(WANDS, false);
        player->LearnSpell(STAVES, false);
        player->LearnSpell(SHOOT, false);
        player->LearnSpell(ONE_H_SWORDS, false);
        player->LearnSpell(DAGGERS, false);
        break;
    default:
        break;
    }
    player->UpdateWeaponsSkillsToMaxSkillsForLevel();

}

struct TalentTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint32         talentId;
};

struct GlyphTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint8          slot;
    uint32         glyph;
};

struct HumanGearTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint8          pos;
    uint32         itemEntry;
    uint32         enchant;
    uint32         socket1;
    uint32         socket2;
    uint32         socket3;
    uint32         bonusEnchant;
    uint32         prismaticEnchant;
};

struct AllianceGearTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint8          pos;
    uint32         itemEntry;
    uint32         enchant;
    uint32         socket1;
    uint32         socket2;
    uint32         socket3;
    uint32         bonusEnchant;
    uint32         prismaticEnchant;
};

struct HordeGearTemplate
{
    std::string    playerClass;
    std::string    playerSpec;
    uint8          pos;
    uint32         itemEntry;
    uint32         enchant;
    uint32         socket1;
    uint32         socket2;
    uint32         socket3;
    uint32         bonusEnchant;
    uint32         prismaticEnchant;
};

typedef std::vector<HumanGearTemplate*> HumanGearContainer;
typedef std::vector<AllianceGearTemplate*> AllianceGearContainer;
typedef std::vector<HordeGearTemplate*> HordeGearContainer;

typedef std::vector<TalentTemplate*> TalentContainer;
typedef std::vector<GlyphTemplate*> GlyphContainer;

class sTemplateNPC
{
public:
    static sTemplateNPC* instance()
    {
        static sTemplateNPC* instance = new sTemplateNPC();
        return instance;
    }
    void LoadTalentsContainer();
    void LoadGlyphsContainer();

    void LoadHumanGearContainer();
    void LoadAllianceGearContainer();
    void LoadHordeGearContainer();

    void ApplyGlyph(Player* player, uint8 slot, uint32 glyphID);
    void RemoveAllGlyphs(Player* player);
    void ApplyBonus(Player* player, Item* item, EnchantmentSlot slot, uint32 bonusEntry);

    bool OverwriteTemplate(Player* /*player*/, std::string& /*playerSpecStr*/);
    void ExtractGearTemplateToDB(Player* /*player*/, std::string& /*playerSpecStr*/);
    void ExtractTalentTemplateToDB(Player* /*player*/, std::string& /*playerSpecStr*/);
    void ExtractGlyphsTemplateToDB(Player* /*player*/, std::string& /*playerSpecStr*/);
    bool CanEquipTemplate(Player* /*player*/, std::string& /*playerSpecStr*/);

    std::string GetClassString(Player* /*player*/);
    std::string sTalentsSpec;

    void LearnTemplateTalents(Player* /*player*/);
    void LearnTemplateGlyphs(Player* /*player*/);
    void EquipTemplateGear(Player* /*player*/);

    void LearnPlateMailSpells(Player* /*player*/);

    GlyphContainer m_GlyphContainer;
    TalentContainer m_TalentContainer;

    HumanGearContainer m_HumanGearContainer;
    AllianceGearContainer m_AllianceGearContainer;
    HordeGearContainer m_HordeGearContainer;
};
#define sTemplateNpcMgr sTemplateNPC::instance()
#endif
