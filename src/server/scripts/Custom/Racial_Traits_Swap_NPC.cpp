/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Racial Traits Swap NPC For Trinitycore Custom Changes Branch
 * Written by SPP DEV MDIC
 * SQL Assistance by Nemesis Dev Jinnai
*/

#include "Chat.h"
#include "Config.h"
#include "Creature.h"
#include "GameEventMgr.h"
#include "Item.h"
#include "ItemEnchantmentMgr.h"
#include "Player.h"
#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "WorldSession.h"

#define LOCALE_RACESWAP_0 "Racial Trait Swap"
#define LOCALE_RACESWAP_1 "인종 특성 교환"
#define LOCALE_RACESWAP_2 "Échange de traits raciaux"
#define LOCALE_RACESWAP_3 "Rassenmerkmalswechsel"
#define LOCALE_RACESWAP_4 "種族特質交換"
#define LOCALE_RACESWAP_5 "種族特質交換"
#define LOCALE_RACESWAP_6 "Intercambio de rasgos raciales"
#define LOCALE_RACESWAP_7 "Intercambio de rasgos raciales"
#define LOCALE_RACESWAP_8 "Обмен расовыми особенностями"

#define LOCALE_EXIT_0 "[EXIT]"
#define LOCALE_EXIT_1 "[출구]"
#define LOCALE_EXIT_2 "[SORTIR]"
#define LOCALE_EXIT_3 "[AUSFAHRT]"
#define LOCALE_EXIT_4 "[出口]"
#define LOCALE_EXIT_5 "[出口]"
#define LOCALE_EXIT_6 "[SALIDA]"
#define LOCALE_EXIT_7 "[SALIDA]"
#define LOCALE_EXIT_8 "[ВЫХОД]"

#define LOCALE_BACK_0 "[BACK]"
#define LOCALE_BACK_1 "[출구]"
#define LOCALE_BACK_2 "[RETOUR]"
#define LOCALE_BACK_3 "[ZURÜCK]"
#define LOCALE_BACK_4 "[背部]"
#define LOCALE_BACK_5 "[背部]"
#define LOCALE_BACK_6 "[ESPALDA]"
#define LOCALE_BACK_7 "[ESPALDA]"
#define LOCALE_BACK_8 "[НАЗАД]"

#define LOCALE_GOLD_0 " Gold."
#define LOCALE_GOLD_1 " 골드."
#define LOCALE_GOLD_2 " L' or."
#define LOCALE_GOLD_3 " Gold."
#define LOCALE_GOLD_4 " 金."
#define LOCALE_GOLD_5 " 金."
#define LOCALE_GOLD_6 " Oro."
#define LOCALE_GOLD_7 " Oro."
#define LOCALE_GOLD_8 " Золото."

#define LOCALE_BES_0 "Blood Elf Racial Trait Swap -"
#define LOCALE_BES_1 "블러드 엘프 인종 특성 스왑 -"
#define LOCALE_BES_2 "Échange de traits raciaux des elfes de sang -"
#define LOCALE_BES_3 "Rassenwechsel der Blutelfen -"
#define LOCALE_BES_4 "血精靈種族特質交換 -"
#define LOCALE_BES_5 "血精靈種族特質交換 -"
#define LOCALE_BES_6 "Intercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_7 "Intercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_8 "Обмен расовой особенностью эльфов крови -"

#define LOCALE_DRS_0 "Draenei Racial Trait Swap  -"
#define LOCALE_DRS_1 "드레나이 인종 특성 스왑 -"
#define LOCALE_DRS_2 "Échange de traits raciaux draeneï -"
#define LOCALE_DRS_3 "Draenei Rasseneigenschaftstausch -"
#define LOCALE_DRS_4 "德萊尼人種特質交換 -"
#define LOCALE_DRS_5 "德萊尼人種特質交換 -"
#define LOCALE_DRS_6 "Cambio de rasgo racial Draenei -"
#define LOCALE_DRS_7 "Cambio de rasgo racial Draenei -"
#define LOCALE_DRS_8 "Обмен расовыми особенностями дренеев -"

#define LOCALE_DWS_0 "Dwarven Racial Trait Swap -"
#define LOCALE_DWS_1 "드워프 종족 특성 전환 -"
#define LOCALE_DWS_2 "Échange de traits raciaux nains -"
#define LOCALE_DWS_3 "Tausch der Rasseneigenschaften der Zwerge -"
#define LOCALE_DWS_4 "矮人種族特質交換 -"
#define LOCALE_DWS_5 "矮人種族特質交換 -"
#define LOCALE_DWS_6 "Intercambio de rasgos raciales enanos -"
#define LOCALE_DWS_7 "Intercambio de rasgos raciales enanos -"
#define LOCALE_DWS_8 "Обмен расовыми особенностями гномов -"

#define LOCALE_GNS_0 "Gnome Racial Trait Swap -"
#define LOCALE_GNS_1 "그놈 인종 특성 스왑 -"
#define LOCALE_GNS_2 "Échange de traits raciaux de gnome -"
#define LOCALE_GNS_3 "Tausch der Rasseneigenschaft Gnom -"
#define LOCALE_GNS_4 "侏儒種族特質交換 -"
#define LOCALE_GNS_5 "侏儒種族特質交換 -"
#define LOCALE_GNS_6 "Cambio de rasgo racial de gnomo -"
#define LOCALE_GNS_7 "Cambio de rasgo racial de gnomo -"
#define LOCALE_GNS_8 "Смена расовой особенности гномов -"

#define LOCALE_HUS_0 "Human Racial Trait Swap -"
#define LOCALE_HUS_1 "인간 인종 특성 교환 -"
#define LOCALE_HUS_2 "Échange de traits raciaux humains -"
#define LOCALE_HUS_3 "Austausch von menschlichen Rassenmerkmalen -"
#define LOCALE_HUS_4 "人類種族特質交換 -"
#define LOCALE_HUS_5 "人類種族特質交換 -"
#define LOCALE_HUS_6 "Intercambio de rasgos raciales humanos -"
#define LOCALE_HUS_7 "Intercambio de rasgos raciales humanos -"
#define LOCALE_HUS_8 "Обмен расовыми особенностями человека -"

#define LOCALE_NES_0 "Night Elf Racial Trait Swap -"
#define LOCALE_NES_1 "나이트 엘프 인종 특성 스왑 -"
#define LOCALE_NES_2 "Échange de traits raciaux des elfes de la nuit -"
#define LOCALE_NES_3 "Tausch der Nachtelfen-Rasseneigenschaft -"
#define LOCALE_NES_4 "暗夜精靈種族特質交換 -"
#define LOCALE_NES_5 "暗夜精靈種族特質交換 -"
#define LOCALE_NES_6 "Cambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_7 "Cambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_8 "Обмен расовыми особенностями ночных эльфов -"

#define LOCALE_ORC_0 "Orc Racial Trait Swap -"
#define LOCALE_ORC_1 "오크 종족 특성 교환 -"
#define LOCALE_ORC_2 "Échange de traits raciaux orques -"
#define LOCALE_ORC_3 "Austausch der Ork-Rassenmerkmale -"
#define LOCALE_ORC_4 "獸人種族特質交換 -"
#define LOCALE_ORC_5 "獸人種族特質交換 -"
#define LOCALE_ORC_6 "Intercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_7 "Intercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_8 "Смена расовой особенности орков -"

#define LOCALE_TUR_0 "Tauren Racial Trait Swap -"
#define LOCALE_TUR_1 "타우렌 인종 특성 교환 -"
#define LOCALE_TUR_2 "Échange de traits raciaux Tauren -"
#define LOCALE_TUR_3 "Tauren Rasseneigenschaftstausch -"
#define LOCALE_TUR_4 "牛頭人種族特質交換 -"
#define LOCALE_TUR_5 "牛頭人種族特質交換 -"
#define LOCALE_TUR_6 "Intercambio de rasgos raciales tauren -"
#define LOCALE_TUR_7 "Intercambio de rasgos raciales tauren -"
#define LOCALE_TUR_8 "Обмен расовыми особенностями тауренов -"

#define LOCALE_TRL_0 "Troll Racial Trait Swap -"
#define LOCALE_TRL_1 "트롤 인종 특성 교환 -"
#define LOCALE_TRL_2 "Échange de traits raciaux de troll -"
#define LOCALE_TRL_3 "Troll-Rasseneigenschaftstausch -"
#define LOCALE_TRL_4 "巨魔種族特質交換 -"
#define LOCALE_TRL_5 "巨魔種族特質交換 -"
#define LOCALE_TRL_6 "Cambio de rasgo racial de trol -"
#define LOCALE_TRL_7 "Cambio de rasgo racial de trol -"
#define LOCALE_TRL_8 "Обмен расовой особенности троллей -"

#define LOCALE_UND_0 "Undead Racial Trait Swap -"
#define LOCALE_UND_1 "언데드 인종 특성 스왑 -"
#define LOCALE_UND_2 "Échange de traits raciaux morts-vivants -"
#define LOCALE_UND_3 "Untoter Rassenmerkmalentausch -"
#define LOCALE_UND_4 "亡靈種族特質交換 -"
#define LOCALE_UND_5 "亡靈種族特質交換 -"
#define LOCALE_UND_6 "Intercambio de rasgos raciales no muertos -"
#define LOCALE_UND_7 "Intercambio de rasgos raciales no muertos -"
#define LOCALE_UND_8 "Смена расовой особенности нежити -"

class Trinitycore_Race_Trait_announce : public PlayerScript
{
public:
    Trinitycore_Race_Trait_announce() : PlayerScript("Trinitycore_Race_Trait_announce") { }

    void OnLogin(Player* Player, bool /*firstLogin*/) override
    {
        if (sConfigMgr->GetBoolDefault("Trinitycore.Racial.Trait.Swap.Announce.enable", true))
        {
            ChatHandler(Player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Trinitycore Racial Trait Swap NPC |rmodule.");
        }
    }
};

class npc_race_trait_swap : public CreatureScript
{
public:

    npc_race_trait_swap() : CreatureScript("npc_race_trait_swap") { }

    struct npc_race_trait_swapAI : public ScriptedAI
    {
        npc_race_trait_swapAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (player->IsInCombat())
            {
                ClearGossipMenuFor(player);
                ChatHandler(player->GetSession()).PSendSysMessage("You are still in combat!");
                return true;
            }
            else
            {
                return OnGossipHello(player, me);
            }
        }

        static bool OnGossipHello(Player* player, Creature* creature)
        {
            char const* localizedEntry;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedEntry = LOCALE_RACESWAP_1; break;
            case LOCALE_frFR: localizedEntry = LOCALE_RACESWAP_2; break;
            case LOCALE_deDE: localizedEntry = LOCALE_RACESWAP_3; break;
            case LOCALE_zhCN: localizedEntry = LOCALE_RACESWAP_4; break;
            case LOCALE_zhTW: localizedEntry = LOCALE_RACESWAP_5; break;
            case LOCALE_esES: localizedEntry = LOCALE_RACESWAP_6; break;
            case LOCALE_esMX: localizedEntry = LOCALE_RACESWAP_7; break;
            case LOCALE_ruRU: localizedEntry = LOCALE_RACESWAP_8; break;
            case LOCALE_enUS: default: localizedEntry = LOCALE_RACESWAP_0;
            }
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, localizedEntry, GOSSIP_SENDER_MAIN, 11);
            SendGossipMenuFor(player, 98888, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            return OnGossipSelect(player, me, sender, action);
        }

        bool OnGossipSelect(Player* player, Creature* _creature, uint32 /*sender*/, uint32 uiAction)
        {
            const int32 RTS1 = sConfigMgr->GetIntDefault("Racial.Traits.Swap.Gold", 100);
            char const* localizedExit;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedExit = LOCALE_EXIT_1; break;
            case LOCALE_frFR: localizedExit = LOCALE_EXIT_2; break;
            case LOCALE_deDE: localizedExit = LOCALE_EXIT_3; break;
            case LOCALE_zhCN: localizedExit = LOCALE_EXIT_4; break;
            case LOCALE_zhTW: localizedExit = LOCALE_EXIT_5; break;
            case LOCALE_esES: localizedExit = LOCALE_EXIT_6; break;
            case LOCALE_esMX: localizedExit = LOCALE_EXIT_7; break;
            case LOCALE_ruRU: localizedExit = LOCALE_EXIT_8; break;
            case LOCALE_enUS: default: localizedExit = LOCALE_EXIT_0;
            }
            char const* localizedBack;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedBack = LOCALE_BACK_1; break;
            case LOCALE_frFR: localizedBack = LOCALE_BACK_2; break;
            case LOCALE_deDE: localizedBack = LOCALE_BACK_3; break;
            case LOCALE_zhCN: localizedBack = LOCALE_BACK_4; break;
            case LOCALE_zhTW: localizedBack = LOCALE_BACK_5; break;
            case LOCALE_esES: localizedBack = LOCALE_BACK_6; break;
            case LOCALE_esMX: localizedBack = LOCALE_BACK_7; break;
            case LOCALE_ruRU: localizedBack = LOCALE_BACK_8; break;
            case LOCALE_enUS: default: localizedBack = LOCALE_BACK_0;
            }
            char const* localizedGold;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedGold = LOCALE_GOLD_1; break;
            case LOCALE_frFR: localizedGold = LOCALE_GOLD_2; break;
            case LOCALE_deDE: localizedGold = LOCALE_GOLD_3; break;
            case LOCALE_zhCN: localizedGold = LOCALE_GOLD_4; break;
            case LOCALE_zhTW: localizedGold = LOCALE_GOLD_5; break;
            case LOCALE_esES: localizedGold = LOCALE_GOLD_6; break;
            case LOCALE_esMX: localizedGold = LOCALE_GOLD_7; break;
            case LOCALE_ruRU: localizedGold = LOCALE_GOLD_8; break;
            case LOCALE_enUS: default: localizedGold = LOCALE_GOLD_0;
            }
            char const* localizedBES;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedBES = LOCALE_BES_1; break;
            case LOCALE_frFR: localizedBES = LOCALE_BES_2; break;
            case LOCALE_deDE: localizedBES = LOCALE_BES_3; break;
            case LOCALE_zhCN: localizedBES = LOCALE_BES_4; break;
            case LOCALE_zhTW: localizedBES = LOCALE_BES_5; break;
            case LOCALE_esES: localizedBES = LOCALE_BES_6; break;
            case LOCALE_esMX: localizedBES = LOCALE_BES_7; break;
            case LOCALE_ruRU: localizedBES = LOCALE_BES_8; break;
            case LOCALE_enUS: default: localizedBES = LOCALE_BES_0;
            }
            char const* localizedDRS;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedDRS = LOCALE_DRS_1; break;
            case LOCALE_frFR: localizedDRS = LOCALE_DRS_2; break;
            case LOCALE_deDE: localizedDRS = LOCALE_DRS_3; break;
            case LOCALE_zhCN: localizedDRS = LOCALE_DRS_4; break;
            case LOCALE_zhTW: localizedDRS = LOCALE_DRS_5; break;
            case LOCALE_esES: localizedDRS = LOCALE_DRS_6; break;
            case LOCALE_esMX: localizedDRS = LOCALE_DRS_7; break;
            case LOCALE_ruRU: localizedDRS = LOCALE_DRS_8; break;
            case LOCALE_enUS: default: localizedDRS = LOCALE_DRS_0;
            }
            char const* localizedDWS;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedDWS = LOCALE_DWS_1; break;
            case LOCALE_frFR: localizedDWS = LOCALE_DWS_2; break;
            case LOCALE_deDE: localizedDWS = LOCALE_DWS_3; break;
            case LOCALE_zhCN: localizedDWS = LOCALE_DWS_4; break;
            case LOCALE_zhTW: localizedDWS = LOCALE_DWS_5; break;
            case LOCALE_esES: localizedDWS = LOCALE_DWS_6; break;
            case LOCALE_esMX: localizedDWS = LOCALE_DWS_7; break;
            case LOCALE_ruRU: localizedDWS = LOCALE_DWS_8; break;
            case LOCALE_enUS: default: localizedDWS = LOCALE_DWS_0;
            }
            char const* localizedGNS;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedGNS = LOCALE_GNS_1; break;
            case LOCALE_frFR: localizedGNS = LOCALE_GNS_2; break;
            case LOCALE_deDE: localizedGNS = LOCALE_GNS_3; break;
            case LOCALE_zhCN: localizedGNS = LOCALE_GNS_4; break;
            case LOCALE_zhTW: localizedGNS = LOCALE_GNS_5; break;
            case LOCALE_esES: localizedGNS = LOCALE_GNS_6; break;
            case LOCALE_esMX: localizedGNS = LOCALE_GNS_7; break;
            case LOCALE_ruRU: localizedGNS = LOCALE_GNS_8; break;
            case LOCALE_enUS: default: localizedGNS = LOCALE_GNS_0;
            }
            char const* localizedHUS;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedHUS = LOCALE_HUS_1; break;
            case LOCALE_frFR: localizedHUS = LOCALE_HUS_2; break;
            case LOCALE_deDE: localizedHUS = LOCALE_HUS_3; break;
            case LOCALE_zhCN: localizedHUS = LOCALE_HUS_4; break;
            case LOCALE_zhTW: localizedHUS = LOCALE_HUS_5; break;
            case LOCALE_esES: localizedHUS = LOCALE_HUS_6; break;
            case LOCALE_esMX: localizedHUS = LOCALE_HUS_7; break;
            case LOCALE_ruRU: localizedHUS = LOCALE_HUS_8; break;
            case LOCALE_enUS: default: localizedHUS = LOCALE_HUS_0;
            }
            char const* localizedNES;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedNES = LOCALE_NES_1; break;
            case LOCALE_frFR: localizedNES = LOCALE_NES_2; break;
            case LOCALE_deDE: localizedNES = LOCALE_NES_3; break;
            case LOCALE_zhCN: localizedNES = LOCALE_NES_4; break;
            case LOCALE_zhTW: localizedNES = LOCALE_NES_5; break;
            case LOCALE_esES: localizedNES = LOCALE_NES_6; break;
            case LOCALE_esMX: localizedNES = LOCALE_NES_7; break;
            case LOCALE_ruRU: localizedNES = LOCALE_NES_8; break;
            case LOCALE_enUS: default: localizedNES = LOCALE_NES_0;
            }
            char const* localizedORC;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedORC = LOCALE_ORC_1; break;
            case LOCALE_frFR: localizedORC = LOCALE_ORC_2; break;
            case LOCALE_deDE: localizedORC = LOCALE_ORC_3; break;
            case LOCALE_zhCN: localizedORC = LOCALE_ORC_4; break;
            case LOCALE_zhTW: localizedORC = LOCALE_ORC_5; break;
            case LOCALE_esES: localizedORC = LOCALE_ORC_6; break;
            case LOCALE_esMX: localizedORC = LOCALE_ORC_7; break;
            case LOCALE_ruRU: localizedORC = LOCALE_ORC_8; break;
            case LOCALE_enUS: default: localizedORC = LOCALE_ORC_0;
            }
            char const* localizedTUR;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedTUR = LOCALE_TUR_1; break;
            case LOCALE_frFR: localizedTUR = LOCALE_TUR_2; break;
            case LOCALE_deDE: localizedTUR = LOCALE_TUR_3; break;
            case LOCALE_zhCN: localizedTUR = LOCALE_TUR_4; break;
            case LOCALE_zhTW: localizedTUR = LOCALE_TUR_5; break;
            case LOCALE_esES: localizedTUR = LOCALE_TUR_6; break;
            case LOCALE_esMX: localizedTUR = LOCALE_TUR_7; break;
            case LOCALE_ruRU: localizedTUR = LOCALE_TUR_8; break;
            case LOCALE_enUS: default: localizedTUR = LOCALE_TUR_0;
            }
            char const* localizedTRL;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedTRL = LOCALE_TRL_1; break;
            case LOCALE_frFR: localizedTRL = LOCALE_TRL_2; break;
            case LOCALE_deDE: localizedTRL = LOCALE_TRL_3; break;
            case LOCALE_zhCN: localizedTRL = LOCALE_TRL_4; break;
            case LOCALE_zhTW: localizedTRL = LOCALE_TRL_5; break;
            case LOCALE_esES: localizedTRL = LOCALE_TRL_6; break;
            case LOCALE_esMX: localizedTRL = LOCALE_TRL_7; break;
            case LOCALE_ruRU: localizedTRL = LOCALE_TRL_8; break;
            case LOCALE_enUS: default: localizedTRL = LOCALE_TRL_0;
            }
            char const* localizedUND;
            switch (player->GetSession()->GetSessionDbcLocale())
            {
            case LOCALE_koKR: localizedUND = LOCALE_UND_1; break;
            case LOCALE_frFR: localizedUND = LOCALE_UND_2; break;
            case LOCALE_deDE: localizedUND = LOCALE_UND_3; break;
            case LOCALE_zhCN: localizedUND = LOCALE_UND_4; break;
            case LOCALE_zhTW: localizedUND = LOCALE_UND_5; break;
            case LOCALE_esES: localizedUND = LOCALE_UND_6; break;
            case LOCALE_esMX: localizedUND = LOCALE_UND_7; break;
            case LOCALE_ruRU: localizedUND = LOCALE_UND_8; break;
            case LOCALE_enUS: default: localizedUND = LOCALE_UND_0;
            }
            std::ostringstream messageBE;
            messageBE << localizedBES << RTS1 << localizedGold;
            std::ostringstream messageDR;
            messageDR << localizedDRS << RTS1 << localizedGold;
            std::ostringstream messageDW;
            messageDW << localizedDWS << RTS1 << localizedGold;
            std::ostringstream messageGN;
            messageGN << localizedGNS << RTS1 << localizedGold;
            std::ostringstream messageHU;
            messageHU << localizedHUS << RTS1 << localizedGold;
            std::ostringstream messageNE;
            messageNE << localizedNES << RTS1 << localizedGold;
            std::ostringstream messageOR;
            messageOR << localizedORC << RTS1 << localizedGold;
            std::ostringstream messageTA;
            messageTA << localizedTUR << RTS1 << localizedGold;
            std::ostringstream messageTR;
            messageTR << localizedTRL << RTS1 << localizedGold;
            std::ostringstream messageUN;
            messageUN << localizedUND << RTS1 << localizedGold;
            ClearGossipMenuFor(player);

            switch (uiAction)
            {

            case 11:

                if (player)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageBE.str(), GOSSIP_SENDER_MAIN, 1);//Blood Elf Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageDR.str(), GOSSIP_SENDER_MAIN, 2);//Draenei Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageDW.str(), GOSSIP_SENDER_MAIN, 3);//Dwarves Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageGN.str(), GOSSIP_SENDER_MAIN, 4);//Gnome Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageHU.str(), GOSSIP_SENDER_MAIN, 5);//Human Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageNE.str(), GOSSIP_SENDER_MAIN, 6);//Night Elf Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageOR.str(), GOSSIP_SENDER_MAIN, 7);//Orc Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageTA.str(), GOSSIP_SENDER_MAIN, 8);//Tauren Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageTR.str(), GOSSIP_SENDER_MAIN, 9);//Troll Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageUN.str(), GOSSIP_SENDER_MAIN, 10);//Undead
                    AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                    SendGossipMenuFor(player, 98888, _creature->GetGUID());
                }
                break;

            case 1111://close
                CloseGossipMenuFor(player);
                break;

            case 1://Blood Elf Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Pride of the Blood Elves", GOSSIP_SENDER_MAIN, 112);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988881, _creature->GetGUID());
                break;

            case 112://Blood Elf
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(28877, false);//Arcane Affinity
                player->LearnSpell(822, false);// Magic Resistance
                if (player->GetClass() == CLASS_DEATH_KNIGHT)
                {
                    player->LearnSpell(50613, false);//Arcane Torrent
                }
                if (player->GetClass() == CLASS_HUNTER || player->GetClass() == CLASS_MAGE || player->GetClass() == CLASS_PALADIN || player->GetClass() == CLASS_WARLOCK)
                {
                    player->LearnSpell(28730, false);//Arcane Torrent
                }
                if (player->GetClass() == CLASS_ROGUE)
                {
                    player->LearnSpell(25046, false);//Arcane Torrent
                }
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 2://Draenei Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Light of the Draenei", GOSSIP_SENDER_MAIN, 212);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988882, _creature->GetGUID());
                break;

            case 212://Draenei
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(28875, false);// Gem Cutting
                if (player->GetClass() == CLASS_DEATH_KNIGHT)
                {
                    player->LearnSpell(59545, false);//Gift of Naaru
                    player->LearnSpell(59539, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_DEATH_KNIGHT || player->GetClass() == CLASS_HUNTER || player->GetClass() == CLASS_PALADIN || player->GetClass() == CLASS_WARRIOR)
                {
                    player->LearnSpell(6562, false);//Heroic Presence
                }
                if (player->GetClass() == CLASS_HUNTER)
                {
                    player->LearnSpell(59543, false);//Gift of Naaru
                    player->LearnSpell(59536, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_MAGE)
                {
                    player->LearnSpell(59548, false);//Gift of Naaru
                    player->LearnSpell(59541, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_MAGE || player->GetClass() == CLASS_PRIEST || player->GetClass() == CLASS_SHAMAN)
                {
                    player->LearnSpell(28878, false);//Heroic Presence
                }
                if (player->GetClass() == CLASS_PALADIN)
                {
                    player->LearnSpell(59542, false);//Gift of Naaru
                    player->LearnSpell(59535, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_PRIEST)
                {
                    player->LearnSpell(59544, false);//Gift of Naaru
                    player->LearnSpell(59538, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_SHAMAN)
                {
                    player->LearnSpell(59547, false);//Gift of Naaru
                    player->LearnSpell(59540, false);//Shadow Resistance
                }
                if (player->GetClass() == CLASS_SHAMAN)
                {
                    player->LearnSpell(28880, false);//Gift of Naaru
                    player->LearnSpell(59221, false);//Shadow Resistance
                }
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 3://Dwarves Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Iron Will of the Dwarves", GOSSIP_SENDER_MAIN, 31);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988883, _creature->GetGUID());
                break;

            case 31://Dwarves
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(2481, false);//Find Treasure
                player->LearnSpell(20596, false);//Frost Resistance
                player->LearnSpell(20595, false);//Gun Specialization
                player->LearnSpell(59224, false);//Mace Specialization
                player->LearnSpell(20594, false);//Stoneform
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 4://Gnome Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Technological Might of the Gnomes", GOSSIP_SENDER_MAIN, 41);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988884, _creature->GetGUID());
                break;

            case 41://Gnome
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occur and money is deducted
                player->LearnSpell(20592, false);//Arcane Resistance
                player->LearnSpell(20593, false);//Engineering Specialization
                player->LearnSpell(20589, false);//Escape Artist
                player->LearnSpell(20591, false);//Expansive Mind
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 5://Human Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Human Spirit", GOSSIP_SENDER_MAIN, 51);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988885, _creature->GetGUID());
                break;

            case 51://Human
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20599, false);//Diplomacy
                player->LearnSpell(59752, false);//Every man for himself
                player->LearnSpell(20864, false);//Mace Specialization
                player->LearnSpell(58985, false);//Perception
                player->LearnSpell(20597, false);//Sword Specialization
                player->LearnSpell(20598, false);//The Human Spirit
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 6://Night Elf Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Ancient Night Elves", GOSSIP_SENDER_MAIN, 61);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988886, _creature->GetGUID());
                break;

            case 61://Night Elf
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20583, false);//Nature Resistance
                player->LearnSpell(20582, false);//Quickness
                player->LearnSpell(58984, false);//Shadowmeld
                player->LearnSpell(20585, false);//Wisp Spirit
                if (player->GetClass() == CLASS_DEATH_KNIGHT || player->GetClass() == CLASS_DRUID || player->GetClass() == CLASS_HUNTER || player->GetClass() == CLASS_PRIEST || player->GetClass() == CLASS_ROGUE || player->GetClass() == CLASS_WARRIOR)
                {
                    player->LearnSpell(21009, false);//Elusiveness
                }
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 7://Orc Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Strength of the Orc", GOSSIP_SENDER_MAIN, 71);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988887, _creature->GetGUID());
                break;

            case 71://Orc
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20574, false);//Axe Specialization
                player->LearnSpell(20573, false);//Hardiness
                if (player->GetClass() == CLASS_DEATH_KNIGHT)
                {
                    player->LearnSpell(54562, false);//Command
                }
                if (player->GetClass() == CLASS_DEATH_KNIGHT || player->GetClass() == CLASS_HUNTER || player->GetClass() == CLASS_ROGUE || player->GetClass() == CLASS_WARRIOR)
                {
                    player->LearnSpell(20572, false);//Blood Fury
                }
                if (player->GetClass() == CLASS_DRUID || player->GetClass() == CLASS_PALADIN || player->GetClass() == CLASS_PRIEST || player->GetClass() == CLASS_ROGUE || player->GetClass() == CLASS_WARRIOR)
                {
                    player->LearnSpell(21563, false);//Command
                }
                if (player->GetClass() == CLASS_HUNTER)
                {
                    player->LearnSpell(20576, false);//Command
                }
                if (player->GetClass() == CLASS_SHAMAN)
                {
                    player->LearnSpell(33697, false);//Blood Fury
                    player->LearnSpell(65222, false);//Command
                }
                if (player->GetClass() == CLASS_WARLOCK)
                {
                    player->LearnSpell(33702, false);//Blood Fury
                    player->LearnSpell(20575, false);//Command
                }
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 8://Tauren Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Endurance of the Taurens", GOSSIP_SENDER_MAIN, 81);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988888, _creature->GetGUID());
                break;

            case 81://Tauren
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20552, false);//Cultivation
                player->LearnSpell(20550, false);//Endurance
                player->LearnSpell(20551, false);//Nature Resistance
                player->LearnSpell(20549, false);//Warstomp
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 9://Troll Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Vodoo of the Trolls", GOSSIP_SENDER_MAIN, 91);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988889, _creature->GetGUID());
                break;

            case 91://Troll
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20557, false);//Beast Slaying
                player->LearnSpell(26297, false);//Berserking
                player->LearnSpell(26290, false);//Bow Specialization
                player->LearnSpell(58943, false);//Da Voodoo Shuffle
                player->LearnSpell(20555, false);//Regeneration
                player->LearnSpell(20558, false);//Throwing Specialization
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Undead:
                player->RemoveSpell(20577, false, false);// unlearn Cannibalize
                player->RemoveSpell(20579, false, false);// unlearn Shadow Resistance
                player->RemoveSpell(5227, false, false);// unlearn Underwater Breating
                player->RemoveSpell(7744, false, false);// unlearn Will of the Forsaken
                break;

            case 10://Undead Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Will of the Forsaken", GOSSIP_SENDER_MAIN, 101);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988880, _creature->GetGUID());
                break;

            case 101://Undead
                CloseGossipMenuFor(player);
                if (!player->HasEnoughMoney(int32(RTS1 * GOLD)))//gold check
                    return true;
                player->ModifyMoney(int32(-RTS1 * GOLD));//Deducting the money if check passes
                player->CastSpell(player, 47292);//Level up visual effect to let you know the transaction did occure and money is deducted
                player->LearnSpell(20577, false);//Cannibalize
                player->LearnSpell(20579, false);//Shadow Resistance
                player->LearnSpell(5227, false);//Underwater Breating
                player->LearnSpell(7744, false);//Will of the Forsaken
                //blood elf:
                player->RemoveSpell(28877, false, false);// unlearn Arcane Affinity
                player->RemoveSpell(822, false, false);// unlearn Magic Resistance
                //~DK
                player->RemoveSpell(50613, false, false);// unlearn Arcane Torrent
                //~Hunter, mage, paladin, priest, warlock
                player->RemoveSpell(28730, false, false);// unlearn Arcane Torrent
                //~Rogue
                player->RemoveSpell(25046, false, false);// unlearn Arcane Torrent
                //Draenei:
                player->RemoveSpell(28875, false, false);// unlearn Gemcutting
                //~DK
                player->RemoveSpell(59545, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59539, false, false);// unlearnShadow Resistance
                //~DK, Hunter, Paladin, Warrior
                player->RemoveSpell(6562, false, false);// unlearn Heroic Presence
                //~Hunter
                player->RemoveSpell(59543, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59536, false, false);// unlearn Shadow Resistance
                //~Mage
                player->RemoveSpell(59548, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59541, false, false);// unlearn Shadow Resistance
                //~Mage, Priest, Shaman
                player->RemoveSpell(28878, false, false);// unlearn Heroic Presence
                //~Paladin
                player->RemoveSpell(59542, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59535, false, false);// unlearn Shadow Resistance
                //~Priest
                player->RemoveSpell(59544, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59538, false, false);// unlearn Shadow Resistance
                //~Shaman
                player->RemoveSpell(59547, false, false);// unlearn Gift of the Narru
                player->RemoveSpell(59540, false, false);// unlearn Shadow Resistance
                //~Warrior
                player->RemoveSpell(28880, false, false);// unlearn Gift of Naaru
                player->RemoveSpell(59221, false, false);// unlearn Shadow Resistance
                //Dwarven:
                player->RemoveSpell(2481, false, false);// unlearn Find Treasure
                player->RemoveSpell(20596, false, false);// unlearn Frost Resistance
                player->RemoveSpell(20595, false, false);// unlearn Gun Specialization
                player->RemoveSpell(59224, false, false);// unlearn Mace Specialization
                player->RemoveSpell(20594, false, false);// unlearn Stoneform
                //Gnome:
                player->RemoveSpell(20592, false, false);// unlearn Arcane Resistance
                player->RemoveSpell(20593, false, false);// unlearn Engineering Specialization
                player->RemoveSpell(20589, false, false);// unlearn Escape Artist
                player->RemoveSpell(20591, false, false);// unlearn Expansive Mind
                //Human:
                player->RemoveSpell(20599, false, false);// unlearn Diplomacy
                player->RemoveSpell(59752, false, false);// unlearn Every man for himself
                player->RemoveSpell(20864, false, false);// unlearn Mace Specialization
                player->RemoveSpell(58985, false, false);// unlearn Perception
                player->RemoveSpell(20597, false, false);// unlearn Sword Specialization
                player->RemoveSpell(20598, false, false);// unlearn The Human Spirit
                //Night Elf:
                player->RemoveSpell(20583, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20582, false, false);// unlearn Quickness
                player->RemoveSpell(58984, false, false);// unlearn Shadowmeld
                player->RemoveSpell(20585, false, false);// unlearn Wisp Spirit
                //~DK, Hunter, Druid, Hunter, Priest, Rogue, Warrior
                player->RemoveSpell(21009, false, false);// unlearn Elusiveness
                //Orc:
                player->RemoveSpell(20574, false, false);// unlearn Axe Specialization
                player->RemoveSpell(20573, false, false);// unlearn Hardiness
                //~DK
                player->RemoveSpell(54562, false, false);// unlearn Command
                //~DK, hunter, rogue, warrior
                player->RemoveSpell(20572, false, false);// unlearn Blood Fury
                //~Druid, mage, paladin, priest, rogue, warrior
                player->RemoveSpell(21563, false, false);// unlearn Command
                //~Hunter
                player->RemoveSpell(20576, false, false);// unlearn Command
                //~Shaman
                player->RemoveSpell(33697, false, false);// unlearn Blood fury
                player->RemoveSpell(65222, false, false);// unlearn Command
                //~Warlock
                player->RemoveSpell(33702, false, false);// unlearn Blood Fury
                player->RemoveSpell(20575, false, false);// unlearn Command
                //Tauren:
                player->RemoveSpell(20552, false, false);// unlearn Cultivation
                player->RemoveSpell(20550, false, false);// unlearn Endurance
                player->RemoveSpell(20551, false, false);// unlearn Nature Resistance
                player->RemoveSpell(20549, false, false);// unlearn Warstomp
                //Troll:
                player->RemoveSpell(20557, false, false);// unlearn Beast Slaying
                player->RemoveSpell(26297, false, false);// unlearn Berserking
                player->RemoveSpell(26290, false, false);// unlearn Bow Specialization
                player->RemoveSpell(58943, false, false);// unlearn Da Voodoo Shuffle
                player->RemoveSpell(20555, false, false);// unlearn Regeneration
                player->RemoveSpell(20558, false, false);// unlearn Throwing Specialization
                break;

            default:

                break;
            }
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_race_trait_swapAI(creature);
    }
};

void AddSC_racial_traits_npc()
{
    new Trinitycore_Race_Trait_announce();
    new npc_race_trait_swap();
}
