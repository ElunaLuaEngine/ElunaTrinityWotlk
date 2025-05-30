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
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "the_black_morass.h"

enum TemporusTexts
{
    SAY_ENTER               = 0,
    SAY_AGGRO               = 1,
    SAY_BANISH              = 2,
    SAY_SLAY                = 3,
    SAY_DEATH               = 4
};

enum TemporusSpells
{
    SPELL_HASTE             = 31458,
    SPELL_MORTAL_WOUND      = 31464,
    SPELL_WING_BUFFET       = 31475,
    SPELL_REFLECT           = 38592
};

enum TemporusEvents
{
    EVENT_HASTE             = 1,
    EVENT_MORTAL_WOUND,
    EVENT_WING_BUFFET,
    EVENT_SPELL_REFLECTION
};

// 17880 - Temporus
struct boss_temporus : public BossAI
{
    boss_temporus(Creature* creature) : BossAI(creature, TYPE_TEMPORUS) { }

    void Reset() override { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_HASTE, 15s, 23s);
        events.ScheduleEvent(EVENT_MORTAL_WOUND, 8s);
        events.ScheduleEvent(EVENT_WING_BUFFET, 25s, 35s);
        if (IsHeroic())
            events.ScheduleEvent(EVENT_SPELL_REFLECTION, 30s);

        Talk(SAY_AGGRO);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);

        instance->SetData(TYPE_RIFT, SPECIAL);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        //Despawn Time Keeper
        if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_TIME_KEEPER)
        {
            if (me->IsWithinDistInMap(who, 20.0f))
            {
                Talk(SAY_BANISH);

                Unit::DealDamage(me, who, who->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
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
                case EVENT_HASTE:
                    DoCastSelf(SPELL_HASTE);
                    events.Repeat(20s, 25s);
                    break;
                case EVENT_MORTAL_WOUND:
                    DoCastVictim(SPELL_MORTAL_WOUND);
                    events.Repeat(10s, 20s);
                    break;
                case EVENT_WING_BUFFET:
                    DoCastSelf(SPELL_WING_BUFFET);
                    events.Repeat(20s, 30s);
                    break;
                case EVENT_SPELL_REFLECTION:
                    DoCastSelf(SPELL_REFLECT);
                    events.Repeat(25s, 35s);
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

void AddSC_boss_temporus()
{
    RegisterBlackMorassCreatureAI(boss_temporus);
}
