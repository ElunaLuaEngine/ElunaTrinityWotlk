#ifndef _CPLAYER_H
#define _CPLAYER_H

#include "Common.h"
#include "Player.h"

class Player;

class TC_GAME_API CFBGData
{
public:
    CFBGData(Player* player)
    {
        this->player = player;
    }

    bool NativeTeam() { return player->GetTeam() == GetOTeam(); }
    uint8 GetFRace() const { return m_fRace; }
    uint8 GetORace() const { return m_oRace; }
    uint32 GetOFaction() const { return m_oFaction; }
    uint32 GetFFaction() const { return m_fFaction; }
    uint32 GetOTeam() const { return m_oTeam; }
    void SetCFBGData();
    void ReplaceRacials();
    void ReplaceItems();
    void InitializeCFData();
    void SetRaceDisplayID();

private:
    Player* player;
    uint8 m_fRace;
    uint8 m_oRace;
    uint32 m_fFaction;
    uint32 m_oFaction;
    uint32 m_fTeam;
    uint32 m_oTeam;
};

#endif // _CPLAYER_H
