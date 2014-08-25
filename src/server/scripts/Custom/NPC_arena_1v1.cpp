#include "ScriptMgr.h"
#include "ArenaTeamMgr.h"
#include "Common.h"
#include "DisableMgr.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ArenaTeam.h"
#include "Language.h"

#define ARENA_1V1_MIN_LEVEL 80 // min level to create an arenateam
#define ARENA_1V1_COST 40 * 10000 // costs for create a team: 40 gold

#define CREATE_1V1_RATED_ARENA_TEAM  "|TInterface/ICONS/Achievement_FeatsOfStrength_Gladiator_10:30:30:-18:0|tCreate 1v1 rated arena team"
#define LEAVE_1V1_RATED_ARENA_TEAM   "|TInterface/ICONS/Ability_DualWield:30:30:-18:0|tLeave 1v1 rated arena team"
#define JOIN_1V1_RATED_ARENA         "|TInterface/ICONS/Ability_DualWieldSpecialization:30:30:-18:0|tJoin 1v1 rated arena"
#define DISBAND_1V1_RATED_ARENA_TEAM "|TInterface/ICONS/Ability_DualWieldSpecialization:30:30:-18:0|tDisband 1v1 rated arena team"
#define SHOW_STATISTICS              "|TInterface/ICONS/Spell_Holy_Power:30:30:-18:0|tShow statistics"

class NPC_arena_1v1 : public CreatureScript  
{
    public:
        NPC_arena_1v1() : CreatureScript("NPC_arena_1v1") { }

    bool JoinQueueArena(Player* player, Creature* creature)
    {
        if (ARENA_1V1_MIN_LEVEL > player->getLevel())
            return false;

        uint64 guid = player->GetGUID();
        uint8 arenaslot = ArenaTeam::GetSlotByType(ARENA_TEAM_1v1);
        uint8 arenatype = ARENA_TYPE_1v1;
        uint32 arenaRating = 0;
        uint32 matchmakerRating = 0;
        bool isRated = true;

        // Ignore if we already in BG or BG queue
        if (player->InBattleground())
            return false;

        // Check existance
        Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
        if (!bg)
        {
            TC_LOG_ERROR("network", "Battleground: template bg (all arenas) not found.");
            return false;
        }

        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ARENA_DISABLED);
            return false;
        }

        BattlegroundTypeId bgTypeId = bg->GetTypeID();
        BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, arenatype);
        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), player->getLevel());
        if (!bracketEntry)
            return false;

        GroupJoinBattlegroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL;

        // check if already in queue
        if (player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return false;
        // check if has free queue slots
        if (!player->HasFreeBattlegroundQueueId())
            return false;

        uint32 ateamId = 0;

        ateamId = player->GetArenaTeamId(arenaslot);
        ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(ateamId);
        if (!at)
        {
            player->GetSession()->SendNotInArenaTeamPacket(arenatype);
            return false;
        }
        // Get the team rating for queueing
        arenaRating = at->GetRating();
        matchmakerRating = arenaRating;
        // The arenateam id must match for everyone in the group

        if (arenaRating <= 0)
            arenaRating = 1;

        BattlegroundQueue &bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        bg->SetRated(isRated);

        GroupQueueInfo* ginfo = bgQueue.AddGroup(player, NULL, bgTypeId, bracketEntry, arenatype, isRated, false, arenaRating, matchmakerRating, ateamId);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        uint32 queueSlot = player->AddBattlegroundQueueId(bgQueueTypeId);

        WorldPacket data;
        // Send status packet (in queue)
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype, 0);
        player->GetSession()->SendPacket(&data);

        sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, arenatype, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());

        return true;
    }


    bool CreateArenateam(Player* player, Creature* creature)
    {
        uint8 slot = ArenaTeam::GetSlotByType(ARENA_TEAM_1v1);
        if (slot >= MAX_ARENA_SLOT)
            return false;

        // Check if player is already in an arena team
        if (player->GetArenaTeamId(slot))
        {
            player->GetSession()->SendArenaTeamCommandResult(ERR_ARENA_TEAM_CREATE_S, player->GetName(), "", ERR_ALREADY_IN_ARENA_TEAM);
            return false;
        }


        // Teamname = playername
        // if teamname exist, we have to choose another name (playername  number)
        int i = 1;
        std::stringstream teamName;
        teamName << player->GetName();
        do
        {
            if (sArenaTeamMgr->GetArenaTeamByName(teamName.str()) != NULL) // teamname exist, so choose another name
            {
                teamName.str(std::string());
                teamName << player->GetName() << i;
            }
            else
                break;
        } while (i < 100); // should never happen

        // Create arena team
        ArenaTeam* arenaTeam = new ArenaTeam();

        if (!arenaTeam->Create(player->GetGUID(), ARENA_TEAM_1v1, teamName.str(), 0, 0, 0, 0, 0))
        {
            delete arenaTeam;
            return false;
        }

        // Register arena team
        sArenaTeamMgr->AddArenaTeam(arenaTeam);
        arenaTeam->AddMember(player->GetGUID());

        ChatHandler(player->GetSession()).SendSysMessage("Your 1v1 arena team creation was successful.");

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1)) == NULL)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, CREATE_1V1_RATED_ARENA_TEAM, GOSSIP_SENDER_MAIN, 1, "Do you want to create an 1v1 rated arena team?", ARENA_1V1_COST, false);
        else
        {
            if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_1v1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, LEAVE_1V1_RATED_ARENA_TEAM, GOSSIP_SENDER_MAIN, 3);
            else
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, JOIN_1V1_RATED_ARENA, GOSSIP_SENDER_MAIN, 2);
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, DISBAND_1V1_RATED_ARENA_TEAM, GOSSIP_SENDER_MAIN, 5, "Do your want to disband your 1v1 rated arena team?", 0, false);
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, SHOW_STATISTICS, GOSSIP_SENDER_MAIN, 4);
        }
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case 1:
                {
                    if (ARENA_1V1_MIN_LEVEL <= player->getLevel())
                    {
                        if (player->GetMoney() >= ARENA_1V1_COST && CreateArenateam(player, creature))
                            player->ModifyMoney(-(int32)ARENA_1V1_COST);
                    }
                    else
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("You need level %u+ to create an 1v1 rated arena team.", ARENA_1V1_MIN_LEVEL);
                        player->CLOSE_GOSSIP_MENU();
                        return true;
                    }
                }
                break;

            case 2:
                {
                    if (JoinQueueArena(player, creature) == false)
                        ChatHandler(player->GetSession()).SendSysMessage("Something went wrong while join queue.");
                    player->CLOSE_GOSSIP_MENU();
                    return true;
                }
                break;

            case 3:
                {
                    WorldPacket Data;
                    Data << (uint8)0x1 << (uint8)0x0 << (uint32)BATTLEGROUND_AA << (uint16)0x0 << (uint8)0x0;
                    player->GetSession()->HandleBattleFieldPortOpcode(Data);
                    player->CLOSE_GOSSIP_MENU();
                    return true;
                }
                break;

            case 4:
                {
                    ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1)));
                    if (at)
                    {
                        std::stringstream s;
                        s << "Rating: |cff00FF00" << at->GetStats().Rating;
                        s << "\nRank: |cff00FF00" << at->GetStats().Rank;
                        s << "\nSeason Games: |cff00FF00" << at->GetStats().SeasonGames;
                        s << "\nSeason Wins: |cff00FF00" << at->GetStats().SeasonWins;
                        s << "\nWeek Games: |cff00FF00" << at->GetStats().WeekGames;
                        s << "\nWeek Wins: |cff00FF00" << at->GetStats().WeekWins;
                        ChatHandler(player->GetSession()).PSendSysMessage(s.str().c_str());
                    }
                }
                break;

            case 5:
                {
                    WorldPacket Data;
                    Data << (uint32)player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1));
                    player->GetSession()->HandleArenaTeamLeaveOpcode(Data);
                    ChatHandler(player->GetSession()).PSendSysMessage("Your arena team deletion was successful.");
                    player->CLOSE_GOSSIP_MENU();
                    return true;
                }
                break;
        }
        return true;
    }
};

void AddSC_NPC_arena_1v1()
{
    new NPC_arena_1v1();
}
