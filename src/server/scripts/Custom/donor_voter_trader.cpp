/*
*
* Made by mthsena
*
*
 
SET
@Entry = 900000,
@Model = 25900,
@Name = "Warpweaver";

INSERT INTO `creature_template` (`entry`, `modelid1`, `modelid2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Model, 0, @Name, "Transmogrifier", NULL, 0, 80, 80, 2, 35, 1, 1, 0, 0, 2000, 0, 1, 0, 7, 138936390, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, 'Donor_Voter_Trader');

 
*/
 
#include "Arena.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "Item.h"
#include "DBCStores.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "CharacterDatabase.h"
#include "Player.h"
#include "WorldSession.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "GossipDef.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "WorldSession.h"
#include "World.h"
 
enum Tokens
{
    DONOR_TOKEN = 24581, // Your token ID, Default: Mark of Thrallmar
    VOTER_TOKEN = 24579, // Your token ID, Default: Mark of Honor Hold
};
 
class Donor_Voter_Trader : public CreatureScript
{
public:
    Donor_Voter_Trader() : CreatureScript("Donor_Voter_Trader") { }

    class TrintyRetardsAI : public ScriptedAI
    {
    public:
        TrintyRetardsAI(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player *player)
    {
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "[Donor Points]->", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "[Voter Points]->", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "Show My Points", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "Close Window.", GOSSIP_SENDER_MAIN, 10);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }
 
    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
        uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);

        if (!player || !me)
            return true;

        QueryResult select = LoginDatabase.PQuery("SELECT dp, vp FROM auth.users WHERE id = '%u'", player->GetSession()->GetAccountId());
 
        if(!select)
        {
            player->GetSession()->SendAreaTriggerMessage("Internal error!");
            return false;
        }
 
        Field* fields = select->Fetch();
        uint32 dp = fields[0].GetUInt32();
        uint32 vp = fields[1].GetUInt32();
 
        player->PlayerTalkClass->ClearMenus();
 
        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch(action)
            {
            case 1:// Donor Points
                if (dp == 0)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have none donor point.");
                    CloseGossipMenuFor(player);
                    return false;
                }
 
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "[1 Point] For [1 Token]", GOSSIP_SENDER_MAIN, 4, "Are you sure?", 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "[10 Points] For [10 Tokens]", GOSSIP_SENDER_MAIN, 5, "Are you sure?", 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "...Back", GOSSIP_SENDER_MAIN, 11);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
                break;
 
            case 2:// Voter Points
                if (vp == 0)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have none voter point.");
                    CloseGossipMenuFor(player);
                    return false;
                }
 
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "[1 Point] For [1 Token]", GOSSIP_SENDER_MAIN, 6, "Are you sure?", 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "[10 Points] For [10 Tokens]", GOSSIP_SENDER_MAIN, 7, "Are you sure?", 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "...Back", GOSSIP_SENDER_MAIN, 11);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
                break;
 
            case 3:// Show My Points
                player->GetSession()->SendAreaTriggerMessage("Donor Points: %u", dp);
                player->GetSession()->SendAreaTriggerMessage("Voter Points: %u", vp);
                CloseGossipMenuFor(player);
                break;
 
            case 4:// [1 Donor Point] For [1 Donor Token]
                if (dp < 1)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have the required points.");
                    CloseGossipMenuFor(player);
                }
                else
                {
                    LoginDatabase.PExecute("UPDATE auth.users SET dp = '%u' -1 WHERE id = '%u'", dp, player->GetSession()->GetAccountId()); // DP Exchange 1
                    player->GetSession()->SendAreaTriggerMessage("Successfully!");
                    player->AddItem(DONOR_TOKEN, 1);
                    player->SaveToDB();
                    CloseGossipMenuFor(player);
                }
                break;
 
            case 5:// [10 Donor Points] For [10 Donor Tokens]
                if (dp < 10)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have the required points.");
                    CloseGossipMenuFor(player);
                }
                else
                {
                    LoginDatabase.PExecute("UPDATE auth.users SET dp = '%u' -10 WHERE id = '%u'", dp, player->GetSession()->GetAccountId()); // DP Exchange 10
                    player->GetSession()->SendAreaTriggerMessage("Successfully!");
                    player->AddItem(DONOR_TOKEN, 10);
                    player->SaveToDB();
                    CloseGossipMenuFor(player);
                }
                break;
 
            case 6:// [1 Voter Point] For [1 Voter Token]
                if (vp < 1)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have the required points.");
                    CloseGossipMenuFor(player);
                }
                else
                {
                    LoginDatabase.PExecute("UPDATE auth.users SET vp = '%u' -1 WHERE id = '%u'", vp, player->GetSession()->GetAccountId()); // VP Exchange 1
                    player->GetSession()->SendAreaTriggerMessage("Successfully!");
                    player->AddItem(VOTER_TOKEN, 1);
                    player->SaveToDB();
                    CloseGossipMenuFor(player);
                }
                break;
 
            case 7:// [10 Voter Points] For [10 Voter Tokens]
                if (vp < 10)
                {
                    player->GetSession()->SendAreaTriggerMessage("You don't have the required points.");
                    CloseGossipMenuFor(player);
                }
                else
                {
                    LoginDatabase.PExecute("UPDATE auth.users SET vp = '%u' -10 WHERE id = '%u'", vp, player->GetSession()->GetAccountId()); // VP Exchange 10
                    player->GetSession()->SendAreaTriggerMessage("Successfully!");
                    player->AddItem(VOTER_TOKEN, 10);
                    player->SaveToDB();
                    CloseGossipMenuFor(player);
                }
                break;
 
            case 10:// Close Window
                CloseGossipMenuFor(player);
                break;
 
            case 11: // ...Back
                OnGossipHello(player);
                break;
            }
        }
 
        return true;
    }
};

CreatureAI* GetAI(Creature* creature) const override
{
    return  new TrintyRetardsAI(creature);
}
};

void AddSC_Donor_Voter_Trader()
{
    new Donor_Voter_Trader();
}
