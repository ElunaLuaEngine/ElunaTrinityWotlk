#include "ScriptMgr.h"
#include "Player.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Config.h"
#include "Chat.h"

#define Welcome_Name "Notice"

class StartGuild : public PlayerScript
{

public:
	StartGuild() : PlayerScript("StartGuild") { }

	// Announce Module
	void OnLogin(Player *player, bool firstLogin) 
	{
		if (sConfigMgr->GetBoolDefault("StartGuild.Enable", true))
		{
            if (firstLogin) {
                if (sConfigMgr->GetBoolDefault("StartGuild.Announce", true))
                {
                    ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00StartGuild |rmodule.");
                }
            }
		}
	}

	// Start Guild
	void OnFirstLogin(Player* player)
	{
		// If enabled...
		if (sConfigMgr->GetBoolDefault("StartGuild.Enable", true))
		{
			const uint32 GUILD_ID_ALLIANCE = sConfigMgr->GetIntDefault("StartGuild.Alliance", 0);
			const uint32 GUILD_ID_HORDE = sConfigMgr->GetIntDefault("StartGuild.Horde", 0);

			Guild* guild = sGuildMgr->GetGuildById(player->GetTeamId() == TEAM_ALLIANCE ? GUILD_ID_ALLIANCE : GUILD_ID_HORDE);

			// If a guild is present, assign the character to the guild; otherwise skip assignment.
			if (guild)
			{
                ObjectGuid playerGuid = player->GetGUID();
                // player's guild membership checked in AddMember before add
                CharacterDatabaseTransaction trans(nullptr);
                guild->AddMember(trans, playerGuid);

				// Inform the player they have joined the guild	
				std::ostringstream ss;
				ss << "Welcome to the " << player->GetGuildName() << " guild " << player->GetName() << "!";
				ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
			}
		}
	}
};

void AddStartGuildScripts()
{
	new StartGuild();
}
