#include "CFBGData.h"
#include "Player.h"
#include "Item.h"
#include "WorldSession.h"
#include "World.h"
#include "MiscPackets.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "Battleground.h"
#include "CharacterCache.h"
#include "DBCStores.h"

void CFBGData::SetCFBGData()
{
    if (!sWorld->getBoolConfig(CONFIG_CFBG_ENABLED))
        return;

    player->SetByteValue(UNIT_FIELD_BYTES_0, 0, NativeTeam() ? GetORace() : GetFRace());
    player->SetFaction(NativeTeam() ? GetOFaction() : GetFFaction());
    ReplaceRacials();
    SetRaceDisplayID();

    // Calling this in BattleGround::AddPlayer fixes scoreboard
    sCharacterCache->UpdateCharacterData(player->GetGUID(), player->GetName(), {}, player->GetRace());
}

void CFBGData::SetRaceDisplayID()
{
    if (!sWorld->getBoolConfig(CONFIG_CFBG_ENABLED))
        return;

    PlayerInfo const* info = sObjectMgr->GetPlayerInfo(player->GetRace(), player->GetClass());

    if (!info)
        return;

    player->SetObjectScale(1.f);

    uint8 gender = player->GetGender();
    switch (gender)
    {
    case GENDER_FEMALE:
        player->SetDisplayId(info->displayId_f);
        player->SetNativeDisplayId(info->displayId_f);
        break;
    case GENDER_MALE:
        player->SetDisplayId(info->displayId_m);
        player->SetNativeDisplayId(info->displayId_m);
        break;
    default:
        return;
    }
}

void CFBGData::ReplaceRacials()
{
    if (!sWorld->getBoolConfig(CONFIG_CFBG_ENABLED))
        return;

    ReplaceItems();

    std::unordered_map<uint16, bool> skills;

    auto updateskills = [&] (uint32 prace, uint32 pclass, bool add) -> void
    {
        if (auto pinfo = sObjectMgr->GetPlayerInfo(prace, pclass))
        {
            for (auto& i : pinfo->skills)
            {
                if (SkillLineEntry const* skill = sSkillLineStore.LookupEntry(i.SkillId))
                {
                    if ((skill->CategoryID == SKILL_CATEGORY_LANGUAGES &&
                         sWorld->getBoolConfig(CONFIG_CFBG_REPLACELANGUAGES)) ||
                        (skill->CategoryID != SKILL_CATEGORY_LANGUAGES &&
                         sWorld->getBoolConfig(CONFIG_CFBG_REPLACERACIALS)))
                    {
                        skills[i.SkillId] = add;
                    }
                }
            }
        }
    };

    for (uint8 i = 0; i < MAX_RACES; ++i)
        updateskills(i, player->GetClass(), false);

    updateskills(NativeTeam() ? GetORace() : GetFRace(), player->GetClass(), true);

    for (std::pair<uint16 const, bool>& skillinfo : skills)
    {
        if (skillinfo.second)
            player->LearnDefaultSkill(skillinfo.first, 0);
        else
            player->SetSkill(skillinfo.first, 0, 0, 0);
    }

    player->SendUpdateToPlayer(player);
}

void CFBGData::ReplaceItems()
{
    if (!sWorld->getBoolConfig(CONFIG_CFBG_ENABLED))
        return;

    for (std::pair<uint32 const, uint32>& itempair : sObjectMgr->FactionChangeItems)
    {
        uint32 item_alliance = itempair.first;
        uint32 item_horde = itempair.second;

        auto replaceitem = [&] (uint32 sourceitem, uint32 destinationitem) -> void
        {
            while (Item* item = player->GetItemByEntry(sourceitem))
            {
                item->SetEntry(destinationitem);
                item->SetState(ITEM_CHANGED);
            }
        };

        if (player->GetTeam() == ALLIANCE)
            replaceitem(item_horde, item_alliance);
        else
            replaceitem(item_alliance, item_horde);
    }

}

void CFBGData::InitializeCFData()
{
    m_oRace = player->GetRace();
    player->SetFactionForRace(player->GetRace());
    m_oFaction = player->GetFaction();
    m_oTeam = Player::TeamForRace(m_oRace);

    m_fRace = 0;

    while (m_fRace == 0)
        for (uint8 i = 0; i < MAX_RACES; ++i)
            if (sObjectMgr->GetPlayerInfo(i, player->GetClass()) && Player::TeamForRace(i) != GetOTeam() && urand(0, 5) == 0)
                m_fRace = i;

    ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(m_fRace);
    m_fFaction = rEntry ? rEntry->FactionID : 0;
    m_fTeam = Player::TeamForRace(m_fRace);
}
