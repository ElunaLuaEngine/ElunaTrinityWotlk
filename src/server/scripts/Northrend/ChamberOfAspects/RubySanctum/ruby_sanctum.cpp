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
#include "MotionMaster.h"
#include "Player.h"
#include "ruby_sanctum.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"

enum RubySanctumTexts
{
    SAY_XERESTRASZA_EVENT       = 0,
    SAY_XERESTRASZA_EVENT_1     = 1,
    SAY_XERESTRASZA_EVENT_2     = 2,
    SAY_XERESTRASZA_EVENT_3     = 3,
    SAY_XERESTRASZA_EVENT_4     = 4,
    SAY_XERESTRASZA_EVENT_5     = 5,
    SAY_XERESTRASZA_EVENT_6     = 6,
    SAY_XERESTRASZA_EVENT_7     = 7,
    SAY_XERESTRASZA_INTRO       = 8,
};

enum RubySanctumEvents
{
    EVENT_XERESTRASZA_EVENT_1   = 1,
    EVENT_XERESTRASZA_EVENT_2   = 2,
    EVENT_XERESTRASZA_EVENT_3   = 3,
    EVENT_XERESTRASZA_EVENT_4   = 4,
    EVENT_XERESTRASZA_EVENT_5   = 5,
    EVENT_XERESTRASZA_EVENT_6   = 6,
    EVENT_XERESTRASZA_EVENT_7   = 7,
};

enum RubySanctumSpells
{
    SPELL_RALLY                 = 75416
};

Position const xerestraszaMovePos = {3151.236f, 379.8733f, 86.31996f, 0.0f};

// 40429 - Sanctum Guardian Xerestrasza
struct npc_xerestrasza : public ScriptedAI
{
    npc_xerestrasza(Creature* creature) : ScriptedAI(creature)
    {
        _isIntro = true;
        _introDone = false;
    }

    void Reset() override
    {
        _events.Reset();
        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_BALTHARUS_DEATH)
        {
            me->setActive(true);
            me->SetFarVisible(true);
            _isIntro = false;

            Talk(SAY_XERESTRASZA_EVENT);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(0, xerestraszaMovePos);

            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_1, 16s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_2, 25s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_3, 32s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_4, 42s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_5, 51s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_6, 61s);
            _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_7, 69s);
        }
        else if (action == ACTION_INTRO_BALTHARUS && !_introDone)
        {
            _introDone = true;
            Talk(SAY_XERESTRASZA_INTRO);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (_isIntro)
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_XERESTRASZA_EVENT_1:
                    Talk(SAY_XERESTRASZA_EVENT_1);
                    break;
                case EVENT_XERESTRASZA_EVENT_2:
                    Talk(SAY_XERESTRASZA_EVENT_2);
                    break;
                case EVENT_XERESTRASZA_EVENT_3:
                    Talk(SAY_XERESTRASZA_EVENT_3);
                    break;
                case EVENT_XERESTRASZA_EVENT_4:
                    Talk(SAY_XERESTRASZA_EVENT_4);
                    break;
                case EVENT_XERESTRASZA_EVENT_5:
                    Talk(SAY_XERESTRASZA_EVENT_5);
                    break;
                case EVENT_XERESTRASZA_EVENT_6:
                    Talk(SAY_XERESTRASZA_EVENT_6);
                    break;
                case EVENT_XERESTRASZA_EVENT_7:
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    Talk(SAY_XERESTRASZA_EVENT_7);
                    me->setActive(false);
                    me->SetFarVisible(false);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    bool _isIntro;
    bool _introDone;
};

class at_baltharus_plateau : public OnlyOnceAreaTriggerScript
{
    public:
        at_baltharus_plateau() : OnlyOnceAreaTriggerScript("at_baltharus_plateau") { }

        bool TryHandleOnce(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            // Only trigger once
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* xerestrasza = instance->GetCreature(DATA_XERESTRASZA))
                    xerestrasza->AI()->DoAction(ACTION_INTRO_BALTHARUS);

                if (Creature* baltharus = instance->GetCreature(DATA_BALTHARUS_THE_WARBORN))
                    baltharus->AI()->DoAction(ACTION_INTRO_BALTHARUS);
            }

            return true;
        }
};

// 75415 - Rallying Shout
class spell_ruby_sanctum_rallying_shout : public SpellScript
{
    PrepareSpellScript(spell_ruby_sanctum_rallying_shout);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_RALLY });
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _targetCount = targets.size();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (_targetCount && !GetCaster()->HasAura(SPELL_RALLY))
        {
            CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
            args.AddSpellMod(SPELLVALUE_AURA_STACK, _targetCount);
            GetCaster()->CastSpell(GetCaster(), SPELL_RALLY, args);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ruby_sanctum_rallying_shout::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnEffectHit += SpellEffectFn(spell_ruby_sanctum_rallying_shout::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

    uint32 _targetCount = 0;
};

void AddSC_ruby_sanctum()
{
    RegisterRubySanctumCreatureAI(npc_xerestrasza);
    new at_baltharus_plateau();
    RegisterSpellScript(spell_ruby_sanctum_rallying_shout);
}
