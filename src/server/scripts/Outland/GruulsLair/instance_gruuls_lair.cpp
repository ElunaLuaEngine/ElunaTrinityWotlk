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
 */

#include "ScriptMgr.h"
#include "Creature.h"
#include "gruuls_lair.h"
#include "InstanceScript.h"

DoorData const doorData[] =
{
    { GO_MAULGAR_DOOR,  DATA_MAULGAR,   DOOR_TYPE_PASSAGE },
    { GO_GRUUL_DOOR,    DATA_GRUUL,     DOOR_TYPE_ROOM },
    { 0,                0,              DOOR_TYPE_ROOM } // END
};

MinionData const minionData[] =
{
    { NPC_MAULGAR,              DATA_MAULGAR },
    { NPC_KROSH_FIREHAND,       DATA_MAULGAR },
    { NPC_OLM_THE_SUMMONER,     DATA_MAULGAR },
    { NPC_KIGGLER_THE_CRAZED,   DATA_MAULGAR },
    { NPC_BLINDEYE_THE_SEER,    DATA_MAULGAR },
    { 0, 0 }
};

ObjectData const creatureData[] =
{
    { NPC_MAULGAR,              DATA_MAULGAR },
    { 0,                        0            } // END
};

class instance_gruuls_lair : public InstanceMapScript
{
    public:
        instance_gruuls_lair() : InstanceMapScript(GLScriptName, 565) { }

        struct instance_gruuls_lair_InstanceMapScript : public InstanceScript
        {
            instance_gruuls_lair_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                LoadMinionData(minionData);
                LoadObjectData(creatureData, nullptr);
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_gruuls_lair_InstanceMapScript(map);
        }
};

void AddSC_instance_gruuls_lair()
{
    new instance_gruuls_lair();
}
