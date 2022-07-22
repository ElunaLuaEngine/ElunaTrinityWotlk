#ifndef _BOT_AI_H
#define _BOT_AI_H

#include "botcommon.h"

#include "CreatureAI.h"
#include "EventProcessor.h"
#include "ItemDefines.h"
#include "Position.h"

/*
NpcBot System by Trickerer (onlysuffering@gmail.com)
*/

class TeleportHomeEvent;
class TeleportFinishEvent;

enum CombatRating : uint8;
enum GossipOptionIcon : uint8;
enum MeleeHitOutcome : uint8;

struct CleanDamage;
struct CalcDamageInfo;
struct ItemTemplate;
struct PlayerClassLevelInfo;
struct SpellNonMeleeDamage;

class Aura;
class DamageInfo;
class Item;
class Spell;
class SpellCastTargets;
class Unit;
class Vehicle;

class bot_ai : public CreatureAI
{
    public:
        virtual ~bot_ai();

        bool canUpdate;

        void InitializeAI() override { Reset(); }
        //void Reset() override { }

        void JustDied(Unit*) override;
        void KilledUnit(Unit* u) override;
        void AttackStart(Unit* u) override;
        void JustEnteredCombat(Unit* u) override;
        void MoveInLineOfSight(Unit* u) override;
        void DamageDealt(Unit* victim, uint32& damage, DamageEffectType damageType) override;
        //void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override { }
        void ReceiveEmote(Player* player, uint32 emote) override;
        //void EnterEvadeMode(EvadeReason/* why*/ = EVADE_REASON_OTHER) override { }
        //void LeavingWorld() override { }
        void OnSpellStart(SpellInfo const* spellInfo) override { OnBotSpellStart(spellInfo); }

        virtual void OnBotSummon(Creature* /*summon*/) {}
        virtual void OnBotDespawn(Creature* /*summon*/) {}
        virtual void UnsummonAll() {}

        virtual void OnBotDamageDealt(Unit* /*victim*/, uint32 /*damage*/, CleanDamage const* /*cleanDamage*/, DamageEffectType /*damagetype*/, SpellInfo const* /*spellInfo*/) {}
        virtual void OnBotDispelDealt(Unit* /*dispelled*/, uint8 /*num*/) {}

        bool OnGossipHello(Player* player) override;
        bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override;
        bool OnGossipSelectCode(Player* player, uint32 menuId, uint32 gossipListId, char const* code) override;

        virtual void OnBotEnterVehicle(Vehicle const* /*vehicle*/);
        virtual void OnBotExitVehicle(Vehicle const* /*vehicle*/);
        virtual void AfterBotOwnerEnterVehicle();
        virtual void OnBotOwnerEnterVehicle(Vehicle const* /*vehicle*/);
        virtual void OnBotOwnerExitVehicle(Vehicle const* /*vehicle*/);

        Unit* SpawnVehicle(uint32 creEntry, uint32 vehEntry);
        void ChooseVehicleForEncounter(uint32 &creEntry, uint32 &vehEntry) const;

        static Position GetAbsoluteTransportPosition(WorldObject const* object);

        static const std::string& LocalizedNpcText(Player const* forPlayer, uint32 textId);

        bool OnGossipHello(Player* player, uint32 option);
        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action);
        bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code);

        Creature* GetBotsPet() const { return botPet; }

        void Evade();

        EventProcessor* GetEvents() { return &Events; }
        ObjectGuid::LowType GetBotOwnerGuid() const { return _ownerGuid; }
        Player* GetBotOwner() const { return master; }
        bool SetBotOwner(Player* newowner);
        void CheckOwnerExpiry();
        uint8 GetBotClass() const { return _botclass; }
        uint32 GetLastDiff() const { return lastdiff; }
        virtual void UpdateDeadAI(uint32 /*diff*/) {}
        void ReturnHome() { _atHome = false; }
        void CommonTimers(uint32 diff);
        void ResetBotAI(uint8 resetType);
        void KillEvents(bool force);
        bool CanRespawn() { return IAmFree(); }
        void BotMovement(BotMovementType type, Position const* pos, Unit* target = nullptr, bool generatePath = true) const;
        bool CanBotMoveVehicle() const;
        void SetBotCommandState(uint8 st, bool force = false, Position* newpos = nullptr);
        void RemoveBotCommandState(uint8 st);
        bool HasBotCommandState(uint8 st) const { return (m_botCommandState & st); }
        uint8 GetBotCommandState() const { return m_botCommandState; }
        bool IsInBotParty(Unit const* unit) const;
        bool IsInBotParty(ObjectGuid guid) const;
        bool CanBotAttack(Unit const* target, int8 byspell = 0) const;
        bool CanBotAttackOnVehicle() const;
        void ApplyBotDamageMultiplierMelee(uint32& damage, CalcDamageInfo& damageinfo) const;
        void ApplyBotDamageMultiplierMelee(int32& damage, SpellNonMeleeDamage& damageinfo, SpellInfo const* spellInfo, WeaponAttackType attackType, bool iscrit) const;
        void ApplyBotDamageMultiplierSpell(int32& damage, SpellNonMeleeDamage& damageinfo, SpellInfo const* spellInfo, WeaponAttackType attackType, bool iscrit) const;
        void ApplyBotDamageMultiplierHeal(Unit const* victim, float& heal, SpellInfo const* spellInfo, DamageEffectType damagetype, uint32 stack) const;
        void ApplyBotCritMultiplierAll(Unit const* victim, float& crit_chance, SpellInfo const* spellInfo, SpellSchoolMask schoolMask, WeaponAttackType attackType) const;
        void ApplyBotSpellCostMods(SpellInfo const* spellInfo, int32& cost) const;
        void ApplyBotSpellCastTimeMods(SpellInfo const* spellInfo, int32& casttime) const;
        void ApplyBotSpellCooldownMods(SpellInfo const* spellInfo, uint32& cooldown) const;
        void ApplyBotSpellCategoryCooldownMods(SpellInfo const* spellInfo, uint32& cooldown) const;
        void ApplyBotSpellGlobalCooldownMods(SpellInfo const* spellInfo, float& cooldown) const;
        void ApplyBotSpellRadiusMods(SpellInfo const* spellInfo, float& radius) const;
        void ApplyBotSpellRangeMods(SpellInfo const* spellInfo, float& maxrange) const;
        void ApplyBotSpellMaxTargetsMods(SpellInfo const* spellInfo, uint32& targets) const;
        void ApplyBotSpellChanceOfSuccessMods(SpellInfo const* spellInfo, float& chance) const;
        void ApplyBotEffectMods(WorldObject const* wtarget, SpellInfo const* spellInfo, uint8 effIndex, float& value) const;
        void ApplyBotThreatMods(SpellInfo const* spellInfo, float& threat) const;
        void ApplyBotEffectValueMultiplierMods(SpellInfo const* spellInfo, SpellEffIndex effIndex, float& multiplier) const;
        virtual uint8 GetBotStance() const;
        uint32 GetBotRoles() const { return _roleMask; }
        bool HasRole(uint32 role) const { return _roleMask & role; }
        GossipOptionIcon GetRoleIcon(uint32 role) const;
        static uint32 GetRoleString(uint32 role);
        void ToggleRole(uint32 role, bool force);
        static uint32 DefaultRolesForClass(uint8 m_class);
        bool IsTank(Unit const* unit = nullptr) const;
        bool IsOffTank(Unit const* unit = nullptr) const;

        uint32 GetLastZoneId() const { return _lastZoneId; }
        bool IsInHeroicOrRaid() const;

        bool IAmFree() const;

        static bool CCed(Unit const* target, bool root = false);

        void TeleportHome();
        bool FinishTeleport(/*uint32 mapId, uint32 instanceId, float x, float y, float z, float o*/);

        bool IsDuringTeleport() const { return teleFinishEvent || teleHomeEvent; }
        void SetTeleportFinishEvent(TeleportFinishEvent* tfevent) { ASSERT(!teleFinishEvent); teleFinishEvent = tfevent; }
        void AbortTeleport();

        uint8 GetPlayerClass() const;
        uint8 GetPlayerRace() const;

        bool IsTempBot() const;

        void SetShouldUpdateStats() { shouldUpdateStats = true; }
        void UpdateHealth() { doHealth = true; }
        void UpdateMana() { doMana = true; }

        //float GetHitRating() const { return hit; }
        int32 GetHaste() const { return haste; }
        float GetBotParryChance() const { return parry; }
        float GetBotDodgeChance() const { return dodge; }
        float GetBotBlockChance() const { return block; }
        float GetBotCritChance() const { return crit; }
        float GetBotMissChance() const { return -hit; }
        float GetBotDamageTakenMod(bool magic) const { return magic ? dmg_taken_mag : dmg_taken_phy; }
        uint32 GetBotExpertise() const { return expertise; }
        uint32 GetBotSpellPenetration() const { return spellpen; }
        uint32 GetBotSpellPower() const { return spellpower; }
        uint32 GetBotDefense() const { return defense; }
        uint32 GetShieldBlockValue() const { return blockvalue; }
        int32 GetBotResistanceBonus(SpellSchoolMask mask) const;
        int32 GetBotResistanceBonus(uint8 school) const { return (school > SPELL_SCHOOL_NORMAL && school < MAX_SPELL_SCHOOL) ? resistbonus[school-1] : 0; }
        bool CanBlock() const;
        bool CanParry() const;
        bool CanDodge() const;
        bool CanCrit() const;
        bool CanMiss() const;
        bool CanSheath() const;
        bool CanSit() const;
        virtual bool CanEat() const;
        bool CanDrink() const;
        bool CanRegenInCombat() const;
        bool CanMount() const;
        bool CanUseAmmo() const;
        bool RespectEquipsAttackTime() const;
        bool CanChangeEquip(uint8 slot) const;
        virtual bool CanSeeEveryone() const { return false; }
        virtual float GetBotArmorPenetrationCoef() const { return armor_pen; }
        virtual uint32 GetAIMiscValue(uint32 /*data*/) const { return 0; }
        virtual void SetAIMiscValue(uint32 /*data*/, uint32 /*value*/) {}
        uint8 GetBotComboPoints() const;
        float GetBotAmmoDPS() const;

        MeleeHitOutcome BotRollCustomMeleeOutcomeAgainst(Unit const* victim, WeaponAttackType attType) const;

        float GetTotalBotStat(uint8 stat) const { return _getTotalBotStat(stat); }

        Item* GetEquips(uint8 slot) const { return _equips[slot]; }
        Item* GetEquipsByGuid(ObjectGuid itemGuid) const;
        bool UnEquipAll(ObjectGuid::LowType receiver);
        float GetAverageItemLevel() const;

        void CastBotItemCombatSpell(DamageInfo const& damageInfo);
        void CastBotItemCombatSpell(DamageInfo const& damageInfo, Item* item, ItemTemplate const* proto);
        void OnBotSpellStart(SpellInfo const* spellInfo);
        void OnBotSpellInterrupted(SpellSchoolMask schoolMask, uint32 unTimeMs);
        void OnBotSpellGo(Spell const* spell, bool ok = true);
        void OnBotOwnerSpellGo(Spell const* spell, bool ok = true);
        void OnOwnerVehicleDamagedBy(Unit* attacker);
        virtual void OnClassSpellStart(SpellInfo const* /*spellInfo*/) {}
        virtual void OnClassSpellGo(SpellInfo const* /*spell*/) {}

        static void InitBotCustomSpells();
        static bool IsBotCustomSpell(uint32 spellId);

        uint32 GetReviveTimer() const { return _reviveTimer; }
        void SetReviveTimer(uint32 newtime) { _reviveTimer = newtime; }
        void UpdateReviveTimer(uint32 diff);

        uint32 GetEngageTimer() const { return _engageTimer; }
        void ResetEngageTimer(uint32 delay);

        bool HasSpell(uint32 basespell) const;
        uint32 GetBaseSpell(std::string_view spell_name, LocaleConstant locale) const;
        uint32 GetSpellCooldown(uint32 basespell) const;
        bool IsSpellReady(uint32 basespell, uint32 diff, bool checkGCD = true) const;
        void SetSpellCooldown(uint32 basespell, uint32 msCooldown);
        void SetSpellCategoryCooldown(SpellInfo const* spellInfo, uint32 msCooldown);
        void ReleaseSpellCooldown(uint32 basespell);

        virtual void SpendRunes(SpellInfo const* /*spellInfo*/, bool /*didHit*/) {}

        void ReInitFaction() { InitFaction(); }
        void ReinitOwner() { InitOwner(); }
        void SetSpec(uint8 spec, bool activate = true);
        uint8 GetSpec() const { return _spec; }
        static uint8 DefaultSpecForClass(uint8 m_class);
        static uint32 TextForSpec(uint8 spec);
        static bool IsValidSpecForClass(uint8 m_class, uint8 spec);

        static bool IsMeleeClass(uint8 m_class);
        static bool IsTankingClass(uint8 m_class);
        static bool IsBlockingClass(uint8 m_class);
        static bool IsCastingClass(uint8 m_class);
        static bool IsHealingClass(uint8 m_class);
        static bool IsHumanoidClass(uint8 m_class);
        static bool IsHeroExClass(uint8 m_class);

        AoeSpotsVec const& GetAoeSpots() const;
        static void CalculateAoeSpots(Unit const* unit, AoeSpotsVec& spots);
        void CalculateAoeSafeSpots(Unit* target, float maxdist, AoeSafeSpotsVec& safespots) const;

        //Pet stuff
        static uint32 GetPetOriginalEntry(uint32 entry);
        static bool IsPetMelee(uint32 entry);
        virtual uint8 GetPetPositionNumber(Creature const* /*summon*/) const { return 0; }

        Unit* HelpFindStunTarget(float dist = 20) const { return FindStunTarget(dist); }
        Unit* HelpFindCastingTarget(float maxdist = 10, float mindist = 0, uint32 spellId = 0, uint8 minHpPct = 0) const { return FindCastingTarget(maxdist, mindist, spellId, minHpPct); }
        Unit* HelpFindAOETarget(float dist, WorldObject const* src) const { return FindAOETarget(dist, src); }
        void HelpGetNearbyTargetsList(std::list<Unit*> &targets, float maxdist, uint8 CCoption, WorldObject const* source = nullptr) const { GetNearbyTargetsList(targets, maxdist, CCoption, source); }

        bool IsPointedTarget(Unit const* target, uint8 targetFlags) const;
        bool IsPointedHealTarget(Unit const* target) const;
        bool IsPointedTankingTarget(Unit const* target) const;
        bool IsPointedDPSTarget(Unit const* target) const;
        bool IsPointedRangedDPSTarget(Unit const* target) const;
        bool IsPointedNoDPSTarget(Unit const* target) const;
        bool IsPointedAnyAttackTarget(Unit const* target) const;

        static bool IsDamagingSpell(SpellInfo const* spellInfo);

        bool IsImmunedToMySpellEffect(Unit const* unit, SpellInfo const* spellInfo, SpellEffIndex index) const;

    protected:
        explicit bot_ai(Creature* creature);

        virtual void ReduceCD(uint32 /*diff*/) {}
        bool GlobalUpdate(uint32 diff);

        virtual bool HealTarget(Unit* /*target*/, uint32 /*diff*/) { return false; }
        virtual bool BuffTarget(Unit* /*target*/, uint32 /*diff*/) { return false; }

        void BuffAndHealGroup(uint32 diff);
        void RezGroup(uint32 REZZ);
        void CureGroup(uint32 cureSpell, uint32 diff);
        void SetStats(bool force);
        void DefaultInit();
        void InitUnitFlags(); // call only in constructor

        void OnOwnerDamagedBy(Unit* attacker);

        static uint32 InitSpell(Unit const* caster, uint32 spell);
        void InitSpellMap(uint32 basespell, bool forceadd = false, bool forwardRank = true);
        uint32 GetSpell(uint32 basespell) const;
        void ResetSpellCooldown(uint32 basespell) { SetSpellCooldown(basespell, 0); }
        void RemoveSpell(uint32 basespell);
        //void RemoveAllSpells();
        void EnableAllSpells();
        void SpellTimers(uint32 diff);
        static uint32 RaceSpellForClass(uint8 myrace, uint8 myclass);

        virtual bool CanUseManually(uint32 /*basespell*/) const { return false; }
        virtual bool HasAbilitiesSpecifics() const { return false; }
        virtual void FillAbilitiesSpecifics(Player const* /*player*/, std::list<std::string> &/*specList*/) {}

        virtual std::vector<uint32> const* GetDamagingSpellsList() const { return nullptr; }
        virtual std::vector<uint32> const* GetCCSpellsList() const       { return nullptr; }
        virtual std::vector<uint32> const* GetHealingSpellsList() const  { return nullptr; }
        virtual std::vector<uint32> const* GetSupportSpellsList() const  { return nullptr; }

        uint32 GetDPSTaken(Unit const* u) const;
        int32 GetHPSTaken(Unit const* u) const;
        int32 GetHPS(Unit const* u) const;
        int32 GetHPPCTPS(Unit const* u) const;
        uint8 GetExpectedHPPCT(Unit const* u, uint32 mseconds) const;

        void RefreshAura(uint32 spellId, int8 count = 1, Unit* target = nullptr) const;
        bool CheckAttackTarget();
        void MoveBehind(Unit const* target) const;

        void OnStartAttack(Unit const* u);
        bool StartAttack(Unit const* u, bool force = false);

        virtual void BreakCC(uint32 diff);
        void CheckRacials(uint32 diff);

        void DrinkPotion(bool mana);
        bool IsPotionReady() const;
        uint32 GetPotion(bool mana) const;

        //everything cast-related
        bool doCast(Unit* victim, uint32 spellId, bool triggered = false);
        bool doCast(Unit* victim, uint32 spellId, TriggerCastFlags flags);
        SpellCastResult CheckBotCast(Unit const* victim, uint32 spellId) const;
        virtual bool removeShapeshiftForm() { return true; }

        bool CanAffectVictim(uint32 schoolMask) const;
        bool CanRemoveReflectSpells(Unit const* target, uint32 spellId) const;

        bool IsMelee() const;

        bool IsShootingWand(Unit const* u = nullptr) const;

        bool IsChanneling(Unit const* u = nullptr) const;
        bool IsCasting(Unit const* u = nullptr) const;
        bool JumpingFlyingOrFalling() const;
        bool JumpingOrFalling() const;
        bool Jumping() const;
        bool IsInContactWithWater() const;

        float CalcSpellMaxRange(uint32 spellId, bool enemy = true) const;

        static bool IsPeriodicDynObjAOEDamage(SpellInfo const* spellInfo);
        bool IsWithinAoERadius(Position const& pos) const;

        float InitAttackRange(float origRange, bool ranged) const;
        void CalculateAttackPos(Unit* target, Position &pos, bool& force) const;
        void GetInPosition(bool force, Unit* newtarget, Position* pos = nullptr);
        bool AdjustTankingPosition(Unit const* mytarget) const;
        virtual float GetSpellAttackRange(bool longRange) const { return longRange ? 25.f : 15.f; }
        virtual void CheckAttackState();
        void OnSpellHit(Unit* caster, SpellInfo const* spell);
        void OnSpellHitTarget(Unit* /*target*/, SpellInfo const* spell);

        //Searchers
        WorldObject* GetNearbyRezTarget(float dist = 30) const;
        Unit* FindImmunityShieldDispelTarget(float dist = 30) const;
        Unit* FindHostileDispelTarget(float dist = 30, bool stealable = false) const;
        Unit* FindAffectedTarget(uint32 spellId, ObjectGuid caster = ObjectGuid::Empty, float dist = DEFAULT_VISIBILITY_DISTANCE, uint8 hostile = 0) const;
        Unit* FindPolyTarget(float dist = 30) const;
        Unit* FindFearTarget(float dist = 30) const;
        Unit* FindStunTarget(float dist = 20) const;
        Unit* FindUndeadCCTarget(float dist, uint32 spellId, bool unattacked = true) const;
        Unit* FindRootTarget(float dist, uint32 spellId) const;
        Unit* FindCastingTarget(float maxdist = 10, float mindist = 0, uint32 spellId = 0, uint8 minHpPct = 0) const;
        Unit* FindAOETarget(float dist, WorldObject const* src = nullptr) const;
        Unit* FindSplashTarget(float dist = 5, Unit* To = nullptr, float splashdist = 4) const;
        Unit* FindSplashTarget(float dist, Unit* To, float splashdist, uint8 minTargets) const;
        Unit* FindTranquilTarget(float mindist = 5, float maxdist = 35) const;
        Unit* FindDistantTauntTarget(float maxdist = 30, bool ally = false) const;
        Unit* FindDrainTarget(float maxdist = 30) const;
        void GetNearbyTargetsList(std::list<Unit*> &targets, float maxdist, uint8 CCoption, WorldObject const* source = nullptr) const;
        void GetNearbyTargetsInConeList(std::list<Unit*> &targets, float maxdist = 10) const;
        void GetNearbyFriendlyTargetsList(std::list<Unit*> &targets, float maxdist = 30) const;

        //Bot specific player-like mods hooks
        //todo remove &damage ApplyClassDamageMultiplierMelee (uint&, CalcDamageInfo&)
        virtual void ApplyClassDamageMultiplierMelee(uint32& /*damage*/, CalcDamageInfo& /*damageinfo*/) const {}
        virtual void ApplyClassDamageMultiplierMeleeSpell(int32& /*damage*/, SpellNonMeleeDamage& /*damageinfo*/, SpellInfo const* /*spellInfo*/, WeaponAttackType /*attackType*/, bool /*crit*/) const {}
        virtual void ApplyClassDamageMultiplierSpell(int32& /*damage*/, SpellNonMeleeDamage& /*damageinfo*/, SpellInfo const* /*spellInfo*/, WeaponAttackType /*attackType*/, bool /*crit*/) const {}
        virtual void ApplyClassDamageMultiplierHeal(Unit const* /*victim*/, float& /*heal*/, SpellInfo const* /*spellInfo*/, DamageEffectType /*damagetype*/, uint32 /*stack*/) const {}
        virtual void ApplyClassSpellCritMultiplierAll(Unit const* /*victim*/, float& /*crit_chance*/, SpellInfo const* /*spellInfo*/, SpellSchoolMask /*schoolMask*/, WeaponAttackType /*attackType*/) const {}
        virtual void ApplyClassSpellCostMods(SpellInfo const* /*spellInfo*/, int32& /*cost*/) const {}
        virtual void ApplyClassSpellCastTimeMods(SpellInfo const* /*spellInfo*/, int32& /*casttime*/) const {}
        virtual void ApplyClassSpellCooldownMods(SpellInfo const* /*spellInfo*/, uint32& /*cooldown*/) const {}
        virtual void ApplyClassSpellCategoryCooldownMods(SpellInfo const* /*spellInfo*/, uint32& /*cooldown*/) const {}
        virtual void ApplyClassSpellGlobalCooldownMods(SpellInfo const* /*spellInfo*/, float& /*cooldown*/) const {}
        virtual void ApplyClassSpellRadiusMods(SpellInfo const* /*spellInfo*/, float& /*radius*/) const {}
        virtual void ApplyClassSpellRangeMods(SpellInfo const* /*spellInfo*/, float& /*maxrange*/) const {}
        virtual void ApplyClassSpellMaxTargetsMods(SpellInfo const* /*spellInfo*/, uint32& /*targets*/) const {}
        virtual void ApplyClassSpellChanceOfSuccessMods(SpellInfo const* /*spellInfo*/, float& /*chance*/) const {}
        virtual void ApplyClassEffectMods(WorldObject const* /*wtarget*/, SpellInfo const* /*spellInfo*/, uint8 /*effIndex*/, float& /*value*/) const {}
        virtual void ApplyClassThreatMods(SpellInfo const* /*spellInfo*/, float& /*threat*/) const {}
        virtual void ApplyClassEffectValueMultiplierMods(SpellInfo const* /*spellInfo*/, SpellEffIndex /*effIndex*/, float& /*multiplier*/) const {}

        virtual void InitPowers() {}
        virtual void InitSpells() = 0;
        virtual void ApplyClassPassives() const = 0;
        virtual void InitHeals() {}

        void Regenerate();
        void RegenerateEnergy();
        bool Feasting() const;
        uint32 GetRation(bool drink) const;

        bool Wait();
        uint16 Rand() const;
        void GenerateRand() const;

        static uint32 GetLostHP(Unit const* unit);
        static uint8 GetHealthPCT(Unit const* u);
        static uint8 GetManaPCT(Unit const* u);

        virtual MeleeHitOutcome GetNextAttackMeleeOutCome() const;

        //event helpers
        void BotJumpInPlaceInFrontOf(Position const* pos, float speedXY, float maxHeight);

        void BuildGrouUpdatePacket(WorldPacket* data);

        void BotSay(const std::string &text, Player const* target = nullptr) const;
        void BotWhisper(const std::string &text, Player const* target = nullptr) const;
        void BotYell(const std::string &text, Player const* target = nullptr) const;
        void BotSay(std::string&& text, Player const* target = nullptr) const;
        void BotWhisper(std::string&& text, Player const* target = nullptr) const;
        void BotYell(std::string&& text, Player const* target = nullptr) const;

        void ReportSpellCast(uint32 spellId, const std::string& followedByString, Player const* target) const;

        void ApplyItemBonuses(uint8 slot);

        bool HasAuraTypeWithValueAtLeast(AuraType auratype, int32 minvalue, Unit const* unit = nullptr) const;

        void DoSkytalonVehicleStrats(uint32 diff);
        void DoRubyDrakeVehicleStrats(uint32 diff);
        void DoEmeraldDrakeVehicleStrats(uint32 diff);
        void DoAmberDrakeVehicleStrats(uint32 diff);
        void DoArgentMountVehicleStrats(uint32 diff);
        void DoDemolisherVehicleStrats(uint32 diff);
        void DoSiegeEngineVehicleStrats(uint32 diff);
        void DoChopperVehicleStrats(uint32 diff);
        void DoGenericVehicleStrats(uint32 diff);
        void DoVehicleStrats(BotVehicleStrats strat, uint32 diff);
        void DoVehicleActions(uint32 diff);
        bool CheckVehicleAttackTarget(BotVehicleStrats /*strat*/);
        bool HasVehicleRoleOverride(uint32 role) const;
        float GetVehicleAttackDistanceOverride() const;
        uint8 LivingVehiclesCount(uint32 entry = 0) const;

        bool ProcessImmediateNonAttackTarget();

        Player* master;
        Player* _prevRRobin;
        Unit* opponent;
        Creature* botPet;
        EventProcessor Events;
        ObjectGuid aftercastTargetGuid;
        uint32 GC_Timer;

        uint8 _botclass;
        uint8 _spec, _newspec;

        BotVehicleStrats curVehStrat;
        uint8 vehcomboPoints;
        bool shouldEnterVehicle;

    private:
        void FindMaster();

        void _OnHealthUpdate() const;
        void _OnManaUpdate() const;
        void _OnManaRegenUpdate() const;

        void _UpdateWMOArea();
        void _OnZoneUpdate(uint32 zoneId, uint32 areaId);
        void _OnAreaUpdate(uint32 areaId);

        void RemoveItemBonuses(uint8 slot);
        void RemoveItemEnchantments(Item const* item);
        void RemoveItemEnchantment(Item const* item, EnchantmentSlot eslot);
        void RemoveItemClassEnchants();
        void ApplyItemEnchantments(Item* item, uint8 slot);
        void ApplyItemEnchantment(Item* item, EnchantmentSlot eslot, uint8 slot);
        void ApplyItemEquipSpells(Item* item, bool apply);
        void ApplyItemEquipEnchantmentSpells(Item* item);
        void ApplyItemSetBonuses(Item* item, bool apply);
        void ApplyItemsSpells();

        bool IsPotionSpell(uint32 spellId) const;
        void StartPotionTimer();

        void BotJump(Position const* pos);
        bool UpdateImpossibleChase(Unit const* target);
        void ResetChaseTimer(Position const* pos);
        void ResetChase(Position const* pos);

        void ApplyRacials();
        void InitRoles();
        void InitSpec();
        void InitEquips();
        void InitOwner();
        void InitFaction();
        void InitRace();

        bool _canCureTarget(Unit const* target, uint32 cureSpell) const;
        void _getBotDispellableAuraList(Unit const* target, uint32 dispelMask, std::list<Aura const*> &dispelList) const;
        void _calculatePos(Position& pos) const;
        void _updateMountedState();
        void _updateStandState() const;
        void _updateRations();
        void _updateEquips(uint8 slot, Item* item);

        uint32 _getLootQualityMask() const;
        uint32 _getLootQualityThreshold() const;
        bool _canLootItemForPlayer(Player* player, Creature* creature, uint8 slot) const;
        bool _canLootCreatureForPlayer(Player* player, Creature* creature, uint32 lootQualityMask, uint32 lootThreshold) const;
        bool _canLootCreature(Creature* creature) const;
        void _autoLootCreatureGold(Creature* creature) const;
        void _autoLootCreatureItems(Player* receiver, Creature* creature, uint32 lootQualityMask, uint32 lootThreshold) const;
        void _autoLootCreature(Creature* creature);

        bool _canUseOffHand() const;
        bool _canUseRanged() const;
        bool _canUseRelic() const;
        bool _canEquip(Item const* newItem, uint8 slot, bool ignoreItemLevel = false) const;
        bool _unequip(uint8 slot, ObjectGuid::LowType receiver);
        bool _equip(uint8 slot, Item* newItem, ObjectGuid::LowType receiver);
        bool _resetEquipment(uint8 slot, ObjectGuid::LowType receiver);

        void _castBotItemUseSpell(Item const* item, SpellCastTargets const& targets/*, uint8 cast_count = 0, uint32 glyphIndex = 0*/);

        Unit* _getTarget(bool byspell, bool ranged, bool &reset) const;
        Unit* _getVehicleTarget(BotVehicleStrats strat) const;
        void _listAuras(Player const* player, Unit const* unit) const;
        bool _checkImmunities(Unit const* target, SpellInfo const* spellInfo) const;
        static float _getAttackDistance(float distance) { return distance*0.72f; }
        void _extendAttackRange(float& dist) const;

        //for moved
        void GetHomePosition(uint16& mapid, Position* pos) const;

        //utilities
        void _AddItemTemplateLink(Player const* forPlayer, ItemTemplate const* item, std::ostringstream &str) const;
        void _AddItemLink(Player const* forPlayer, Item const* item, std::ostringstream &str, bool addIcon = true) const;
        void _AddQuestLink(Player const* forPlayer, Quest const* quest, std::ostringstream &str) const;
        void _AddWeaponSkillLink(Player const* forPlayer, SpellInfo const* spellInfo, std::ostringstream &str, uint32 skillid) const;
        void _AddSpellLink(Player const* forPlayer, SpellInfo const* spellInfo, std::ostringstream &str, bool color = true) const;
        void _AddProfessionLink(Player const* forPlayer, SpellInfo const* spellInfo, std::ostringstream &str, uint32 skillId) const;
        void _LocalizeItem(Player const* forPlayer, std::string &itemName, uint32 entry) const;
        void _LocalizeItem(Player const* forPlayer, std::string &itemName, std::string &suffix, Item const* item) const;
        void _LocalizeQuest(Player const* forPlayer, std::string &questTitle, uint32 entry) const;
        void _LocalizeCreature(Player const* forPlayer, std::string &creatureName, uint32 entry) const;
        void _LocalizeGameObject(Player const* forPlayer, std::string &gameobjectName, uint32 entry) const;
        void _LocalizeSpell(Player const* forPlayer, std::string &spellName, uint32 entry) const;

        float _getBotStat(uint8 slot, uint8 stat) const;
        float _getTotalBotStat(uint8 stat) const;
        float _getRatingMultiplier(CombatRating cr) const;

        float _getStatScore(uint8 stat) const;
        float _getItemGearScore(Item const* item, uint8 forslot) const;

        void _saveStats();

        PlayerClassLevelInfo* _classinfo;
        SpellInfo const* m_botSpellInfo;
        Position movepos, attackpos;

        uint8 m_botCommandState;

        //stats
        float hit, parry, dodge, block, crit, dmg_taken_phy, dmg_taken_mag, armor_pen;
        uint32 expertise, spellpower, spellpen, defense, blockvalue;
        int32 haste, resistbonus[6];

        //timers
        uint32 _reviveTimer, _powersTimer, _chaseTimer, _engageTimer, _potionTimer;
        uint32 lastdiff, checkAurasTimer, checkMasterTimer, roleTimer, ordersTimer, regenTimer, _updateTimerMedium, _updateTimerEx1;
        uint32 _wmoAreaUpdateTimer;
        uint32 waitTimer;
        //save timers
        uint32 _saveDisabledSpellsTimer;

        uint32 _lastZoneId, _lastAreaId, _lastWMOAreaId;

        uint8 _jumpCount, _evadeCount;
        uint32 _roleMask;
        ObjectGuid::LowType _ownerGuid;
        ObjectGuid _lastTargetGuid;
        bool doHealth, doMana, shouldUpdateStats;
        bool feast_health, feast_mana;
        bool spawned;
        bool firstspawn;
        bool _evadeMode;
        bool _atHome;

        float _energyFraction;

        //counters (this session)
        uint16 _deathsCount;
        uint16 _killsCount;
        uint16 _pvpKillsCount;
        uint16 _playerKillsCount;

        //save flags
        bool _saveDisabledSpells;

        TeleportHomeEvent* teleHomeEvent;
        TeleportFinishEvent* teleFinishEvent;

        struct BotSpell
        {
            BotSpell() : spellId(0), cooldown(0), enabled(true) {}
            BotSpell(BotSpell const&) = delete;
            BotSpell(BotSpell&&) = delete;
            BotSpell& operator=(BotSpell const&) = delete;
            BotSpell& operator=(BotSpell&&) = delete;
            uint32 spellId;
            uint32 cooldown;
            bool enabled;
        };

        typedef int32 ItemStatBonus[MAX_BOT_ITEM_MOD];
        ItemStatBonus _stats[BOT_INVENTORY_SIZE];
        Item* _equips[BOT_INVENTORY_SIZE];

    public:
        typedef std::unordered_map<uint32 /*firstrankspellid*/, BotSpell* /*spell*/> BotSpellMap;
        BotSpellMap const& GetSpellMap() const { return _spells; }

    private:
        BotSpellMap _spells;

    public:
        //much simplier than SmartAI I guess...
        struct BotOrder
        {
            friend class bot_ai;

            enum { ORDERS_PARAMS_MAX_SIZE = sizeof(uint64) + sizeof(uint32) };
            union
            {
                char whole[ORDERS_PARAMS_MAX_SIZE];
                struct
                {
                    uint64 targetGuid;
                    uint32 baseSpell;
                } spellCastParams;
            } params;

            explicit BotOrder(BotOrderTypes order_type) : _type(order_type)
            {
                memset(params.whole, 0, ORDERS_PARAMS_MAX_SIZE);
            }
            BotOrder(BotOrder&&) noexcept = default;

            BotOrder(BotOrder const&) = delete;
            BotOrder& operator=(BotOrder const&) = delete;
            BotOrder& operator=(BotOrder&&) = delete;

        private:
            BotOrderTypes _type;
        };

        bool HasOrders() const { return !_orders.empty(); }
        std::size_t GetOrdersCount() const { return _orders.size(); }
        bool AddOrder(BotOrder&& order);
        void CancelOrder(BotOrder const& order);
        void CompleteOrder(BotOrder const& order);
        void CancelAllOrders();

    private:
        void _ProcessOrders();

        typedef std::queue<BotOrder> OrdersQueue;
        OrdersQueue _orders;
};

#endif
