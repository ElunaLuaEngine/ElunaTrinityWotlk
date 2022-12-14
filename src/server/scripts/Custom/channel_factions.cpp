#include "ScriptPCH.h"
#include "Channel.h"
#include "Player.h"
#include <sstream>

class channel_factions : public PlayerScript
{
     public:
         channel_factions() : PlayerScript("channel_factions") { }

     void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
     {
         if (!player || !channel)
             return;

         std::stringstream ssMsg;
         ssMsg << ((player->GetTeam() == HORDE) ? "|TInterface\\PVPFrame\\PVP-Currency-Horde:18:18:-3:-3|t" : "|TInterface\\PVPFrame\\PVP-Currency-Alliance:18:18:-3:-3|t") << msg;
         msg = ssMsg.str();
     }
};

void AddSC_channel_factions()
{
     new channel_factions();
}
