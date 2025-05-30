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
#include "mechanar.h"
#include "ScriptedCreature.h"

enum IronHandTexts
{
    SAY_AGGRO                      = 0,
    SAY_HAMMER                     = 1,
    SAY_SLAY                       = 2,
    SAY_DEATH                      = 3,
    EMOTE_HAMMER                   = 4
};

enum IronHandSpells
{
    SPELL_SHADOW_POWER             = 35322,
    SPELL_HAMMER_PUNCH             = 35326,
    SPELL_JACKHAMMER               = 35327,
    SPELL_STREAM_OF_MACHINE_FLUID  = 35311
};

enum IronHandEvents
{
    EVENT_STREAM_OF_MACHINE_FLUID   = 1,
    EVENT_JACKHAMMER,
    EVENT_SHADOW_POWER
};

// 19710 - Gatewatcher Iron-Hand
struct boss_gatewatcher_iron_hand : public BossAI
{
    boss_gatewatcher_iron_hand(Creature* creature) : BossAI(creature, DATA_GATEWATCHER_IRON_HAND) { }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, 55s);
        events.ScheduleEvent(EVENT_JACKHAMMER, 45s);
        events.ScheduleEvent(EVENT_SHADOW_POWER, 25s);
        Talk(SAY_AGGRO);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        if (roll_chance_i(50))
            Talk(SAY_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_STREAM_OF_MACHINE_FLUID:
                    DoCastVictim(SPELL_STREAM_OF_MACHINE_FLUID);
                    events.Repeat(35s, 50s);
                    break;
                case EVENT_JACKHAMMER:
                    Talk(EMOTE_HAMMER);
                    /// @todo expect cast this about 5 times in a row (?), announce it by emote only once
                    DoCastVictim(SPELL_JACKHAMMER);
                    if (roll_chance_i(50))
                        Talk(SAY_HAMMER);
                    events.Repeat(30s);
                    break;
                case EVENT_SHADOW_POWER:
                    DoCastSelf(SPELL_SHADOW_POWER);
                    events.Repeat(20s, 28s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_gatewatcher_iron_hand()
{
    RegisterMechanarCreatureAI(boss_gatewatcher_iron_hand);
}
