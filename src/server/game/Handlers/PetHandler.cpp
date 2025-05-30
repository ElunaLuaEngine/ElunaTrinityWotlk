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

#include "WorldSession.h"
#include "Common.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "Log.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Pet.h"
#include "PetAI.h"
#include "PetPackets.h"
#include "Player.h"
#include "Spell.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"

void WorldSession::HandleDismissCritter(WorldPackets::Pet::DismissCritter& packet)
{
    Unit* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, packet.CritterGUID);

    if (!pet)
    {
        TC_LOG_DEBUG("entities.pet", "Vanitypet ({}) does not exist - player '{}' ({} / account: {}) attempted to dismiss it (possibly lagged out)",
            packet.CritterGUID.ToString(), GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString(), GetAccountId());
        return;
    }

    if (_player->GetCritterGUID() == pet->GetGUID())
    {
         if (pet->GetTypeId() == TYPEID_UNIT && pet->IsSummon())
             pet->ToTempSummon()->UnSummon();
    }
}

void WorldSession::HandlePetAction(WorldPacket& recvData)
{
    ObjectGuid guid1;
    uint32 data;
    ObjectGuid guid2;
    recvData >> guid1; // pet guid
    recvData >> data;
    recvData >> guid2; // tag guid

    uint32 spellid = UNIT_ACTION_BUTTON_ACTION(data);
    uint8 flag = UNIT_ACTION_BUTTON_TYPE(data); // delete = 0x07 CastSpell = C1

    // used also for charmed creature
    Unit* pet = ObjectAccessor::GetUnit(*_player, guid1);
    TC_LOG_DEBUG("entities.pet", "HandlePetAction: {} - flag: {}, spellid: {}, target: {}.", guid1.ToString(), uint32(flag), spellid, guid2.ToString());

    if (!pet)
    {
        TC_LOG_DEBUG("entities.pet", "HandlePetAction: {} doesn't exist for {} {}", guid1.ToString(), GetPlayer()->GetGUID().ToString(), GetPlayer()->GetName());
        return;
    }

    if (pet != GetPlayer()->GetFirstControlled())
    {
        TC_LOG_DEBUG("entities.pet", "HandlePetAction: {} does not belong to {} {}", guid1.ToString(), GetPlayer()->GetGUID().ToString(), GetPlayer()->GetName());
        return;
    }

    if (!pet->IsAlive())
    {
        SpellInfo const* spell = (flag == ACT_ENABLED || flag == ACT_PASSIVE) ? sSpellMgr->GetSpellInfo(spellid) : nullptr;
        if (!spell)
            return;
        if (!spell->HasAttribute(SPELL_ATTR0_CASTABLE_WHILE_DEAD))
            return;
    }

    /// @todo allow control charmed player?
    if (pet->GetTypeId() == TYPEID_PLAYER && !(flag == ACT_COMMAND && spellid == COMMAND_ATTACK))
        return;

    if (GetPlayer()->m_Controlled.size() == 1)
        HandlePetActionHelper(pet, guid1, spellid, flag, guid2);
    else
    {
        // If a pet is dismissed, m_Controlled will change
        std::vector<Unit*> controlled;
        for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
            if ((*itr)->GetEntry() == pet->GetEntry() && (*itr)->IsAlive())
                controlled.push_back(*itr);
        for (std::vector<Unit*>::iterator itr = controlled.begin(); itr != controlled.end(); ++itr)
            HandlePetActionHelper(*itr, guid1, spellid, flag, guid2);
    }
}

void WorldSession::HandlePetStopAttack(WorldPackets::Pet::PetStopAttack& packet)
{
    Unit* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, packet.PetGUID);

    if (!pet)
    {
        TC_LOG_ERROR("entities.pet", "HandlePetStopAttack: {} does not exist", packet.PetGUID.ToString());
        return;
    }

    if (pet != GetPlayer()->GetPet() && pet != GetPlayer()->GetCharmed())
    {
        TC_LOG_ERROR("entities.pet", "HandlePetStopAttack: {} isn't a pet or charmed creature of player {}",
            packet.PetGUID.ToString(), GetPlayer()->GetName());
        return;
    }

    if (!pet->IsAlive())
        return;

    pet->AttackStop();
}

void WorldSession::HandlePetActionHelper(Unit* pet, ObjectGuid guid1, uint32 spellid, uint16 flag, ObjectGuid guid2)
{
    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_DEBUG("entities.pet", "WorldSession::HandlePetAction(petGuid: {}, tagGuid: {}, spellId: {}, flag: {}): object {} is considered pet-like but doesn't have a charminfo!",
            guid1.ToString(), guid2.ToString(), spellid, flag, pet->GetGUID().ToString());
        return;
    }

    switch (flag)
    {
        case ACT_COMMAND: // 0x07
            switch (spellid)
            {
                case COMMAND_STAY: // flat = 1792 - STAY
                    pet->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                    pet->GetMotionMaster()->MoveIdle();

                    charmInfo->SetCommandState(COMMAND_STAY);
                    charmInfo->SetIsCommandAttack(false);
                    charmInfo->SetIsAtStay(true);
                    charmInfo->SetIsCommandFollow(false);
                    charmInfo->SetIsFollowing(false);
                    charmInfo->SetIsReturning(false);
                    charmInfo->SaveStayPosition();
                    break;
                case COMMAND_FOLLOW: // spellid = 1792 - FOLLOW
                    pet->AttackStop();
                    pet->InterruptNonMeleeSpells(false);
                    pet->GetMotionMaster()->MoveFollow(_player, PET_FOLLOW_DIST, pet->GetFollowAngle());

                    charmInfo->SetCommandState(COMMAND_FOLLOW);
                    charmInfo->SetIsCommandAttack(false);
                    charmInfo->SetIsAtStay(false);
                    charmInfo->SetIsReturning(true);
                    charmInfo->SetIsCommandFollow(true);
                    charmInfo->SetIsFollowing(false);
                    break;
                case COMMAND_ATTACK: // spellid = 1792 - ATTACK
                {
                    // only place where pet can be player
                    Unit* TargetUnit = ObjectAccessor::GetUnit(*_player, guid2);
                    if (!TargetUnit)
                        return;

                    if (Unit* owner = pet->GetOwner())
                        if (!owner->IsValidAttackTarget(TargetUnit))
                            return;

                    // This is true if pet has no target or has target but targets differs.
                    if (pet->GetVictim() != TargetUnit || !pet->GetCharmInfo()->IsCommandAttack())
                    {
                        if (pet->GetVictim())
                            pet->AttackStop();

                        if (pet->GetTypeId() != TYPEID_PLAYER && pet->ToCreature()->IsAIEnabled())
                        {
                            charmInfo->SetIsCommandAttack(true);
                            charmInfo->SetIsAtStay(false);
                            charmInfo->SetIsFollowing(false);
                            charmInfo->SetIsCommandFollow(false);
                            charmInfo->SetIsReturning(false);

                            CreatureAI* AI = pet->ToCreature()->AI();
                            if (PetAI* petAI = dynamic_cast<PetAI*>(AI))
                                petAI->_AttackStart(TargetUnit); // force target switch
                            else
                                AI->AttackStart(TargetUnit);

                            // 10% chance to play special pet attack talk, else growl
                            if (pet->IsPet() && pet->ToPet()->getPetType() == SUMMON_PET && pet != TargetUnit && roll_chance_i(10))
                                pet->SendPetActionSound(PET_ACTION_ATTACK);
                            else
                            {
                                // 90% chance for pet and 100% chance for charmed creature
                                pet->SendPetAIReaction(guid1);
                            }
                        }
                        else // charmed player
                        {
                            charmInfo->SetIsCommandAttack(true);
                            charmInfo->SetIsAtStay(false);
                            charmInfo->SetIsFollowing(false);
                            charmInfo->SetIsCommandFollow(false);
                            charmInfo->SetIsReturning(false);

                            pet->Attack(TargetUnit, true);
                            pet->SendPetAIReaction(guid1);
                        }
                    }
                    break;
                }
                case COMMAND_ABANDON: // abandon (hunter pet) or dismiss (summoned pet)
                    if (pet->GetCharmerGUID() == GetPlayer()->GetGUID())
                        _player->StopCastingCharm();
                    else if (pet->GetOwnerGUID() == GetPlayer()->GetGUID())
                    {
                        ASSERT(pet->GetTypeId() == TYPEID_UNIT);
                        if (pet->IsPet())
                        {
                            if (pet->ToPet()->getPetType() == HUNTER_PET)
                                GetPlayer()->RemovePet(pet->ToPet(), PET_SAVE_AS_DELETED);
                            else
                            {
                                pet->SendPetDismissSound();
                                GetPlayer()->RemovePet(pet->ToPet(), PET_SAVE_NOT_IN_SLOT);
                            }
                        }
                        else if (pet->HasUnitTypeMask(UNIT_MASK_MINION))
                        {
                            ((Minion*)pet)->UnSummon();
                        }
                    }
                    break;
                default:
                    TC_LOG_ERROR("entities.pet", "WORLD: unknown PET flag Action {} and spellid {}.", uint32(flag), spellid);
            }
            break;
        case ACT_REACTION: // 0x6
            switch (spellid)
            {
                case REACT_PASSIVE: // passive
                    pet->AttackStop();
                    [[fallthrough]];
                case REACT_DEFENSIVE: // recovery
                case REACT_AGGRESSIVE: // activete
                    if (pet->GetTypeId() == TYPEID_UNIT)
                        pet->ToCreature()->SetReactState(ReactStates(spellid));
                    break;
            }
            break;
        case ACT_DISABLED: // 0x81 spell (disabled), ignore
        case ACT_PASSIVE: // 0x01
        case ACT_ENABLED: // 0xC1 spell
        {
            Unit* unit_target = nullptr;

            if (guid2)
                unit_target = ObjectAccessor::GetUnit(*_player, guid2);

            // do not cast unknown spells
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
            if (!spellInfo)
            {
                TC_LOG_ERROR("spells.pet", "WORLD: unknown PET spell id {}", spellid);
                return;
            }

            for (SpellEffectInfo const& spellEffectInfo : spellInfo->GetEffects())
            {
                if (spellEffectInfo.TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY || spellEffectInfo.TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY || spellEffectInfo.TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                    return;
            }

            // do not cast not learned spells
            if (!pet->HasSpell(spellid) || spellInfo->IsPassive())
                return;

            // Clear the flags as if owner clicked 'attack'. AI will reset them
            // after AttackStart, even if spell failed
            if (pet->GetCharmInfo())
            {
                pet->GetCharmInfo()->SetIsAtStay(false);
                pet->GetCharmInfo()->SetIsCommandAttack(true);
                pet->GetCharmInfo()->SetIsReturning(false);
                pet->GetCharmInfo()->SetIsFollowing(false);
            }

            Spell* spell = new Spell(pet, spellInfo, TRIGGERED_NONE);

            SpellCastResult result = spell->CheckPetCast(unit_target);

            // auto turn to target unless possessed
            if (result == SPELL_FAILED_UNIT_NOT_INFRONT && !pet->isPossessed() && !pet->IsVehicle())
            {
                if (unit_target)
                {
                    if (!pet->HasSpellFocus())
                        pet->SetInFront(unit_target);
                    if (Player* player = unit_target->ToPlayer())
                        pet->SendUpdateToPlayer(player);
                }
                else if (Unit* unit_target2 = spell->m_targets.GetUnitTarget())
                {
                    if (!pet->HasSpellFocus())
                        pet->SetInFront(unit_target2);
                    if (Player* player = unit_target2->ToPlayer())
                        pet->SendUpdateToPlayer(player);
                }

                if (Unit* powner = pet->GetCharmerOrOwner())
                    if (Player* player = powner->ToPlayer())
                        pet->SendUpdateToPlayer(player);

                result = SPELL_CAST_OK;
            }

            if (result == SPELL_CAST_OK)
            {
                unit_target = spell->m_targets.GetUnitTarget();

                // 10% chance to play special pet attack talk, else growl
                // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (pet->IsPet() && pet->ToPet()->getPetType() == SUMMON_PET && pet != unit_target && roll_chance_i(10))
                    pet->SendPetActionSound(PET_ACTION_SPECIAL_SPELL);
                else
                {
                    pet->SendPetAIReaction(guid1);
                }

                if (unit_target && !GetPlayer()->IsFriendlyTo(unit_target) && !pet->isPossessed() && !pet->IsVehicle())
                {
                    // This is true if pet has no target or has target but targets differs.
                    if (pet->GetVictim() != unit_target)
                    {
                        if (CreatureAI* AI = pet->ToCreature()->AI())
                        {
                            if (PetAI* petAI = dynamic_cast<PetAI*>(AI))
                                petAI->_AttackStart(unit_target); // force victim switch
                            else
                                AI->AttackStart(unit_target);
                        }
                    }
                }

                spell->prepare(spell->m_targets);
            }
            else
            {
                if (pet->isPossessed() || pet->IsVehicle()) /// @todo: confirm this check
                    Spell::SendCastResult(GetPlayer(), spellInfo, 0, result);
                else
                    spell->SendPetCastResult(result);

                if (!pet->GetSpellHistory()->HasCooldown(spellid))
                    pet->GetSpellHistory()->ResetCooldown(spellid, true);

                spell->finish(false);
                delete spell;

                // reset specific flags in case of spell fail. AI will reset other flags
                if (pet->GetCharmInfo())
                    pet->GetCharmInfo()->SetIsCommandAttack(false);
            }
            break;
        }
        default:
            TC_LOG_ERROR("entities.pet", "WORLD: unknown PET flag Action {} and spellid {}.", uint32(flag), spellid);
    }
}

void WorldSession::HandleQueryPetName(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_PET_NAME_QUERY");

    uint32 petnumber;
    ObjectGuid petguid;

    recvData >> petnumber;
    recvData >> petguid;

    SendQueryPetNameResponse(petguid, petnumber);
}

void WorldSession::SendQueryPetNameResponse(ObjectGuid petguid, uint32 petnumber)
{
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, petguid);
    if (!pet)
    {
        WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4+1+4+1));
        data << uint32(petnumber);
        data << uint8(0);
        data << uint32(0);
        data << uint8(0);
        _player->SendDirectMessage(&data);
        return;
    }

    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4+4+pet->GetName().size()+1));
    data << uint32(petnumber);
    data << pet->GetName();
    data << uint32(pet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP));

    if (pet->IsPet() && ((Pet*)pet)->GetDeclinedNames())
    {
        data << uint8(1);
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << ((Pet*)pet)->GetDeclinedNames()->name[i];
    }
    else
        data << uint8(0);

    _player->SendDirectMessage(&data);
}

bool WorldSession::CheckStableMaster(ObjectGuid guid)
{
    // spell case or GM
    if (guid == GetPlayer()->GetGUID())
    {
        if (!GetPlayer()->IsGameMaster() && !GetPlayer()->HasAuraType(SPELL_AURA_OPEN_STABLE))
        {
            TC_LOG_DEBUG("entities.player.cheat", "{} attempt open stable in cheating way.", guid.ToString());
            return false;
        }
    }
    // stable master case
    else
    {
        if (!GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_STABLEMASTER))
        {
            TC_LOG_DEBUG("entities.player", "Stablemaster {} not found or you can't interact with him.", guid.ToString());
            return false;
        }
    }
    return true;
}

void WorldSession::HandlePetSetAction(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_PET_SET_ACTION");

    ObjectGuid petguid;
    uint8  count;

    recvData >> petguid;

    Unit* pet = ObjectAccessor::GetUnit(*_player, petguid);

    if (!pet || pet != _player->GetFirstControlled())
    {
        TC_LOG_ERROR("entities.pet", "HandlePetSetAction: Unknown {} or owner ({})", petguid.ToString(), _player->GetGUID().ToString());
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        TC_LOG_ERROR("entities.pet", "WorldSession::HandlePetSetAction: object {} is considered pet-like but doesn't have a charminfo!", pet->GetGUID().ToString());
        return;
    }

    count = (recvData.size() == 24) ? 2 : 1;

    uint32 position[2];
    uint32 data[2];
    bool move_command = false;

    for (uint8 i = 0; i < count; ++i)
    {
        recvData >> position[i];
        recvData >> data[i];

        uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

        // ignore invalid position
        if (position[i] >= MAX_UNIT_ACTION_BAR_INDEX)
            return;

        // in the normal case, command and reaction buttons can only be moved, not removed
        // at moving count == 2, at removing count == 1
        // ignore attempt to remove command|reaction buttons (not possible at normal case)
        if (act_state == ACT_COMMAND || act_state == ACT_REACTION)
        {
            if (count == 1)
                return;

            move_command = true;
        }
    }

    std::vector<Unit*> pets;
    for (Unit* controlled : _player->m_Controlled)
        if (controlled->GetEntry() == pet->GetEntry() && controlled->IsAlive())
            pets.push_back(controlled);

    for (Unit* petControlled : pets)
    {
        // check swap (at command->spell swap client remove spell first in another packet, so check only command move correctness)
        if (move_command)
        {
            uint8 act_state_0 = UNIT_ACTION_BUTTON_TYPE(data[0]);
            if (act_state_0 == ACT_COMMAND || act_state_0 == ACT_REACTION)
            {
                uint32 spell_id_0 = UNIT_ACTION_BUTTON_ACTION(data[0]);
                UnitActionBarEntry const* actionEntry_1 = charmInfo->GetActionBarEntry(position[1]);
                if (!actionEntry_1 || spell_id_0 != actionEntry_1->GetAction() ||
                    act_state_0 != actionEntry_1->GetType())
                    return;
            }

            uint8 act_state_1 = UNIT_ACTION_BUTTON_TYPE(data[1]);
            if (act_state_1 == ACT_COMMAND || act_state_1 == ACT_REACTION)
            {
                uint32 spell_id_1 = UNIT_ACTION_BUTTON_ACTION(data[1]);
                UnitActionBarEntry const* actionEntry_0 = charmInfo->GetActionBarEntry(position[0]);
                if (!actionEntry_0 || spell_id_1 != actionEntry_0->GetAction() ||
                    act_state_1 != actionEntry_0->GetType())
                    return;
            }
        }

        for (uint8 i = 0; i < count; ++i)
        {
            uint32 spell_id = UNIT_ACTION_BUTTON_ACTION(data[i]);
            uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

            TC_LOG_DEBUG("entities.pet", "Player {} has changed pet spell action. Position: {}, Spell: {}, State: 0x{:X}",
                _player->GetName(), position[i], spell_id, uint32(act_state));

            // if it's act for spell (en/disable/cast) and there is a spell given (0 = remove spell) which pet doesn't know, don't add
            if (!((act_state == ACT_ENABLED || act_state == ACT_DISABLED || act_state == ACT_PASSIVE) && spell_id && !petControlled->HasSpell(spell_id)))
            {
                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id))
                {
                    // sign for autocast
                    if (act_state == ACT_ENABLED)
                    {
                        if (petControlled->GetTypeId() == TYPEID_UNIT && petControlled->IsPet())
                            ((Pet*)petControlled)->ToggleAutocast(spellInfo, true);
                        else
                            for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
                                if ((*itr)->GetEntry() == petControlled->GetEntry())
                                    (*itr)->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, true);
                    }
                    // sign for no/turn off autocast
                    else if (act_state == ACT_DISABLED)
                    {
                        if (petControlled->GetTypeId() == TYPEID_UNIT && petControlled->IsPet())
                            ((Pet*)petControlled)->ToggleAutocast(spellInfo, false);
                        else
                            for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
                                if ((*itr)->GetEntry() == petControlled->GetEntry())
                                    (*itr)->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, false);
                    }
                }

                charmInfo->SetActionBar(position[i], spell_id, ActiveStates(act_state));
            }
        }
    }
}

void WorldSession::HandlePetRename(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_PET_RENAME");

    ObjectGuid petguid;
    uint8 isdeclined;

    std::string name;
    DeclinedName declinedname;

    recvData >> petguid;
    recvData >> name;
    recvData >> isdeclined;

    PetStable* petStable = _player->GetPetStable();
    Pet* pet = ObjectAccessor::GetPet(*_player, petguid);
    if (!pet || !pet->IsPet() || ((Pet*)pet)->getPetType() != HUNTER_PET || !pet->HasPetFlag(UNIT_PET_FLAG_CAN_BE_RENAMED) ||
        pet->GetOwnerGUID() != _player->GetGUID() || !pet->GetCharmInfo() ||
        !petStable || !petStable->CurrentPet || petStable->CurrentPet->PetNumber != pet->GetCharmInfo()->GetPetNumber())
        return;

    PetNameInvalidReason res = ObjectMgr::CheckPetName(name, GetSessionDbcLocale());
    if (res != PET_NAME_SUCCESS)
    {
        SendPetNameInvalid(res, name, nullptr);
        return;
    }

    if (sObjectMgr->IsReservedName(name))
    {
        SendPetNameInvalid(PET_NAME_RESERVED, name, nullptr);
        return;
    }

    pet->SetName(name);

    if (pet->GetOwner()->GetGroup())
        pet->GetOwner()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_NAME);

    pet->RemovePetFlag(UNIT_PET_FLAG_CAN_BE_RENAMED);

    petStable->CurrentPet->Name = name;
    petStable->CurrentPet->WasRenamed = true;

    if (isdeclined)
    {
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
        {
            recvData >> declinedname.name[i];
        }

        std::wstring wname;
        if (!Utf8toWStr(name, wname))
            return;

        if (!ObjectMgr::CheckDeclinedNames(wname, declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    if (isdeclined)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_PET_DECLINEDNAME);
        stmt->setUInt32(0, pet->GetCharmInfo()->GetPetNumber());
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_PET_DECLINEDNAME);
        stmt->setUInt32(0, pet->GetCharmInfo()->GetPetNumber());
        stmt->setUInt32(1, _player->GetGUID().GetCounter());

        for (uint8 i = 0; i < 5; i++)
            stmt->setString(i + 2, declinedname.name[i]);

        trans->Append(stmt);
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_NAME);
    stmt->setString(0, name);
    stmt->setUInt32(1, _player->GetGUID().GetCounter());
    stmt->setUInt32(2, pet->GetCharmInfo()->GetPetNumber());
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    pet->SetPetNameTimestamp(uint32(GameTime::GetGameTime())); // cast can't be helped
}

void WorldSession::HandlePetAbandon(WorldPackets::Pet::PetAbandon& packet)
{
    if (!_player->IsInWorld())
        return;

    // pet/charmed
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, packet.PetGUID);
    if (pet && pet->ToPet() && pet->ToPet()->getPetType() == HUNTER_PET)
    {
        if (pet->GetGUID() == _player->GetPetGUID())
        {
            uint32 feelty = pet->GetPower(POWER_HAPPINESS);
            pet->SetPower(POWER_HAPPINESS, feelty > 50000 ? (feelty-50000) : 0);
        }

        _player->RemovePet(pet->ToPet(), PET_SAVE_AS_DELETED);
    }
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPackets::Pet::PetSpellAutocast& packet)
{
    Creature* pet = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, packet.PetGUID);
    if (!pet)
    {
        TC_LOG_ERROR("entities.pet", "WorldSession::HandlePetSpellAutocastOpcode: Pet {} not found.", packet.PetGUID.ToString());
        return;
    }

    if (pet != _player->GetGuardianPet() && pet != _player->GetCharmed())
    {
        TC_LOG_ERROR("entities.pet", "WorldSession::HandlePetSpellAutocastOpcode: {} isn't pet of player {} ({}).",
            packet.PetGUID.ToString(), GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(packet.SpellID);
    if (!spellInfo)
    {
        TC_LOG_ERROR("spells.pet", "WorldSession::HandlePetSpellAutocastOpcode: Unknown spell id {} used by {}.", packet.SpellID, packet.PetGUID.ToString());
        return;
    }

    std::vector<Unit*> pets;
    for (Unit* controlled : _player->m_Controlled)
        if (controlled->GetEntry() == pet->GetEntry() && controlled->IsAlive())
            pets.push_back(controlled);

    for (Unit* petControlled : pets)
    {
        // do not add not learned spells/ passive spells
        if (!petControlled->HasSpell(packet.SpellID) || !spellInfo->IsAutocastable())
            return;

        CharmInfo* charmInfo = petControlled->GetCharmInfo();
        if (!charmInfo)
        {
            TC_LOG_ERROR("entities.pet", "WorldSession::HandlePetSpellAutocastOpcode: object {} is considered pet-like but doesn't have a charminfo!", petControlled->GetGUID().ToString());
            return;
        }

        if (Pet* summon = petControlled->ToPet())
            summon->ToggleAutocast(spellInfo, packet.AutocastEnabled);
        else
            charmInfo->ToggleCreatureAutocast(spellInfo, packet.AutocastEnabled);

        charmInfo->SetSpellAutocast(spellInfo, packet.AutocastEnabled);
    }
}

void WorldSession::HandlePetCastSpellOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_PET_CAST_SPELL");

    ObjectGuid guid;
    uint8 castCount;
    uint32 spellId;
    uint8 castFlags;

    recvPacket >> guid >> castCount >> spellId >> castFlags;

    TC_LOG_DEBUG("entities.pet", "WORLD: CMSG_PET_CAST_SPELL, {}, castCount: {}, spellId {}, castFlags {}", guid.ToString(), castCount, spellId, castFlags);

    // This opcode is also sent from charmed and possessed units (players and creatures)
    if (!_player->GetGuardianPet() && !_player->GetCharmed())
        return;

    Unit* caster = ObjectAccessor::GetUnit(*_player, guid);

    if (!caster || (caster != _player->GetGuardianPet() && caster != _player->GetCharmed()))
    {
        TC_LOG_ERROR("entities.pet", "HandlePetCastSpellOpcode: {} isn't pet of player {} ({}).", guid.ToString(), GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        return;
    }

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("spells.pet", "WORLD: unknown PET spell id {}", spellId);
        return;
    }

    SpellCastTargets targets;
    targets.Read(recvPacket, caster);
    HandleClientCastFlags(recvPacket, castFlags, targets);

    TriggerCastFlags triggerCastFlags = TRIGGERED_NONE;

    if (spellInfo->IsPassive())
        return;

    // cast only learned spells
    if (!caster->HasSpell(spellId))
    {
        bool allow = false;

        // allow casting of spells triggered by clientside periodic trigger auras
        if (caster->HasAuraTypeWithTriggerSpell(SPELL_AURA_PERIODIC_TRIGGER_SPELL_FROM_CLIENT, spellId))
        {
            allow = true;
            triggerCastFlags = TRIGGERED_FULL_MASK;
        }

        if (!allow)
            return;
    }

    Spell* spell = new Spell(caster, spellInfo, triggerCastFlags);
    spell->m_fromClient = true;
    spell->m_cast_count = castCount; // probably pending spell cast
    spell->InitExplicitTargets(targets);

    SpellCastResult result = spell->CheckPetCast(nullptr);

    if (result == SPELL_CAST_OK)
    {
        if (Creature* creature = caster->ToCreature())
        {
            if (Pet* pet = creature->ToPet())
            {
                // 10% chance to play special pet attack sound, else growl
                // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (pet->getPetType() == SUMMON_PET && roll_chance_i(10))
                    pet->SendPetActionSound(PET_ACTION_SPECIAL_SPELL);
                else
                    pet->SendPetAIReaction(guid);
            }
        }

        spell->prepare(spell->m_targets);
    }
    else
    {
        spell->SendPetCastResult(result);

        if (!caster->GetSpellHistory()->HasCooldown(spellId))
            caster->GetSpellHistory()->ResetCooldown(spellId, true);

        spell->finish(false);
        delete spell;
    }
}

void WorldSession::SendPetNameInvalid(uint32 error, const std::string& name, DeclinedName *declinedName)
{
    WorldPacket data(SMSG_PET_NAME_INVALID, 4 + name.size() + 1 + 1);
    data << uint32(error);
    data << name;
    if (declinedName)
    {
        data << uint8(1);
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << declinedName->name[i];
    }
    else
        data << uint8(0);
    SendPacket(&data);
}

void WorldSession::HandlePetLearnTalent(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_PET_LEARN_TALENT");

    ObjectGuid guid;
    uint32 talentId, requestedRank;
    recvData >> guid >> talentId >> requestedRank;

    _player->LearnPetTalent(guid, talentId, requestedRank);
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandleLearnPreviewTalentsPet(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Received CMSG_LEARN_PREVIEW_TALENTS_PET");

    ObjectGuid guid;
    recvData >> guid;

    uint32 talentsCount;
    recvData >> talentsCount;

    uint32 talentId, talentRank;

    // Client has max 24 talents, rounded up : 30
    uint32 const MaxTalentsCount = 30;

    for (uint32 i = 0; i < talentsCount && i < MaxTalentsCount; ++i)
    {
        recvData >> talentId >> talentRank;

        _player->LearnPetTalent(guid, talentId, talentRank);
    }

    _player->SendTalentsInfoData(true);

    recvData.rfinish();
}

void WorldSession::HandleRequestPetInfo(WorldPackets::Pet::RequestPetInfo& /*packet*/)
{
    // Handle the packet CMSG_REQUEST_PET_INFO - sent when player does ingame /reload command

    // Packet sent when player has a pet
    if (_player->GetPet())
        _player->PetSpellInitialize();
    else if (Unit* charm = _player->GetCharmed())
    {
        // Packet sent when player has a possessed unit
        if (charm->HasUnitState(UNIT_STATE_POSSESSED))
            _player->PossessSpellInitialize();
        // Packet sent when player controlling a vehicle
        else if (charm->HasUnitFlag(UNIT_FLAG_PLAYER_CONTROLLED) && charm->HasUnitFlag(UNIT_FLAG_POSSESSED))
            _player->VehicleSpellInitialize();
        // Packet sent when player has a charmed unit
        else
            _player->CharmSpellInitialize();
    }
}
