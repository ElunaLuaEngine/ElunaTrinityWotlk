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
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"
#include "magtheridons_lair.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"

enum MagtheridonTexts
{
    SAY_TAUNT           = 0,
    SAY_FREE            = 1,
    SAY_SLAY            = 2,
    SAY_BANISHED        = 3,
    SAY_COLLAPSE        = 4,
    SAY_DEATH           = 5,
    EMOTE_WEAKEN        = 6,
    EMOTE_NEARLY_FREE   = 7,
    EMOTE_BREAKS_FREE   = 8,
    EMOTE_BLAST_NOVA    = 9
};

enum MagtheridonSpells
{
    // Magtheridon
    SPELL_BLAST_NOVA            = 30616,
    SPELL_CLEAVE                = 30619,
    SPELL_BLAZE_TARGET          = 30541,
    SPELL_CAMERA_SHAKE          = 36455,
    SPELL_BERSERK               = 27680,
    SPELL_QUAKE                 = 30657,
    SPELL_DEBRIS_SERVERSIDE     = 30630,

    // Player or Manticron Cube
    SPELL_SHADOW_CAGE           = 30168,
    SPELL_SHADOW_GRASP          = 30410,
    SPELL_MIND_EXHAUSTION       = 44032,

    // Hellfire Raid Trigger
    SPELL_SHADOW_GRASP_VISUAL   = 30166,

    // HellFire Channeler
    SPELL_SHADOW_CAGE_C         = 30205,
    SPELL_SHADOW_GRASP_C        = 30207,
    SPELL_SHADOW_BOLT_VOLLEY    = 30510,
    SPELL_DARK_MENDING          = 30528,
    SPELL_BURNING_ABYSSAL       = 30511,
    SPELL_SOUL_TRANSFER         = 30531,
    SPELL_FEAR                  = 30530,

    // WorldTrigger
    SPELL_DEBRIS_KNOCKDOWN      = 36449,

    // Magtheridon Room
    SPELL_DEBRIS_VISUAL         = 30632,
    SPELL_DEBRIS_DAMAGE         = 30631,

    // Target Trigger
    SPELL_BLAZE                 = 30542
};

enum MagtheridonEvents
{
    // Magtheridon
    EVENT_BERSERK = 1,
    EVENT_CLEAVE,
    EVENT_BLAZE,
    EVENT_BLAST_NOVA,
    EVENT_QUAKE,
    EVENT_START_FIGHT,
    EVENT_RELEASED,
    EVENT_COLLAPSE,
    EVENT_DEBRIS_KNOCKDOWN,
    EVENT_DEBRIS,
    EVENT_NEARLY_EMOTE,
    EVENT_TAUNT,
    // Hellfire Channelers events
    EVENT_SHADOWBOLT,
    EVENT_FEAR,
    EVENT_CHECK_FRIEND,
    EVENT_DARK_MENDING,
    EVENT_ABYSSAL
};

enum MagtheridonPhases
{
    PHASE_BANISH = 1,
    PHASE_1,
    PHASE_2,
    PHASE_3
};

enum MagtheridonMisc
{
    SUMMON_GROUP_CHANNELERS       = 1,
    ACTION_START_CHANNELERS_EVENT = 2
};

// 17257 - Magtheridon
struct boss_magtheridon : public BossAI
{
    boss_magtheridon(Creature* creature) : BossAI(creature, DATA_MAGTHERIDON), _channelersCount(5) { }

    void Reset() override
    {
        _Reset();
        DoCastSelf(SPELL_SHADOW_CAGE_C);
        me->SummonCreatureGroup(SUMMON_GROUP_CHANNELERS);
        events.SetPhase(PHASE_BANISH);
        _channelersCount = 5;
        events.ScheduleEvent(EVENT_TAUNT, 4min, 5min);
    }

    void CombatStart()
    {
        events.CancelEvent(EVENT_START_FIGHT);
        events.CancelEvent(EVENT_NEARLY_EMOTE);
        events.ScheduleEvent(EVENT_RELEASED, 6s);
        Talk(EMOTE_BREAKS_FREE, me);
        Talk(SAY_FREE);
        me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_C);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        summons.DespawnAll();
        events.Reset();
        instance->SetData(DATA_MANTICRON_CUBE, ACTION_DISABLE);
        instance->SetData(DATA_COLLAPSE, ACTION_DISABLE);
        instance->SetData(DATA_COLLAPSE_2, ACTION_DISABLE);

        _DespawnAtEvade();
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == NPC_HELLFIRE_CHANNELLER && events.IsInPhase(PHASE_1))
        {
            _channelersCount--;

            if (_channelersCount == 0)
                CombatStart();
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_START_CHANNELERS_EVENT && events.IsInPhase(PHASE_BANISH))
        {
            events.SetPhase(PHASE_1);
            Talk(EMOTE_WEAKEN, me);
            summons.DoZoneInCombat(NPC_HELLFIRE_CHANNELLER);
            events.ScheduleEvent(EVENT_START_FIGHT, 2min);
            events.ScheduleEvent(EVENT_NEARLY_EMOTE, 1min);
            events.CancelEvent(EVENT_TAUNT);
            instance->SetBossState(DATA_MAGTHERIDON, IN_PROGRESS);
            instance->SetData(DATA_CALL_WARDERS, ACTION_ENABLE);
        }
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_SHADOW_CAGE)
            Talk(SAY_BANISHED);
    }

    void DamageTaken(Unit* /*who*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPctDamaged(30, damage) && !events.IsInPhase(PHASE_3))
        {
            events.SetPhase(PHASE_3);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            Talk(SAY_COLLAPSE);
            instance->SetData(DATA_COLLAPSE, ACTION_ENABLE);
            DoCastAOE(SPELL_CAMERA_SHAKE);
            events.ScheduleEvent(EVENT_COLLAPSE, 6s);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
        _JustDied();
        instance->SetData(DATA_MANTICRON_CUBE, ACTION_DISABLE);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_SLAY);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!events.IsInPhase(PHASE_BANISH) && !events.IsInPhase(PHASE_1) && !UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCastSelf(SPELL_BERSERK);
                    break;
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    events.Repeat(10s);
                    break;
                case EVENT_BLAZE:
                    DoCastAOE(SPELL_BLAZE_TARGET, { SPELLVALUE_MAX_TARGETS, 1 });
                    events.Repeat(20s);
                    break;
                case EVENT_QUAKE:
                    DoCastAOE(SPELL_QUAKE, { SPELLVALUE_MAX_TARGETS, 5 });
                    events.Repeat(60s);
                    break;
                case EVENT_START_FIGHT:
                    CombatStart();
                    break;
                case EVENT_RELEASED:
                    me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                    me->SetImmuneToPC(false);
                    DoZoneInCombat();
                    events.SetPhase(PHASE_2);
                    instance->SetData(DATA_MANTICRON_CUBE, ACTION_ENABLE);
                    events.ScheduleEvent(EVENT_CLEAVE, 10s);
                    events.ScheduleEvent(EVENT_BLAST_NOVA, 1min);
                    events.ScheduleEvent(EVENT_BLAZE, 20s);
                    events.ScheduleEvent(EVENT_QUAKE, 35s);
                    events.ScheduleEvent(EVENT_BERSERK, 20min);
                    break;
                case EVENT_COLLAPSE:
                    instance->SetData(DATA_COLLAPSE_2, ACTION_ENABLE);
                    events.ScheduleEvent(EVENT_DEBRIS_KNOCKDOWN, 4s);
                    break;
                case EVENT_DEBRIS_KNOCKDOWN:
                    if (Creature* trigger = instance->GetCreature(DATA_WORLD_TRIGGER))
                    {
                        trigger->CastSpell(trigger, SPELL_DEBRIS_KNOCKDOWN, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(EVENT_DEBRIS, 20s);
                    }
                    break;
                case EVENT_DEBRIS:
                    DoCastAOE(SPELL_DEBRIS_SERVERSIDE);
                    events.Repeat(20s);
                    break;
                case EVENT_NEARLY_EMOTE:
                    Talk(EMOTE_NEARLY_FREE, me);
                    break;
                case EVENT_BLAST_NOVA:
                    Talk(EMOTE_BLAST_NOVA, me);
                    DoCastAOE(SPELL_BLAST_NOVA);
                    events.Repeat(55s);
                    break;
                case EVENT_TAUNT:
                    Talk(SAY_TAUNT);
                    events.Repeat(4min, 5min);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    uint8 _channelersCount;
};

// 17256 - Hellfire Channeler
struct npc_hellfire_channeler : public ScriptedAI
{
    npc_hellfire_channeler(Creature* creature) : ScriptedAI(creature), _instance(me->GetInstanceScript()), _canCastDarkMending(true)
    {
        SetBoundary(_instance->GetBossBoundary(DATA_MAGTHERIDON));
    }

    void Reset() override
    {
        _events.Reset();
        DoCastSelf(SPELL_SHADOW_GRASP_C);
        me->SetReactState(REACT_DEFENSIVE);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->InterruptNonMeleeSpells(false);

        if (Creature* magtheridon = _instance->GetCreature(DATA_MAGTHERIDON))
            magtheridon->AI()->DoAction(ACTION_START_CHANNELERS_EVENT);

        _events.ScheduleEvent(EVENT_SHADOWBOLT, 20s);
        _events.ScheduleEvent(EVENT_CHECK_FRIEND, 1s);
        _events.ScheduleEvent(EVENT_ABYSSAL, 30s);
        _events.ScheduleEvent(EVENT_FEAR, 15s, 20s);

    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastAOE(SPELL_SOUL_TRANSFER);

        // Channelers killed by "Hit Kill" need trigger combat event too. It's needed for Cata+
        if (Creature* magtheridon = _instance->GetCreature(DATA_MAGTHERIDON))
            magtheridon->AI()->DoAction(ACTION_START_CHANNELERS_EVENT);
    }

    void JustSummoned(Creature* summon) override
    {
        if (Creature* magtheridon = _instance->GetCreature(DATA_MAGTHERIDON))
            magtheridon->AI()->JustSummoned(summon);

        DoZoneInCombat(summon);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        if (_instance->GetBossState(DATA_MAGTHERIDON) == IN_PROGRESS)
            if (Creature* magtheridon = _instance->GetCreature(DATA_MAGTHERIDON))
                magtheridon->AI()->EnterEvadeMode(EVADE_REASON_OTHER);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHADOWBOLT:
                    DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
                    _events.Repeat(15s, 20s);
                    break;
                case EVENT_FEAR:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 1))
                        DoCast(target, SPELL_FEAR);
                    _events.Repeat(25s, 40s);
                    break;
                case EVENT_CHECK_FRIEND:
                    if (_canCastDarkMending)
                    {
                        if (Unit* target = DoSelectBelowHpPctFriendlyWithEntry(NPC_HELLFIRE_CHANNELLER, 30.0f, 51, false))
                        {
                            DoCast(target, SPELL_DARK_MENDING);
                            _canCastDarkMending = false;
                            _events.ScheduleEvent(EVENT_DARK_MENDING, 10s, 20s);
                        }
                    }
                    _events.Repeat(1s);
                    break;
                case EVENT_DARK_MENDING:
                    _canCastDarkMending = true;
                    break;
                case EVENT_ABYSSAL:
                    DoCastVictim(SPELL_BURNING_ABYSSAL);
                    _events.Repeat(60s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    bool _canCastDarkMending;
};

// 17516 - Magtheridon's Room
struct npc_magtheridon_room : public PassiveAI
{
    npc_magtheridon_room(Creature* creature) : PassiveAI(creature) { }

    void Reset() override
    {
        DoCastSelf(SPELL_DEBRIS_VISUAL);

        _scheduler.Schedule(5s, [this](TaskContext /*context*/)
        {
            DoCastAOE(SPELL_DEBRIS_DAMAGE);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
};

// 181713 - Manticron Cube
struct go_manticron_cube : public GameObjectAI
{
    go_manticron_cube(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasAura(SPELL_MIND_EXHAUSTION) || player->HasAura(SPELL_SHADOW_GRASP))
            return true;

        if (Creature* trigger = player->FindNearestCreature(NPC_HELFIRE_RAID_TRIGGER, 10.0f))
            trigger->CastSpell(nullptr, SPELL_SHADOW_GRASP_VISUAL);

        player->CastSpell(nullptr, SPELL_SHADOW_GRASP, true);
        return true;
    }
};

// 30541 - Blaze
class spell_magtheridon_blaze_target : public SpellScript
{
    PrepareSpellScript(spell_magtheridon_blaze_target);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_BLAZE });
    }

    void HandleAura()
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_BLAZE);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_magtheridon_blaze_target::HandleAura);
    }
};

// 30410 - Shadow Grasp
class spell_magtheridon_shadow_grasp : public AuraScript
{
    PrepareAuraScript(spell_magtheridon_shadow_grasp);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_MIND_EXHAUSTION });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->InterruptNonMeleeSpells(false);
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MIND_EXHAUSTION, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_magtheridon_shadow_grasp::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 30166 - Shadow Grasp (Visual Effect)
class spell_magtheridon_shadow_grasp_visual : public AuraScript
{
    PrepareAuraScript(spell_magtheridon_shadow_grasp_visual);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHADOW_CAGE,
            SPELL_SHADOW_GRASP_VISUAL
        });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (target->GetAuraCount(SPELL_SHADOW_GRASP_VISUAL) == 5)
            target->CastSpell(target, SPELL_SHADOW_CAGE, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_magtheridon_shadow_grasp_visual::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_magtheridon_shadow_grasp_visual::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_magtheridon()
{
    RegisterMagtheridonsLairCreatureAI(boss_magtheridon);
    RegisterMagtheridonsLairCreatureAI(npc_hellfire_channeler);
    RegisterMagtheridonsLairCreatureAI(npc_magtheridon_room);
    RegisterMagtheridonsLairGameObjectAI(go_manticron_cube);
    RegisterSpellScript(spell_magtheridon_blaze_target);
    RegisterSpellScript(spell_magtheridon_shadow_grasp);
    RegisterSpellScript(spell_magtheridon_shadow_grasp_visual);
}
