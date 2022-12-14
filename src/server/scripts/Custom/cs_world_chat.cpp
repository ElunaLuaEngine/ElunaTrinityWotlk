/*
* orginal code by ??
* developed for last rev && added more option by irancore.ir
*
* import to database - auth - allow to player use cmd
REPLACE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES (195, 525);
* import to database - world - Passive report chat cencured to gms
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES (17000, 'Player |cff00ff00%s|r Chat Cencured : |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
* My Discord : Ac_Dev#8552
*/
#include "Rbac.h"
#include "Config.h"
#include "WorldSession.h"
#include "Player.h"
#include "Chat.h"
#include "World.h"
#include "DatabaseEnv.h"
#define Playedtimetochat 1500
#define mutetimecencure 300
#define mutetimeantispam 10

#define FACTION_SPECIFIC 0
using namespace std;
const char* CLASS_ICON;
const char* RACE_ICON;

std::string GetNameLink(Player* player)
{
    std::string name = player->GetName();
    std::string color;
    std::string icon;
    switch (player->GetRace())
    {
        // Done - Bloodelf
    case RACE_BLOODELF:
        if (player->GetGender() == GENDER_MALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Bloodelf_Male:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Bloodelf_Female:15|t";
        break;
        // Done - Dranei
    case RACE_DRAENEI:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Draenei_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Draenei_Male:15|t";
        break;
    case RACE_DWARF:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Dwarf_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Dwarf_Male:15|t";
        break;
        // Done - Gnome
    case RACE_GNOME:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Gnome_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Gnome_Male:15|t";
        break;
        // Done - Human
    case RACE_HUMAN:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Human_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Human_Male:15|t";
        break;
    case RACE_NIGHTELF:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Nightelf_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Nightelf_Male:15|t";
        break;
    case RACE_ORC:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Orc_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Orc_Male:15|t";
        break;
        // Done - Tauren
    case RACE_TAUREN:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Tauren_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Tauren_Male:15|t";
        break;
    case RACE_TROLL:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Troll_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Troll_Male:15|t";
        break;
    case RACE_UNDEAD_PLAYER:
        if (player->GetGender() == GENDER_FEMALE)
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Undead_Female:15|t";
        else
            RACE_ICON = "|TInterface/ICONS/Achievement_Character_Undead_Male:15|t";
        break;
    }

    switch (player->GetClass())
    {
    case CLASS_DEATH_KNIGHT:
        color = "|cffC41F3B";
        //CLASS_ICON = "|TInterface\\icons\\Spell_Deathknight_ClassIcon:15|t|r";
        break;
    case CLASS_DRUID:
        color = "|cffFF7D0A";
        // CLASS_ICON = "|TInterface\\icons\\Ability_Druid_Maul:15|t|r";
        break;
    case CLASS_HUNTER:
        color = "|cffABD473";
        // CLASS_ICON = "|TInterface\\icons\\INV_Weapon_Bow_07:15|t|r";
        break;
    case CLASS_MAGE:
        color = "|cff69CCF0";
        //CLASS_ICON = "|TInterface\\icons\\INV_Staff_13:15|t|r";
        break;
    case CLASS_PALADIN:
        color = "|cffF58CBA";
        // CLASS_ICON = "|TInterface\\icons\\INV_Hammer_01:15|t|r";
        break;
    case CLASS_PRIEST:
        color = "|cffFFFFFF";
        // CLASS_ICON = "|TInterface\\icons\\INV_Staff_30:15|t|r";
        break;
    case CLASS_ROGUE:
        color = "|cffFFF569";
        // CLASS_ICON = "|TInterface\\icons\\INV_ThrowingKnife_04:15|t|r";
        break;
    case CLASS_SHAMAN:
        color = "|cff0070DE";
        // CLASS_ICON = "|TInterface\\icons\\Spell_Nature_BloodLust:15|t|r";
        break;
    case CLASS_WARLOCK:
        color = "|cff9482C9";
        //  CLASS_ICON = "|TInterface\\icons\\Spell_Nature_FaerieFire:15|t|r";
        break;
    case CLASS_WARRIOR:
        color = "|cffC79C6E";
        // CLASS_ICON = "|TInterface\\icons\\INV_Sword_27.png:15|t|r";
        break;
    }
    return "|Hplayer:" + name + "|h" + RACE_ICON + "|cffFFFFFF[" + color + name + "|cffFFFFFF]|h|r";
}

void OnLogin(Player *player, bool firstLogin) {
    if (firstLogin) {
        ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Advanced World Chat |rmodule. Use .world");
    }
}
		 
void _SendWorldChat(Player* player, string message)
{
    size_t stringpos;

    if (message.find("|TInterface") != string::npos)
        return;

    if (message.find("\n") != string::npos)
        return;

    if ((stringpos = message.find("|Hquest:")) != string::npos)
        return;

    if ((stringpos = message.find("|Htrade:")) != string::npos)
        return;

    if ((stringpos = message.find("|Htalent:")) != string::npos)
        return;

    if ((stringpos = message.find("|Henchant:")) != string::npos)
        return;

    if ((stringpos = message.find("|Hachievement:")) != string::npos)
        return;

    if ((stringpos = message.find("|Hglyph:")) != string::npos)
        return;

    if ((stringpos = message.find("|Hspell:")) != string::npos)
        return;

    if ((stringpos = message.find("Hitem:")) != string::npos)
        return;

    if (message.find("|c") != string::npos && message.find("|r") != string::npos)
        return;

    if (message.find("|c") != string::npos && message.find("|h") != string::npos)
        return;

    uint8 cheksSize = 118;//Change these if you want to add more words to the array.
    std::string checks[118];//Change these if you want to add more words to the array.
    // Strony (Sites)
    // Strony (Sites)
    checks[0] = "http://";
    checks[1] = ".com";
    checks[2] = ".net";
    checks[3] = ".org";
    checks[4] = "www.";
    checks[5] = "wow-";
    checks[6] = "-wow";
    checks[7] = "no-ip";
    checks[8] = ".zapto";
    checks[9] = ".biz";
    checks[10] = ".servegame";
    checks[11] = ".ir";
    checks[12] = "com.br";

    checks[13] = "h t t p : / /";
    checks[14] = ". c o m";
    checks[15] = ". n e t";
    checks[16] = ". o r g";
    checks[17] = "w w w .";
    checks[18] = " w o w -";
    checks[19] = "- w o w";
    checks[20] = "n o - i p";
    checks[21] = ". z a p t o";
    checks[22] = ". b i z";
    checks[23] = ". s e r v e g a m e";
    checks[24] = ". b r";
    checks[25] = "c o m . b r";

    checks[26] = "h  t  t  p  :  /  /";
    checks[27] = ".  c  o  m";
    checks[28] = ".  n  e  t";
    checks[29] = ".  o  r  g";
    checks[30] = "w  w  w  .";
    checks[31] = " w  o  w  -";
    checks[32] = "-  w  o  w";
    checks[33] = "n  o  -  i  p";
    checks[34] = ".  z  a  p  t  o";
    checks[35] = ".  b  i  z";
    checks[36] = ".  s  e  r  v  e  g  a  m  e";
    checks[37] = ".  b  r";
    checks[38] = "c  o  m  .  b  r";

    checks[39] = "h   t   t   p   :   /   /";
    checks[40] = ".   c   o   m";
    checks[41] = ".   n   e   t";
    checks[42] = ".   o   r   g";
    checks[43] = "w   w   w   .";
    checks[44] = " w   o   w   -";
    checks[45] = "-   w   o   w";
    checks[46] = "n   o   -   i   p";
    checks[47] = ".   z   a   p   t   o";
    checks[48] = ".   b   i   z";
    checks[49] = ".   s   e   r   v   e   g   a   m   e";
    checks[50] = ".   b   r";
    checks[51] = "   c   o   m   .   b   r";

    checks[52] = "h    t    t    p   :   /   /";
    checks[53] = ".    c    o    m";
    checks[54] = ".    n    e   t";
    checks[55] = ".    o    r    g";
    checks[56] = "w    w    w    .";
    checks[57] = "w    o    w    -";
    checks[58] = "-    w    o    w";
    checks[59] = "n    o    -    i    p";
    checks[60] = ".    z    a    p    t    o";
    checks[61] = ".    b    i     z";
    checks[62] = ".    s    e    r    v    e    g    a    m    e";
    checks[63] = ".    b    r";
    checks[64] = "c    o    m    .    b    r";

    checks[65] = "trevon";
    checks[66] = "megawow";
    checks[67] = "fatalwow";
    checks[68] = "uniforgiven-wow";
    checks[69] = "wow-autolouco";
    checks[70] = "heaven-wow";
    checks[71] = "fireballwow";
    checks[72] = "wowbrasilpa";
    checks[73] = "fatalitywow";
    checks[74] = "demonic-wow";
    checks[75] = "revenge-wow";
    checks[76] = "heavenwow";
    checks[77] = "logon.";
    checks[78] = "linebr";
    checks[79] = "azralon";
    checks[80] = "ultra";
    checks[81] = "ultra-wow";

    checks[82] = "t r e v o n";
    checks[83] = "m e g a w o w";
    checks[84] = "f a t a l w o w";
    checks[85] = "u n i f o r g i v e n - w o w";
    checks[86] = "w o w - a u t o l o u c o";
    checks[87] = "h e a v e n - w o w";
    checks[88] = "f i r e b a l l w o w";
    checks[89] = "w o w b r a s i l  p a";
    checks[90] = "f a t a l i t y w o w";
    checks[91] = "d e m o n i c - w o w";
    checks[92] = "r e v e n g e - w o w";
    checks[93] = "h e a v e n w o w";
    checks[94] = "u n d e a d - w o w";
    checks[95] = "l i n e b r";
    checks[96] = "a z r a l o n";
    checks[97] = "b l a c k - w o w";
    checks[98] = "t r e v o n w o w";

    checks[99] = "t  r  e  v  o  n";
    checks[100] = "m  e  g  a  w  o  w";
    checks[101] = "f  a  t  a  l  w  o  w";
    checks[102] = "u  n  i  f  o  r  g  i  v  e  n  -  w  o  w";
    checks[103] = "w  o  w  -  a  u  t  o   l o  u  c  o";
    checks[104] = "h  e  a  v  e  n  -  w  o  w";
    checks[105] = "f  i  r  e  b  a  l  l  w  o  w";
    checks[106] = "w  o  w  b  r  a  s  i  l  p  a";
    checks[107] = "f  a  t  a  l  i  t  y  w  o  w";
    checks[108] = "d  e  m  o  n  i  c  -  w  o  w";
    checks[109] = "r  e  v  e  n  g  e  -  w  o  w";
    checks[110] = "h  e  a  v  e  n  w  o  w";
    checks[111] = "u  n  d  e  a  d  -  w  o  w";
    checks[112] = "l  i  n  e  b  r";
    checks[113] = "a  z  r  a  l  o  n";
    checks[114] = "b  l  a  c  k  -  w  o  w";
    checks[115] = "t  r  e  v  o  n  w  o  w";

    checks[116] = " [The Lightbringer's Redemption]"; // old source code will crashed with this macro we cencured this
    checks[117] = "[The Lightbringer's Redemption]"; // old source code will crashed with this macro we cencured this

    for (int i = 0; i < cheksSize; ++i)
    {
        if (message.find(checks[i]) != string::npos)
        {
            std::string say = "";
            std::string str = "";
            say = message;
            sWorld->SendGMText(17000, player->GetName().c_str(), say.c_str()); // sned passive report to gm
            say = "";
            ChatHandler(player->GetSession()).PSendSysMessage("Links or Bad Words are not allowed on the server.");
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
            int64 muteTime = time(NULL) + mutetimecencure; // muted player if use bad words
            player->GetSession()->m_muteTime = muteTime;
            mt->setInt64(0, muteTime);
            return;
        }
    }

    string msg;
    ostringstream chat_string;

    if (player->GetTotalPlayedTime() <= Playedtimetochat) // New If - Played Time Need For Use This Cmd
    {
        std::string adStr = secsToTimeString(Playedtimetochat - player->GetTotalPlayedTime());
        player->GetSession()->SendNotification("You Must %s seconds played To use world chat!", adStr.c_str());
        return;
    }

    // if you have vip script can enable this option and will work eazy like amdwow
    /*switch (player->GetSession()->GetVipLevel()) // vip cases aded
    {
    case 1: // Vip Rank 1
        msg += "|cffffffff[VIP Bronze]";
        break;
    case 2: // Vip Rank 1
        msg += "|cffbbbbbb[VIP Silver]";
        break;
    case 3: // Vip Rank 1
        msg += "|cffff00ff[VIP Gold]";
        break;
    case 4: // Vip Rank 1
        msg += "|cffff6060[VIP Platinum]";
        break;
    case 5: // Vip Rank 1
        msg += "|cff0000ff[VIP Diamond]";
        break;
    case 6: // Vip Rank 1
        msg += "|cffff0000[VIP Warlord]";
        break;
    }*/
    switch (player->GetSession()->GetSecurity())
    {
        // Player
    case SEC_PLAYER:
        if (player->GetTeam() == ALLIANCE)
        {
            msg += "|cff00ff00[World] ";
            msg += "|cfffa9900[Player]";
            msg += "|cff0000ff|TInterface\\pvpframe\\pvp-currency-alliance:17|t|r ";
            msg += GetNameLink(player);
            msg += ":|cfffaeb00";
        }

        else
        {
            msg += "|cff00ff00[World] ";
            msg += "|cfffa9900[Player]";
            msg += "|cffff0000|TInterface\\pvpframe\\pvp-currency-horde:17|t|r ";
            msg += GetNameLink(player);
            msg += ":|cfffaeb00";
        }
        break;
        // Moderator
    case SEC_MODERATOR:
        msg += "|cff00ff00[World]";
        msg += "|cffFF8C00[Moderator]";
        msg += " |TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:15|t|r ";
        msg += GetNameLink(player);
        msg += ":|cfffaeb00";
        break;
    case SEC_GAMEMASTER:
        msg += "|cff00ff00[World]";
        msg += "|cffFF8C00[GM]";
        msg += " |TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:15|t|r ";
        msg += GetNameLink(player);
        msg += ":|cfffaeb00";
        break;
    case SEC_ADMINISTRATOR:
        msg += "|cff00ff00[World]";
        msg += "|cffFF8C00[Head GM]";
        msg += " |TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:15|t|r ";
        msg += GetNameLink(player);
        msg += ":|cfffaeb00";
        break;
    case SEC_CONSOLE:
        msg += "|cff00ff00[World]";
        msg += "|cffFF0000[ConSOle]";
        msg += " |TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:15|t|r ";
        msg += GetNameLink(player);
        msg += ":|cfffaeb00";
        break;
    }

    chat_string << msg << " " << message;

    char c_msg[1024];

    snprintf(c_msg, 1024, chat_string.str().c_str());

    if (FACTION_SPECIFIC)
    {
        SessionMap sessions = sWorld->GetAllSessions();
        for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
            if (Player* plr = itr->second->GetPlayer())
                if (plr->GetTeam() == player->GetTeam())
                    sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str(), plr);
    }
    else
        sWorld->SendGlobalText(c_msg, NULL);
}

class cs_world_chat : public CommandScript
{
public:
    cs_world_chat() : CommandScript("cs_world_chat") {}

    static bool HandleWorldChatCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        _SendWorldChat(handler->GetSession()->GetPlayer(), args);
        // add mute time for stop spam
        LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
        int64 muteTime = time(NULL) + mutetimeantispam;
        player->GetSession()->m_muteTime = muteTime;
        mt->setInt64(0, muteTime);
        return true;
    }
    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> cs_world_chat =
        {
            {"chat", rbac::RBAC_PERM_COMMAND_SAVE, true, &HandleWorldChatCommand, "" },
            {"c", rbac::RBAC_PERM_COMMAND_SAVE, true, &HandleWorldChatCommand, "" },
            {"world", rbac::RBAC_PERM_COMMAND_SAVE, true, &HandleWorldChatCommand, "" },
        };

        return cs_world_chat;
    }
};
void AddSC_cs_world_chat()
{
    new cs_world_chat;
}
