#include "TemplateNPC.h"
#include "Item.h"
#include "DBCStores.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "CharacterDatabase.h"
#include "Player.h"
#include "WorldSession.h"
#include "ScriptedGossip.h"
#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GossipDef.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "RBAC.h"
#include "Config.h"
#include "World.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
//#include "DBCStores.h"
#include "ObjectMgr.h"

#define BOOSTCOST "Buy level 80 and learn ALL spells - "
#define GOLDTEXT " Gold."

using namespace Trinity::ChatCommands;


void sTemplateNPC::LearnPlateMailSpells(Player* player)
{
    switch (player->GetClass())
    {
    case CLASS_WARRIOR:
    case CLASS_PALADIN:
    case CLASS_DEATH_KNIGHT:
        player->LearnSpell(PLATE_MAIL, true);
        break;
    case CLASS_SHAMAN:
    case CLASS_HUNTER:
        player->LearnSpell(MAIL, true);
        break;
    default:
        break;
    }
}

void sTemplateNPC::ApplyBonus(Player* player, Item* item, EnchantmentSlot slot, uint32 bonusEntry)
{
    if (!item)
        return;

    if (!bonusEntry || bonusEntry == 0)
        return;

    player->ApplyEnchantment(item, slot, false);
    item->SetEnchantment(slot, bonusEntry, 0, 0);
    player->ApplyEnchantment(item, slot, true);
}

void sTemplateNPC::ApplyGlyph(Player* player, uint8 slot, uint32 glyphID)
{
    if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyphID))
    {
        if (uint32 oldGlyph = player->GetGlyph(slot))
        {
            player->RemoveAurasDueToSpell(sGlyphPropertiesStore.LookupEntry(oldGlyph)->SpellID);
            player->SetGlyph(slot, 0);
        }
        player->CastSpell(player, gp->SpellID, true);
        player->SetGlyph(slot, glyphID);
    }
}

void sTemplateNPC::RemoveAllGlyphs(Player* player)
{
    for (uint8 i = 0; i < MAX_GLYPH_SLOT_INDEX; ++i)
    {
        if (uint32 glyph = player->GetGlyph(i))
        {
            if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyph))
            {
                if (GlyphSlotEntry const* gs = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(i)))
                {
                    player->RemoveAurasDueToSpell(sGlyphPropertiesStore.LookupEntry(glyph)->SpellID);
                    player->SetGlyph(i, 0);
                    player->SendTalentsInfoData(false); // this is somewhat an in-game glyph realtime update (apply/remove)
                }
            }
        }
    }
}

void sTemplateNPC::LearnTemplateTalents(Player* player)
{
    for (TalentContainer::const_iterator itr = m_TalentContainer.begin(); itr != m_TalentContainer.end(); ++itr)
    {
        if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
        {
            player->LearnSpell((*itr)->talentId, false);
            player->AddTalent((*itr)->talentId, player->GetActiveSpec(), true);
        }
    }
    player->SetFreeTalentPoints(0);
    player->SendTalentsInfoData(false);
}

void sTemplateNPC::LearnTemplateGlyphs(Player* player)
{
    for (GlyphContainer::const_iterator itr = m_GlyphContainer.begin(); itr != m_GlyphContainer.end(); ++itr)
    {
        if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            ApplyGlyph(player, (*itr)->slot, (*itr)->glyph);
    }
    player->SendTalentsInfoData(false);
}

void sTemplateNPC::EquipTemplateGear(Player* player)
{
    if (player->GetRace() == RACE_HUMAN)
    {
        for (HumanGearContainer::const_iterator itr = m_HumanGearContainer.begin(); itr != m_HumanGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
    else if (player->GetTeam() == ALLIANCE && player->GetRace() != RACE_HUMAN)
    {
        for (AllianceGearContainer::const_iterator itr = m_AllianceGearContainer.begin(); itr != m_AllianceGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
    else if (player->GetTeam() == HORDE)
    {
        for (HordeGearContainer::const_iterator itr = m_HordeGearContainer.begin(); itr != m_HordeGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
}

void sTemplateNPC::LoadTalentsContainer()
{
    for (TalentContainer::const_iterator itr = m_TalentContainer.begin(); itr != m_TalentContainer.end(); ++itr)
        delete *itr;

    m_TalentContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, talentId FROM template_npc_talents;");

    if (!result)
    {
        TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded 0 talent templates. DB table `template_npc_talents` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        TalentTemplate* pTalent = new TalentTemplate;

        pTalent->playerClass = fields[0].GetString();
        pTalent->playerSpec = fields[1].GetString();
        pTalent->talentId = fields[2].GetUInt32();

        m_TalentContainer.push_back(pTalent);
        ++count;
    } while (result->NextRow());
    TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded %u talent templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadGlyphsContainer()
{
    for (GlyphContainer::const_iterator itr = m_GlyphContainer.begin(); itr != m_GlyphContainer.end(); ++itr)
        delete *itr;

    m_GlyphContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, slot, glyph FROM template_npc_glyphs;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded 0 glyph templates. DB table `template_npc_glyphs` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GlyphTemplate* pGlyph = new GlyphTemplate;

        pGlyph->playerClass = fields[0].GetString();
        pGlyph->playerSpec = fields[1].GetString();
        pGlyph->slot = fields[2].GetUInt8();
        pGlyph->glyph = fields[3].GetUInt32();

        m_GlyphContainer.push_back(pGlyph);
        ++count;
    } while (result->NextRow());
    TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded %u glyph templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadHumanGearContainer()
{
    for (HumanGearContainer::const_iterator itr = m_HumanGearContainer.begin(); itr != m_HumanGearContainer.end(); ++itr)
        delete *itr;

    m_HumanGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_human;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_human` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        HumanGearTemplate* pItem = new HumanGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_HumanGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded %u gear templates for Humans in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadAllianceGearContainer()
{
    for (AllianceGearContainer::const_iterator itr = m_AllianceGearContainer.begin(); itr != m_AllianceGearContainer.end(); ++itr)
        delete *itr;

    m_AllianceGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_alliance;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_alliance` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        AllianceGearTemplate* pItem = new AllianceGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_AllianceGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded %u gear templates for Alliances in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadHordeGearContainer()
{
    for (HordeGearContainer::const_iterator itr = m_HordeGearContainer.begin(); itr != m_HordeGearContainer.end(); ++itr)
        delete *itr;

    m_HordeGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_horde;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_horde` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        HordeGearTemplate* pItem = new HordeGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_HordeGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    TC_LOG_INFO("server.worldserver", ">>TEMPLATE NPC: Loaded %u gear templates for Hordes in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

std::string sTemplateNPC::GetClassString(Player* player)
{
    switch (player->GetClass())
    {
    case CLASS_PRIEST:       return "Priest";      break;
    case CLASS_PALADIN:      return "Paladin";     break;
    case CLASS_WARRIOR:      return "Warrior";     break;
    case CLASS_MAGE:         return "Mage";        break;
    case CLASS_WARLOCK:      return "Warlock";     break;
    case CLASS_SHAMAN:       return "Shaman";      break;
    case CLASS_DRUID:        return "Druid";       break;
    case CLASS_HUNTER:       return "Hunter";      break;
    case CLASS_ROGUE:        return "Rogue";       break;
    case CLASS_DEATH_KNIGHT: return "DeathKnight"; break;
    default:
        break;
    }
    return "Unknown"; // Fix warning, this should never happen
}

bool sTemplateNPC::OverwriteTemplate(Player* player, std::string& playerSpecStr)
{
    // Delete old talent and glyph templates before extracting new ones
    CharacterDatabase.PExecute("DELETE FROM template_npc_talents WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
    CharacterDatabase.PExecute("DELETE FROM template_npc_glyphs WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());

    // Delete old gear templates before extracting new ones
    if (player->GetRace() == RACE_HUMAN)
    {
        CharacterDatabase.PExecute("DELETE FROM template_npc_human WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        player->GetSession()->SendAreaTriggerMessage("Template successfuly created!");
        return false;
    }
    else if (player->GetTeam() == ALLIANCE && player->GetRace() != RACE_HUMAN)
    {
        CharacterDatabase.PExecute("DELETE FROM template_npc_alliance WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        player->GetSession()->SendAreaTriggerMessage("Template successfuly created!");
        return false;
    }
    else if (player->GetTeam() == HORDE)
    {                                                                                                        // ????????????? sTemplateNpcMgr here??
        CharacterDatabase.PExecute("DELETE FROM template_npc_horde WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        player->GetSession()->SendAreaTriggerMessage("Template successfuly created!");
        return false;
    }
    return true;
}

void sTemplateNPC::ExtractGearTemplateToDB(Player* player, std::string& playerSpecStr)
{
    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            if (player->GetRace() == RACE_HUMAN)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_human (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, `prismaticEnchant`) VALUES ('%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');"
                    , GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
            else if (player->GetTeam() == ALLIANCE && player->GetRace() != RACE_HUMAN)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_alliance (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, `prismaticEnchant`) VALUES ('%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');"
                    , GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
            else if (player->GetTeam() == HORDE)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_horde (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, `prismaticEnchant`) VALUES ('%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');"
                    , GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
        }
    }
}

void sTemplateNPC::ExtractTalentTemplateToDB(Player* player, std::string& playerSpecStr)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT spell FROM character_talent WHERE guid = '%u' "
        "AND talentGroup = '%u';", player->GetGUID(), player->GetActiveSpec());

    if (!result)
    {
        return;
    }
    else if (player->GetFreeTalentPoints() > 0)
    {
        player->GetSession()->SendAreaTriggerMessage("You have unspend talent points. Please spend all your talent points and re-extract the template.");
        return;
    }
    else
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spell = fields[0].GetUInt32();

            CharacterDatabase.PExecute("INSERT INTO template_npc_talents (playerClass, playerSpec, talentId) "
                "VALUES ('%s', '%s', '%u');", GetClassString(player).c_str(), playerSpecStr.c_str(), spell);
        } while (result->NextRow());
    }
}

void sTemplateNPC::ExtractGlyphsTemplateToDB(Player* player, std::string& playerSpecStr)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT glyph1, glyph2, glyph3, glyph4, glyph5, glyph6 "
        "FROM character_glyphs WHERE guid = '%u' AND talentGroup = '%u';", player->GetGUID(), player->GetActiveSpec());

    for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
    {
        if (!result)
        {
            player->GetSession()->SendAreaTriggerMessage("Get glyphs and re-extract the template!");
            continue;
        }

        Field* fields = result->Fetch();
        uint32 glyph1 = fields[0].GetUInt32();
        uint32 glyph2 = fields[1].GetUInt32();
        uint32 glyph3 = fields[2].GetUInt32();
        uint32 glyph4 = fields[3].GetUInt32();
        uint32 glyph5 = fields[4].GetUInt32();
        uint32 glyph6 = fields[5].GetUInt32();

        uint32 storedGlyph;

        switch (slot)
        {
        case 0:
            storedGlyph = glyph1;
            break;
        case 1:
            storedGlyph = glyph2;
            break;
        case 2:
            storedGlyph = glyph3;
            break;
        case 3:
            storedGlyph = glyph4;
            break;
        case 4:
            storedGlyph = glyph5;
            break;
        case 5:
            storedGlyph = glyph6;
            break;
        default:
            break;
        }

        CharacterDatabase.PExecute("INSERT INTO template_npc_glyphs (playerClass, playerSpec, slot, glyph) "
            "VALUES ('%s', '%s', '%u', '%u');", GetClassString(player).c_str(), playerSpecStr.c_str(), slot, storedGlyph);
    }
}

bool sTemplateNPC::CanEquipTemplate(Player* player, std::string& playerSpecStr)
{
    if (player->GetRace() == RACE_HUMAN)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_human "
            "WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    else if (player->GetTeam() == ALLIANCE && player->GetRace() != RACE_HUMAN)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_alliance "
            "WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    else if (player->GetTeam() == HORDE)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_horde "
            "WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    return true;
}

class TemplateNPC : public CreatureScript
{
public:
    TemplateNPC() : CreatureScript("TemplateNPC") { }

    struct TemplateNPC_AI : public ScriptedAI
    {
        TemplateNPC_AI(Creature* creature) : ScriptedAI(creature) { }
        bool OnGossipHello(Player* player) override
        {
            switch (player->GetClass())
            {
            case CLASS_PRIEST:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:30|t|r Use Discipline Spec", GOSSIP_SENDER_MAIN, 0);
                /*AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:30|t|r Use Discipline Spec (Talents Only)", GOSSIP_SENDER_MAIN, 100);*/
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:30|t|r Use Holy Spec", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_shadowwordpain:30|t|r Use Shadow Spec", GOSSIP_SENDER_MAIN, 2);
                break;
            case CLASS_PALADIN:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:30|t|r Use Holy Spec", GOSSIP_SENDER_MAIN, 3);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_devotionaura:30|t|r Use Protection Spec", GOSSIP_SENDER_MAIN, 4);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_auraoflight:30|t|r Use Retribution Spec", GOSSIP_SENDER_MAIN, 5);
                break;
            case CLASS_WARRIOR:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_innerrage:30|t|r Use Fury Spec", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:30|t|r Use Arms Spec", GOSSIP_SENDER_MAIN, 7);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_defensivestance:30|t|r Use Protection Spec", GOSSIP_SENDER_MAIN, 8);
                break;
            case CLASS_MAGE:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_magicalsentry:30|t|r Use Arcane Spec", GOSSIP_SENDER_MAIN, 9);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_fire_flamebolt:30|t|r Use Fire Spec", GOSSIP_SENDER_MAIN, 10);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_frost_frostbolt02:30|t|r Use Frost Spec", GOSSIP_SENDER_MAIN, 11);
                break;
            case CLASS_WARLOCK:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_deathcoil:30|t|r Use Affliction Spec", GOSSIP_SENDER_MAIN, 12);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_metamorphosis:30|t|r Use Demonology Spec", GOSSIP_SENDER_MAIN, 13);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_rainoffire:30|t|r Use Destruction Spec", GOSSIP_SENDER_MAIN, 14);
                break;
            case CLASS_SHAMAN:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightning:30|t|r Use Elemental Spec", GOSSIP_SENDER_MAIN, 15);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightningshield:30|t|r Use Enhancement Spec", GOSSIP_SENDER_MAIN, 16);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_magicimmunity:30|t|r Use Restoration Spec", GOSSIP_SENDER_MAIN, 17);
                break;
            case CLASS_DRUID:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_starfall:30|t|r Use Ballance Spec", GOSSIP_SENDER_MAIN, 18);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_racial_bearform:30|t|r Use Feral Spec", GOSSIP_SENDER_MAIN, 19);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_healingtouch:30|t|r Use Restoration Spec", GOSSIP_SENDER_MAIN, 20);
                break;
            case CLASS_HUNTER:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_marksmanship:30|t|r Use Markmanship Spec", GOSSIP_SENDER_MAIN, 21);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_hunter_beasttaming:30|t|r Use Beastmastery Spec", GOSSIP_SENDER_MAIN, 22);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_Hunter_swiftstrike:30|t|r Use Survival Spec", GOSSIP_SENDER_MAIN, 23);
                break;
            case CLASS_ROGUE:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:30|t|r Use Assasination Spec", GOSSIP_SENDER_MAIN, 24);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_backstab:30|t|r Use Combat Spec", GOSSIP_SENDER_MAIN, 25);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_stealth:30|t|r Use Subtlety Spec", GOSSIP_SENDER_MAIN, 26);
                break;
            case CLASS_DEATH_KNIGHT:
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_bloodpresence:30|t|r Use Blood Spec", GOSSIP_SENDER_MAIN, 27);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_frostpresence:30|t|r Use Frost Spec", GOSSIP_SENDER_MAIN, 28);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_unholypresence:30|t|r Use Unholy Spec", GOSSIP_SENDER_MAIN, 29);
                break;
            }

            /*AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\Spell_ChargeNegative:30|t|r Remove all glyphs", GOSSIP_SENDER_MAIN, 30);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\Spell_ChargeNegative:30|t|r Destroy my equipped gear", GOSSIP_SENDER_MAIN, 32);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\Spell_ChargeNegative:30|t|r Reset Talents", GOSSIP_SENDER_MAIN, 31);*/
			std::ostringstream messageG;
            messageG << BOOSTCOST << Boost_Costs << GOLDTEXT;
			if (player->GetLevel() < 80)
			{
			AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageG.str(), GOSSIP_SENDER_MAIN, 33);
			}
			
            SendGossipMenuFor(player, 55002, me->GetGUID());
            return true;
        }

        static void EquipFullTemplateGear(Player* player, std::string& playerSpecStr) // Merge
        {
            if (sTemplateNpcMgr->CanEquipTemplate(player, playerSpecStr) == false)
            {
                player->GetSession()->SendAreaTriggerMessage("There's no templates for %s specialization yet.", playerSpecStr.c_str());
                return;
            }

            // Don't let players to use Template feature while wearing some gear
            for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
            {
                if (Item* haveItemEquipped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    if (haveItemEquipped)
                    {
                        player->GetSession()->SendAreaTriggerMessage("You need to remove all your equipped items in order to use this feature!");
                        CloseGossipMenuFor(player);
                        return;
                    }
                }
            }
			
			

            // Don't let players to use Template feature after spending some talent points
            if (player->GetLevel() > 79 && player->GetFreeTalentPoints() < 71)
            {
                player->GetSession()->SendAreaTriggerMessage("You have already spent some talent points. You need to reset your talents first!");
                CloseGossipMenuFor(player);
                return;
            }
			

            sTemplateNpcMgr->LearnTemplateTalents(player);
            sTemplateNpcMgr->LearnTemplateGlyphs(player);
            sTemplateNpcMgr->EquipTemplateGear(player);
            sTemplateNpcMgr->LearnPlateMailSpells(player);

            LearnWeaponSkills(player);

            player->GetSession()->SendAreaTriggerMessage("Successfuly equipped %s %s template!", playerSpecStr.c_str(), sTemplateNpcMgr->GetClassString(player).c_str());

            if (player->GetPowerType() == POWER_MANA)
                player->SetFullPower(POWER_MANA);

            player->SetFullHealth();

            // Learn Riding/Flying
            if (player->HasSpell(Artisan_Riding) ||
                player->HasSpell(Cold_Weather_Flying) ||
                player->HasSpell(Amani_War_Bear) ||
                player->HasSpell(Teach_Learn_Talent_Specialization_Switches) ||
                player->HasSpell(Learn_a_Second_Talent_Specialization))
                return;

            // Cast spells that teach dual spec
            // Both are also ImplicitTarget self and must be cast by player
            player->CastSpell(player, Teach_Learn_Talent_Specialization_Switches, player->GetGUID());
            player->CastSpell(player, Learn_a_Second_Talent_Specialization, player->GetGUID());

            player->LearnSpell(Artisan_Riding, false);
            player->LearnSpell(Cold_Weather_Flying, false);
            player->LearnSpell(Amani_War_Bear, false);

        }

        static void LearnOnlyTalentsAndGlyphs(Player* player, std::string& playerSpecStr) // Merge
        {
            if (sTemplateNpcMgr->CanEquipTemplate(player, playerSpecStr) == false)
            {
                player->GetSession()->SendAreaTriggerMessage("There's no templates for %s specialization yet.", playerSpecStr.c_str());
                return;
            }

            // Don't let players to use Template feature after spending some talent points
            if (player->GetFreeTalentPoints() < 71)
            {
                player->GetSession()->SendAreaTriggerMessage("You have already spent some talent points. You need to reset your talents first!");
                CloseGossipMenuFor(player);
                return;
            }

            sTemplateNpcMgr->LearnTemplateTalents(player);
            sTemplateNpcMgr->LearnTemplateGlyphs(player);
            //sTemplateNpcMgr->EquipTemplateGear(player);
            sTemplateNpcMgr->LearnPlateMailSpells(player);

            LearnWeaponSkills(player);

            player->GetSession()->SendAreaTriggerMessage("Successfuly learned talent spec %s!", playerSpecStr.c_str());

            // Learn Riding/Flying
            if (player->HasSpell(Artisan_Riding) ||
                player->HasSpell(Cold_Weather_Flying) ||
                player->HasSpell(Amani_War_Bear) ||
                player->HasSpell(Teach_Learn_Talent_Specialization_Switches) ||
                player->HasSpell(Learn_a_Second_Talent_Specialization))
                return;

            // Cast spells that teach dual spec
            // Both are also ImplicitTarget self and must be cast by player
            player->CastSpell(player, Teach_Learn_Talent_Specialization_Switches, player->GetGUID());
            player->CastSpell(player, Learn_a_Second_Talent_Specialization, player->GetGUID());

            player->LearnSpell(Artisan_Riding, false);
            player->LearnSpell(Cold_Weather_Flying, false);
            player->LearnSpell(Amani_War_Bear, false);
        }

        bool OnGossipSelect(Player* player, uint32 /*uiSender*/, uint32 uiAction) override
        {
            uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(uiAction);
            uint32 action = player->PlayerTalkClass->GetGossipOptionAction(uiAction);
			

            ClearGossipMenuFor(player);

            if (!player || !me)
                return false;

            switch (action)
            {
            case 0: // Use Discipline Priest Spec
                sTemplateNpcMgr->sTalentsSpec = "Discipline";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 1: // Use Holy Priest Spec
                sTemplateNpcMgr->sTalentsSpec = "Holy";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 2: // Use Shadow Priest Spec
                sTemplateNpcMgr->sTalentsSpec = "Shadow";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 3: // Use Holy Paladin Spec
                sTemplateNpcMgr->sTalentsSpec = "Holy";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 4: // Use Protection Paladin Spec
                sTemplateNpcMgr->sTalentsSpec = "Protection";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 5: // Use Retribution Paladin Spec
                sTemplateNpcMgr->sTalentsSpec = "Retribution";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 6: // Use Fury Warrior Spec
                sTemplateNpcMgr->sTalentsSpec = "Fury";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 7: // Use Arms Warrior Spec
                sTemplateNpcMgr->sTalentsSpec = "Arms";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 8: // Use Protection Warrior Spec
                sTemplateNpcMgr->sTalentsSpec = "Protection";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 9: // Use Arcane Mage Spec
                sTemplateNpcMgr->sTalentsSpec = "Arcane";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 10: // Use Fire Mage Spec
                sTemplateNpcMgr->sTalentsSpec = "Fire";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 11: // Use Frost Mage Spec
                sTemplateNpcMgr->sTalentsSpec = "Frost";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 12: // Use Affliction Warlock Spec
                sTemplateNpcMgr->sTalentsSpec = "Affliction";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 13: // Use Demonology Warlock Spec
                sTemplateNpcMgr->sTalentsSpec = "Demonology";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 14: // Use Destruction Warlock Spec
                sTemplateNpcMgr->sTalentsSpec = "Destruction";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 15: // Use Elemental Shaman Spec
                sTemplateNpcMgr->sTalentsSpec = "Elemental";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 16: // Use Enhancement Shaman Spec
                sTemplateNpcMgr->sTalentsSpec = "Enhancement";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 17: // Use Restoration Shaman Spec
                sTemplateNpcMgr->sTalentsSpec = "Restoration";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 18: // Use Ballance Druid Spec
                sTemplateNpcMgr->sTalentsSpec = "Ballance";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 19: // Use Feral Druid Spec
                sTemplateNpcMgr->sTalentsSpec = "Feral";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 20: // Use Restoration Druid Spec
                sTemplateNpcMgr->sTalentsSpec = "Restoration";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 21: // Use Marksmanship Hunter Spec
                sTemplateNpcMgr->sTalentsSpec = "Marksmanship";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 22: // Use Beastmastery Hunter Spec
                sTemplateNpcMgr->sTalentsSpec = "Beastmastery";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 23: // Use Survival Hunter Spec
                sTemplateNpcMgr->sTalentsSpec = "Survival";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 24: // Use Assassination Rogue Spec
                sTemplateNpcMgr->sTalentsSpec = "Assassination";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 25: // Use Combat Rogue Spec
                sTemplateNpcMgr->sTalentsSpec = "Combat";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 26: // Use Subtlety Rogue Spec
                sTemplateNpcMgr->sTalentsSpec = "Subtlety";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 27: // Use Blood DK Spec
                sTemplateNpcMgr->sTalentsSpec = "Blood";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 28: // Use Frost DK Spec
                sTemplateNpcMgr->sTalentsSpec = "Frost";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 29: // Use Unholy DK Spec
                sTemplateNpcMgr->sTalentsSpec = "Unholy";
                EquipFullTemplateGear(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            case 30:
                sTemplateNpcMgr->RemoveAllGlyphs(player);
                player->GetSession()->SendAreaTriggerMessage("Your glyphs have been removed.");
                //OnGossipHello(player);
                CloseGossipMenuFor(player);
                break;

            case 31:
                player->ResetTalents(true);
                player->SendTalentsInfoData(false);
                player->GetSession()->SendAreaTriggerMessage("Your talents have been reset.");
                CloseGossipMenuFor(player);
                break;

            case 32:
                for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
                {
                    if (Item* haveItemEquipped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    {
                        if (haveItemEquipped)
                        {
                            player->DestroyItemCount(haveItemEquipped->GetEntry(), 1, true, true);

                            if (haveItemEquipped->IsInWorld())
                            {
                                haveItemEquipped->RemoveFromWorld();
                                haveItemEquipped->DestroyForPlayer(player);
                            }

                            haveItemEquipped->SetGuidValue(ITEM_FIELD_CONTAINED, ObjectGuid::Empty);
                            haveItemEquipped->SetSlot(NULL_SLOT);
                            haveItemEquipped->SetState(ITEM_REMOVED, player);
                        }
                    }
                }
                player->GetSession()->SendAreaTriggerMessage("Your equipped gear has been destroyed.");
                CloseGossipMenuFor(player);
                break;
				
			//skuly	1 gold = 10000
			case 33:	
				if (player->GetLevel() < 80)
            {
				if (player->HasEnoughMoney(int32(Boost_Costs * GOLD)))
				{
				player->SetLevel(80);
				player->InitRunes();
				player->InitStatsForLevel(true);
				player->InitTaxiNodesForLevel();
				player->InitGlyphsForLevel();
				player->InitTalentForLevel();
				player->ResetSpells(true);
				std::vector<Trainer::Trainer const*> const& trainers = sObjectMgr->GetClassTrainers(player->GetClass());
				 bool hadNew;
        do
        {
            hadNew = false;
            for (Trainer::Trainer const* trainer : trainers)
            {
                if (!trainer->IsTrainerValidForPlayer(player))
                    continue;
                for (Trainer::Spell const& trainerSpell : trainer->GetSpells())
                {
                    if (!trainer->CanTeachSpell(player, &trainerSpell))
                        continue;

                    if (trainerSpell.IsCastable())
                        player->CastSpell(player, trainerSpell.SpellId, true);
                    else
                        player->LearnSpell(trainerSpell.SpellId, false);

                    hadNew = true;
                }
            }
        } while (hadNew);

				player->ModifyMoney(-Boost_Costs * GOLD);
				}
				else
				{player->GetSession()->SendAreaTriggerMessage("You don't have enough Gold!");}
            }
				 CloseGossipMenuFor(player);	
				break;	

            case 100: // Use Discipline Priest Spec
                sTemplateNpcMgr->sTalentsSpec = "Discipline";
                LearnOnlyTalentsAndGlyphs(player, sTemplateNpcMgr->sTalentsSpec);
                CloseGossipMenuFor(player);
                break;

            default: // Just in case
                player->GetSession()->SendAreaTriggerMessage("Something went wrong in the code. Please contact the administrator.");
                break;
            }
            player->UpdateSkillsForLevel();

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new TemplateNPC_AI(creature);
    }
};

class TemplateNPC_World : public WorldScript
{
public:
    TemplateNPC_World() : WorldScript("TemplateNPC_World") { }

    void OnStartup() override
        {
		MindsearIsMyHero2();
        // Load templates for Template NPC #1
        TC_LOG_INFO("server.loading", "== TEMPLATE NPC ===========================================================================");
        TC_LOG_INFO("server.loading", "Loading Template Talents...");
        sTemplateNpcMgr->LoadTalentsContainer();

        // Load templates for Template NPC #2
        TC_LOG_INFO("server.loading", "Loading Template Glyphs...");
        sTemplateNpcMgr->LoadGlyphsContainer();

        // Load templates for Template NPC #3
        TC_LOG_INFO("server.loading", "Loading Template Gear for Humans...");
        sTemplateNpcMgr->LoadHumanGearContainer();

        // Load templates for Template NPC #4
        TC_LOG_INFO("server.loading", "Loading Template Gear for Alliances...");
        sTemplateNpcMgr->LoadAllianceGearContainer();

        // Load templates for Template NPC #5
        TC_LOG_INFO("server.loading", "Loading Template Gear for Hordes...");
        sTemplateNpcMgr->LoadHordeGearContainer();
        TC_LOG_INFO("server.loading", "== END TEMPLATE NPC ===========================================================================");

		}
};

class TemplateNPC_command : public CommandScript
{
public:
    TemplateNPC_command() : CommandScript("TemplateNPC_command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> TemplateNPCTable =
        {
            { "reload", rbac::RBAC_PERM_COMMAND_RELOAD, true , &HandleReloadTemplateNPCCommand, "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "templatenpc", TemplateNPCTable },
        };
        return commandTable;
    }


    static bool HandleReloadTemplateNPCCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("server.loading", "misc", "Reloading templates for Template NPC table...");
		sWorld->LoadConfigSettings(true);
		MindsearIsMyHero2();
        sTemplateNpcMgr->LoadTalentsContainer();
        sTemplateNpcMgr->LoadGlyphsContainer();
        sTemplateNpcMgr->LoadHumanGearContainer();
        sTemplateNpcMgr->LoadAllianceGearContainer();
        sTemplateNpcMgr->LoadHordeGearContainer();
        handler->SendGlobalGMSysMessage("Template NPC templates reloaded.");
        return true;
    }
};

void AddSC_TemplateNPC()
{
    new TemplateNPC();
    new TemplateNPC_World();
    new TemplateNPC_command();
}
