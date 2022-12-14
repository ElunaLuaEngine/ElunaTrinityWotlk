#ifndef __CFBGQUEUE_H
#define __CFBGQUEUE_H

#include "Common.h"
#include "DBCEnums.h"

class BattlegroundQueue;
class Battleground;

class TC_GAME_API CFBGQueue
{
public:
    static bool CheckMixedMatch(BattlegroundQueue* queue, Battleground* bg_template, BattlegroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers);
    static bool MixPlayersToBG(BattlegroundQueue* queue, Battleground* bg, BattlegroundBracketId bracket_id);
    static bool CFBGGroupInserter(BattlegroundQueue* queue, Battleground* bg_template, BattlegroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers, uint32 minplayers);
};

#endif // __CFBGQUEUE_H

