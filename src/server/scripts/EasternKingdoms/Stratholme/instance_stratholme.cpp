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

/* ScriptData
SDName: instance_stratholme
SD%Complete: 50
SDComment: In progress. Undead side 75% implemented. Save/load not implemented.
SDCategory: Stratholme
EndScriptData */

#include "ScriptMgr.h"
#include "AreaBoundary.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "EventMap.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Map.h"
#include "MotionMaster.h"
#include "Player.h"
#include "stratholme.h"
#include "Pet.h"

enum InstanceEvents
{
    EVENT_BARON_RUN         = 1,
    EVENT_SLAUGHTER_SQUARE  = 2,
    EVENT_RAT_TRAP_CLOSE    = 3,
};

enum StratholmeMisc
{
    SAY_YSIDA_SAVED         = 0
};

enum SpawnGroups
{
    SPAWN_GROUP_STR_TIMMY   = 327
};

EllipseBoundary const beforeScarletGate(Position(3671.158f, -3181.79f), 60.0f, 40.0f);

enum class StratholmeGateTrapType : uint8
{
    ScaletSide = 0,
    UndeadSide = 1
};

Position const GateTrapPos[] =              // Positions of the two Gate Traps 3919.88 -3547.34 134.269
{
    { 3612.29f, -3335.39f, 124.077f },      // Scarlet side
    { 3919.88f, -3545.34f, 134.269f }       // Undead side
};

struct GateTrapData
{
    std::array<ObjectGuid, 2> Gates;
    GuidUnorderedSet Rats;
    bool Triggered = false;
};

class instance_stratholme : public InstanceMapScript
{
    public:
        instance_stratholme() : InstanceMapScript(StratholmeScriptName, 329) { }

        struct instance_stratholme_InstanceMapScript : public InstanceScript
        {
            instance_stratholme_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    EncounterState[i] = NOT_STARTED;

                for (uint8 i = 0; i < 5; ++i)
                    IsSilverHandDead[i] = false;

                timmySpawned = false;
                scarletsKilled = 0;

                events.ScheduleEvent(EVENT_RAT_TRAP_CLOSE, 15s);
            }

            uint32 EncounterState[MAX_ENCOUNTER];
            uint8 scarletsKilled;

            bool IsSilverHandDead[5];
            bool timmySpawned;

            ObjectGuid serviceEntranceGUID;
            ObjectGuid gauntletGate1GUID;
            ObjectGuid ziggurat1GUID;
            ObjectGuid ziggurat2GUID;
            ObjectGuid ziggurat3GUID;
            ObjectGuid ziggurat4GUID;
            ObjectGuid ziggurat5GUID;
            ObjectGuid portGauntletGUID;
            ObjectGuid portSlaugtherGUID;
            ObjectGuid portElderGUID;
            ObjectGuid ysidaCageGUID;

            ObjectGuid baronGUID;
            ObjectGuid ysidaGUID;
            ObjectGuid ysidaTriggerGUID;
            GuidSet crystalsGUID;
            GuidSet abomnationGUID;
            EventMap events;

            std::array<GateTrapData, 2> TrapGates;

            void OnUnitDeath(Unit* who) override
            {
                switch (who->GetEntry())
                {
                    case NPC_CRIMSON_GUARDSMAN:
                    case NPC_CRIMSON_CONJUROR:
                    case NPC_CRIMSON_INITATE:
                    case NPC_CRIMSON_GALLANT:
                    {
                        if (!timmySpawned)
                        {
                            Position pos = who->ToCreature()->GetHomePosition();
                            // check if they're in front of the entrance
                            if (beforeScarletGate.IsWithinBoundary(pos))
                            {
                                if (++scarletsKilled >= TIMMY_THE_CRUEL_CRUSADERS_REQUIRED)
                                {
                                    instance->SpawnGroupSpawn(SPAWN_GROUP_STR_TIMMY);
                                    timmySpawned = true;
                                }
                            }
                        }
                        break;
                    }
                    case NPC_PLAGUED_RAT:
                    {
                        for (GateTrapData& trapGate : TrapGates)
                        {
                            auto el = trapGate.Rats.find(who->GetGUID());
                            if (el != trapGate.Rats.end())
                            {
                                trapGate.Rats.erase(el);
                                for (ObjectGuid gate : trapGate.Gates)
                                    UpdateGoState(gate, GO_STATE_ACTIVE);
                            }
                        }
                        break;
                    }
                }
            }

            bool StartSlaugtherSquare()
            {
                //change to DONE when crystals implemented
                if (EncounterState[1] == IN_PROGRESS && EncounterState[2] == IN_PROGRESS && EncounterState[3] == IN_PROGRESS)
                {
                    HandleGameObject(portGauntletGUID, true);
                    HandleGameObject(portSlaugtherGUID, true);
                    return true;
                }

                TC_LOG_DEBUG("scripts", "Instance Stratholme: Cannot open slaugther square yet.");
                return false;
            }

            //if restoreTime is not 0, then newState will be ignored and GO should be restored to original state after "restoreTime" millisecond
            void UpdateGoState(ObjectGuid goGuid, uint32 newState, uint32 restoreTime = 0u)
            {
                if (!goGuid)
                    return;
                if (GameObject* go = instance->GetGameObject(goGuid))
                {
                    if (restoreTime)
                        go->UseDoorOrButton(restoreTime);
                    else
                        go->SetGoState((GOState)newState);
                }
            }

            void DoGateTrap(StratholmeGateTrapType type, Unit* where)
            {
                // close the gate, but in two minutes it will open on its own
                for (ObjectGuid trapGateGuid : TrapGates[AsUnderlyingType(type)].Gates)
                    UpdateGoState(trapGateGuid, GO_STATE_READY, 20 * IN_MILLISECONDS);

                for (uint8 i = 0; i < 30; ++i)
                {
                    Position summonPos = where->GetRandomPoint(GateTrapPos[AsUnderlyingType(type)], 5.0f);
                    if (Creature* creature = where->SummonCreature(NPC_PLAGUED_RAT, summonPos, TEMPSUMMON_DEAD_DESPAWN, 0s))
                    {
                        TrapGates[AsUnderlyingType(type)].Rats.insert(creature->GetGUID());
                        creature->EngageWithTarget(where);
                    }
                }

                TrapGates[AsUnderlyingType(type)].Triggered = true;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_BARON:
                        baronGUID = creature->GetGUID();
                        break;
                    case NPC_YSIDA_TRIGGER:
                        ysidaTriggerGUID = creature->GetGUID();
                        break;
                    case NPC_CRYSTAL:
                        crystalsGUID.insert(creature->GetGUID());
                        break;
                    case NPC_ABOM_BILE:
                    case NPC_ABOM_VENOM:
                        abomnationGUID.insert(creature->GetGUID());
                        break;
                    case NPC_YSIDA:
                        ysidaGUID = creature->GetGUID();
                        creature->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_CRYSTAL:
                        crystalsGUID.erase(creature->GetGUID());
                        break;
                    case NPC_ABOM_BILE:
                    case NPC_ABOM_VENOM:
                        abomnationGUID.erase(creature->GetGUID());
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_SERVICE_ENTRANCE:
                        serviceEntranceGUID = go->GetGUID();
                        break;
                    case GO_GAUNTLET_GATE1:
                        //weird, but unless flag is set, client will not respond as expected. DB bug?
                        go->SetFlag(GO_FLAG_LOCKED);
                        gauntletGate1GUID = go->GetGUID();
                        break;
                    case GO_ZIGGURAT1:
                        ziggurat1GUID = go->GetGUID();
                        if (GetData(TYPE_BARONESS) == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_ZIGGURAT2:
                        ziggurat2GUID = go->GetGUID();
                        if (GetData(TYPE_NERUB) == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_ZIGGURAT3:
                        ziggurat3GUID = go->GetGUID();
                        if (GetData(TYPE_PALLID) == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_ZIGGURAT4:
                        ziggurat4GUID = go->GetGUID();
                        if (GetData(TYPE_BARON) == DONE || GetData(TYPE_RAMSTEIN) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_ZIGGURAT5:
                        ziggurat5GUID = go->GetGUID();
                        if (GetData(TYPE_BARON) == DONE || GetData(TYPE_RAMSTEIN) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_PORT_GAUNTLET:
                        portGauntletGUID = go->GetGUID();
                        if (GetData(TYPE_BARONESS) == IN_PROGRESS && GetData(TYPE_NERUB) == IN_PROGRESS && GetData(TYPE_PALLID) == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_PORT_SLAUGTHER:
                        portSlaugtherGUID = go->GetGUID();
                        if (GetData(TYPE_BARONESS) == IN_PROGRESS && GetData(TYPE_NERUB) == IN_PROGRESS && GetData(TYPE_PALLID) == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_PORT_ELDERS:
                        portElderGUID = go->GetGUID();
                        break;
                    case GO_YSIDA_CAGE:
                        ysidaCageGUID = go->GetGUID();
                        break;
                    case GO_PORT_TRAP_GATE_1:
                        TrapGates[AsUnderlyingType(StratholmeGateTrapType::ScaletSide)].Gates[0] = go->GetGUID();
                        break;
                    case GO_PORT_TRAP_GATE_2:
                        TrapGates[AsUnderlyingType(StratholmeGateTrapType::ScaletSide)].Gates[1] = go->GetGUID();
                        break;
                    case GO_PORT_TRAP_GATE_3:
                        TrapGates[AsUnderlyingType(StratholmeGateTrapType::UndeadSide)].Gates[0] = go->GetGUID();
                        break;
                    case GO_PORT_TRAP_GATE_4:
                        TrapGates[AsUnderlyingType(StratholmeGateTrapType::UndeadSide)].Gates[1] = go->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_BARON_RUN:
                        switch (data)
                        {
                            case IN_PROGRESS:
                                if (EncounterState[0] == IN_PROGRESS || EncounterState[0] == FAIL)
                                    break;
                                EncounterState[0] = data;
                                events.ScheduleEvent(EVENT_BARON_RUN, 45min);
                                TC_LOG_DEBUG("scripts", "Instance Stratholme: Baron run in progress.");
                                break;
                            case FAIL:
                                DoRemoveAurasDueToSpellOnPlayers(SPELL_BARON_ULTIMATUM);
                                if (Creature* ysida = instance->GetCreature(ysidaGUID))
                                    ysida->CastSpell(ysida, SPELL_PERM_FEIGN_DEATH, true);
                                EncounterState[0] = data;
                                break;
                            case DONE:
                                EncounterState[0] = data;

                                if (Creature* ysida = instance->GetCreature(ysidaGUID))
                                {
                                    if (GameObject* cage = instance->GetGameObject(ysidaCageGUID))
                                        cage->UseDoorOrButton();

                                    float x, y, z;
                                    //! This spell handles the Dead man's plea quest completion
                                    ysida->CastSpell(nullptr, SPELL_YSIDA_SAVED, true);
                                    ysida->SetWalk(true);
                                    ysida->AI()->Talk(SAY_YSIDA_SAVED);
                                    ysida->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                                    ysida->GetClosePoint(x, y, z, ysida->GetObjectScale() / 3, 4.0f);
                                    ysida->GetMotionMaster()->MovePoint(1, x, y, z);

                                    Map::PlayerList const& players = instance->GetPlayers();

                                    for (auto const& i : players)
                                    {
                                        if (Player* player = i.GetSource())
                                        {
                                            if (player->IsGameMaster())
                                                continue;

                                            //! im not quite sure what this one is supposed to do
                                            //! this is server-side spell
                                            player->CastSpell(ysida, SPELL_YSIDA_CREDIT_EFFECT, true);
                                        }
                                    }
                                }
                                events.CancelEvent(EVENT_BARON_RUN);
                                break;
                        }
                        break;
                    case TYPE_BARONESS:
                        EncounterState[1] = data;
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(ziggurat1GUID, true);
                            //change to DONE when crystals implemented
                            StartSlaugtherSquare();
                        }
                        break;
                    case TYPE_NERUB:
                        EncounterState[2] = data;
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(ziggurat2GUID, true);
                            //change to DONE when crystals implemented
                            StartSlaugtherSquare();
                        }
                        break;
                    case TYPE_PALLID:
                        EncounterState[3] = data;
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(ziggurat3GUID, true);
                            //change to DONE when crystals implemented
                            StartSlaugtherSquare();
                        }
                        break;
                    case TYPE_RAMSTEIN:
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(portGauntletGUID, false);

                            uint32 count = abomnationGUID.size();
                            for (GuidSet::const_iterator i = abomnationGUID.begin(); i != abomnationGUID.end(); ++i)
                            {
                                if (Creature* pAbom = instance->GetCreature(*i))
                                    if (!pAbom->IsAlive())
                                        --count;
                            }

                            if (!count)
                            {
                                //a bit itchy, it should close the door after 10 secs, but it doesn't. skipping it for now.
                                //UpdateGoState(ziggurat4GUID, 0, true);
                                if (Creature* pBaron = instance->GetCreature(baronGUID))
                                    pBaron->SummonCreature(NPC_RAMSTEIN, 4032.84f, -3390.24f, 119.73f, 4.71f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30min);
                                TC_LOG_DEBUG("scripts", "Instance Stratholme: Ramstein spawned.");
                            }
                            else
                                TC_LOG_DEBUG("scripts", "Instance Stratholme: {} Abomnation left to kill.", count);
                        }

                        if (data == NOT_STARTED)
                            HandleGameObject(portGauntletGUID, true);

                        if (data == DONE)
                        {
                            events.ScheduleEvent(EVENT_SLAUGHTER_SQUARE, 1min);
                            TC_LOG_DEBUG("scripts", "Instance Stratholme: Slaugther event will continue in 1 minute.");
                        }
                        EncounterState[4] = data;
                        break;
                    case TYPE_BARON:
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(ziggurat4GUID, false);
                            HandleGameObject(ziggurat5GUID, false);
                        }
                        if (data == DONE || data == NOT_STARTED)
                        {
                            HandleGameObject(ziggurat4GUID, true);
                            HandleGameObject(ziggurat5GUID, true);
                        }
                        if (data == DONE)
                        {
                            HandleGameObject(portGauntletGUID, true);
                            if (GetData(TYPE_BARON_RUN) == IN_PROGRESS)
                                DoRemoveAurasDueToSpellOnPlayers(SPELL_BARON_ULTIMATUM);

                            SetData(TYPE_BARON_RUN, DONE);
                        }
                        EncounterState[5] = data;
                        break;
                    case TYPE_SH_AELMAR:
                        IsSilverHandDead[0] = (data) ? true : false;
                        break;
                    case TYPE_SH_CATHELA:
                        IsSilverHandDead[1] = (data) ? true : false;
                        break;
                    case TYPE_SH_GREGOR:
                        IsSilverHandDead[2] = (data) ? true : false;
                        break;
                    case TYPE_SH_NEMAS:
                        IsSilverHandDead[3] = (data) ? true : false;
                        break;
                    case TYPE_SH_VICAR:
                        IsSilverHandDead[4] = (data) ? true : false;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << EncounterState[0] << ' ' << EncounterState[1] << ' ' << EncounterState[2] << ' '
                    << EncounterState[3] << ' ' << EncounterState[4] << ' ' << EncounterState[5];

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                std::istringstream loadStream(in);
                loadStream >> EncounterState[0] >> EncounterState[1] >> EncounterState[2] >> EncounterState[3]
                >> EncounterState[4] >> EncounterState[5];

                // Do not reset 1, 2 and 3. they are not set to done, yet .
                if (EncounterState[0] == IN_PROGRESS)
                    EncounterState[0] = NOT_STARTED;
                if (EncounterState[4] == IN_PROGRESS)
                    EncounterState[4] = NOT_STARTED;
                if (EncounterState[5] == IN_PROGRESS)
                    EncounterState[5] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            uint32 GetData(uint32 type) const override
            {
                  switch (type)
                  {
                      case TYPE_SH_QUEST:
                          if (IsSilverHandDead[0] && IsSilverHandDead[1] && IsSilverHandDead[2] && IsSilverHandDead[3] && IsSilverHandDead[4])
                              return 1;
                          return 0;
                      case TYPE_BARON_RUN:
                          return EncounterState[0];
                      case TYPE_BARONESS:
                          return EncounterState[1];
                      case TYPE_NERUB:
                          return EncounterState[2];
                      case TYPE_PALLID:
                          return EncounterState[3];
                      case TYPE_RAMSTEIN:
                          return EncounterState[4];
                      case TYPE_BARON:
                          return EncounterState[5];
                  }
                  return 0;
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                    case DATA_BARON:
                        return baronGUID;
                    case DATA_YSIDA_TRIGGER:
                        return ysidaTriggerGUID;
                    case NPC_YSIDA:
                        return ysidaGUID;
                }
                return ObjectGuid::Empty;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BARON_RUN:
                            if (GetData(TYPE_BARON_RUN) != DONE)
                                SetData(TYPE_BARON_RUN, FAIL);
                            TC_LOG_DEBUG("scripts", "Instance Stratholme: Baron run event reached end. Event has state {}.", GetData(TYPE_BARON_RUN));
                            break;
                        case EVENT_SLAUGHTER_SQUARE:
                            if (Creature* baron = instance->GetCreature(baronGUID))
                            {
                                for (uint8 i = 0; i < 4; ++i)
                                    baron->SummonCreature(NPC_BLACK_GUARD, 4032.84f, -3390.24f, 119.73f, 4.71f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30min);

                                HandleGameObject(ziggurat4GUID, true);
                                HandleGameObject(ziggurat5GUID, true);
                                TC_LOG_DEBUG("scripts", "Instance Stratholme: Black guard sentries spawned. Opening gates to baron.");
                            }
                            break;
                        case EVENT_RAT_TRAP_CLOSE:
                        {
                            for (uint8 i = 0; i < std::size(GateTrapPos); ++i)
                            {
                                if (TrapGates[i].Triggered)
                                    continue;

                                Position const* gateTrapPos = &GateTrapPos[i];
                                // Check that the trap is not on cooldown, if so check if player/pet is in range
                                for (MapReference const& itr : instance->GetPlayers())
                                {
                                    Player* player = itr.GetSource();
                                    if (player->IsGameMaster())
                                        continue;

                                    if (player->IsWithinDist2d(gateTrapPos, 5.5f))
                                    {
                                        DoGateTrap(StratholmeGateTrapType(i), player);
                                        break;
                                    }

                                    Pet* pet = player->GetPet();
                                    if (pet && pet->IsWithinDist2d(gateTrapPos, 5.5f))
                                    {
                                        DoGateTrap(StratholmeGateTrapType(i), pet);
                                        break;
                                    }
                                }

                            }
                            //if you haven't already fallen into the trap, update it
                            if (std::any_of(TrapGates.begin(), TrapGates.end(), [](GateTrapData const& trap) { return !trap.Triggered; }))
                                events.ScheduleEvent(EVENT_RAT_TRAP_CLOSE, 1s);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_stratholme_InstanceMapScript(map);
        }
};

void AddSC_instance_stratholme()
{
    new instance_stratholme();
}
