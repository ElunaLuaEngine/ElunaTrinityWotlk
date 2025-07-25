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
#include "Guild.h"
#include "GuildMgr.h"
#include "GuildPackets.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldPacket.h"

void WorldSession::HandleGuildQueryOpcode(WorldPackets::Guild::QueryGuildInfo& query)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_QUERY [{}]: Guild: {}", GetPlayerInfo(), query.GuildId);
    if (!query.GuildId)
        return;

    if (Guild* guild = sGuildMgr->GetGuildById(query.GuildId))
        guild->HandleQuery(this);
}

void WorldSession::HandleGuildCreateOpcode(WorldPackets::Guild::GuildCreate& packet)
{
    TC_LOG_ERROR("entities.player.cheat", "CMSG_GUILD_CREATE: Possible hacking-attempt: {} tried to create a guild [Name: {}] using cheats", GetPlayerInfo(), packet.GuildName);
}

void WorldSession::HandleGuildInviteOpcode(WorldPackets::Guild::GuildInviteByName& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_INVITE [{}]: Invited: {}", GetPlayerInfo(), packet.Name);
    if (normalizePlayerName(packet.Name))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleInviteMember(this, packet.Name);
}

void WorldSession::HandleGuildRemoveOpcode(WorldPackets::Guild::GuildOfficerRemoveMember& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_REMOVE [{}]: Target: {}", GetPlayerInfo(), packet.Removee);

    if (normalizePlayerName(packet.Removee))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleRemoveMember(this, packet.Removee);
}

void WorldSession::HandleGuildAcceptOpcode(WorldPackets::Guild::AcceptGuildInvite& /*invite*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ACCEPT [{}]", GetPlayer()->GetName());

    if (!GetPlayer()->GetGuildId())
        if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildIdInvited()))
            guild->HandleAcceptMember(this);
}

void WorldSession::HandleGuildDeclineOpcode(WorldPackets::Guild::GuildDeclineInvitation& /*decline*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DECLINE [{}]", GetPlayerInfo());
    if (GetPlayer()->GetGuildId())
        return;

    GetPlayer()->SetGuildIdInvited(0);
}

void WorldSession::HandleGuildInfoOpcode(WorldPackets::Guild::GuildGetInfo& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_INFO [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendInfo(this);
}

void WorldSession::HandleGuildRosterOpcode(WorldPackets::Guild::GuildGetRoster& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ROSTER [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRoster(this);
    else
        Guild::SendCommandResult(this, GUILD_COMMAND_ROSTER, ERR_GUILD_PLAYER_NOT_IN_GUILD);
}

void WorldSession::HandleGuildPromoteOpcode(WorldPackets::Guild::GuildPromoteMember& promote)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_PROMOTE [{}]: Target: {}", GetPlayerInfo(), promote.Promotee);

    if (normalizePlayerName(promote.Promotee))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleUpdateMemberRank(this, promote.Promotee, false);
}

void WorldSession::HandleGuildDemoteOpcode(WorldPackets::Guild::GuildDemoteMember& demote)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DEMOTE [{}]: Target: {}", GetPlayerInfo(), demote.Demotee);

    if (normalizePlayerName(demote.Demotee))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleUpdateMemberRank(this, demote.Demotee, true);
}

void WorldSession::HandleGuildLeaveOpcode(WorldPackets::Guild::GuildLeave& /*leave*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_LEAVE [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleLeaveMember(this);
}

void WorldSession::HandleGuildDelete(WorldPackets::Guild::GuildDelete& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DISBAND [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleDisband(this);
}

void WorldSession::HandleGuildUpdateMotdText(WorldPackets::Guild::GuildUpdateMotdText& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_MOTD [{}]: MOTD: {}", GetPlayerInfo(), packet.MotdText);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMOTD(this, packet.MotdText);
}

void WorldSession::HandleGuildSetPublicNoteOpcode(WorldPackets::Guild::GuildSetMemberNote& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_SET_PUBLIC_NOTE [{}]: Target: {}, Note: {}",
         GetPlayerInfo(), packet.NoteeName, packet.Note);

    if (normalizePlayerName(packet.NoteeName))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetMemberNote(this, packet.Note, packet.NoteeName, true);
}

void WorldSession::HandleGuildSetOfficerNoteOpcode(WorldPackets::Guild::GuildSetMemberNote& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_SET_OFFICER_NOTE [{}]: Target: {}, Note: {}",
         GetPlayerInfo(), packet.NoteeName, packet.Note);

    if (normalizePlayerName(packet.NoteeName))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetMemberNote(this, packet.Note, packet.NoteeName, false);
}

void WorldSession::HandleGuildSetRankPermissions(WorldPackets::Guild::GuildSetRankPermissions& packet)
{
    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    std::array<GuildBankRightsAndSlots, GUILD_BANK_MAX_TABS> rightsAndSlots;
    for (uint8 tabId = 0; tabId < GUILD_BANK_MAX_TABS; ++tabId)
        rightsAndSlots[tabId] = GuildBankRightsAndSlots(tabId, uint8(packet.TabFlags[tabId]), uint32(packet.TabWithdrawItemLimit[tabId]));

    TC_LOG_DEBUG("guild", "CMSG_GUILD_RANK [{}]: Rank: {} ({})", GetPlayerInfo(), packet.RankName, packet.RankID);

    guild->HandleSetRankInfo(this, packet.RankID, packet.RankName, packet.Flags, packet.WithdrawGoldLimit, rightsAndSlots);
}

void WorldSession::HandleGuildAddRankOpcode(WorldPackets::Guild::GuildAddRank& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ADD_RANK [{}]: Rank: {}", GetPlayerInfo(), packet.Name);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleAddNewRank(this, packet.Name);
}

void WorldSession::HandleGuildDeleteRank(WorldPackets::Guild::GuildDeleteRank& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DEL_RANK [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRemoveLowestRank(this);
}

void WorldSession::HandleGuildUpdateInfoText(WorldPackets::Guild::GuildUpdateInfoText& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_INFO_TEXT [{}]: {}", GetPlayerInfo(), packet.InfoText);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetInfo(this, packet.InfoText);
}

void WorldSession::HandleSaveGuildEmblemOpcode(WorldPackets::Guild::SaveGuildEmblem& packet)
{
    EmblemInfo emblemInfo;
    emblemInfo.ReadPacket(packet);

    TC_LOG_DEBUG("guild", "MSG_SAVE_GUILD_EMBLEM [{}]: Guid: [{}] Style: {}, Color: {}, BorderStyle: {}, BorderColor: {}, BackgroundColor: {}"
        , GetPlayerInfo(), packet.Vendor.ToString(), emblemInfo.GetStyle()
        , emblemInfo.GetColor(), emblemInfo.GetBorderStyle()
        , emblemInfo.GetBorderColor(), emblemInfo.GetBackgroundColor());

    if (GetPlayer()->GetNPCIfCanInteractWith(packet.Vendor, UNIT_NPC_FLAG_TABARDDESIGNER))
    {
        // Remove fake death
        if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
            GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetEmblem(this, emblemInfo);
        else
            Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_NOGUILD); // "You are not part of a guild!";
    }
    else
        Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_INVALIDVENDOR); // "That's not an emblem vendor!"
}

void WorldSession::HandleGuildEventLogQueryOpcode(WorldPackets::Guild::GuildEventLogQuery& /*packet*/)
{
    TC_LOG_DEBUG("guild", "MSG_GUILD_EVENT_LOG_QUERY [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendEventLog(this);
}

void WorldSession::HandleGuildBankMoneyWithdrawn(WorldPackets::Guild::GuildBankRemainingWithdrawMoneyQuery& /*packet*/)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendMoneyInfo(this);
}

void WorldSession::HandleGuildPermissionsQuery(WorldPackets::Guild::GuildPermissionsQuery& /* packet */)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendPermissions(this);
}

// Called when clicking on Guild bank gameobject
void WorldSession::HandleGuildBankActivate(WorldPackets::Guild::GuildBankActivate& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANKER_ACTIVATE [{}]: [{}] AllSlots: {}"
        , GetPlayerInfo(), packet.Banker.ToString(), packet.FullUpdate);

    GameObject const* const go = GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK);
    if (!go)
        return;

    Guild* const guild = GetPlayer()->GetGuild();
    if (!guild)
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_VIEW_TAB, ERR_GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    guild->SendBankList(this, 0, packet.FullUpdate);
}

// Called when opening guild bank tab only (first one)
void WorldSession::HandleGuildBankQueryTab(WorldPackets::Guild::GuildBankQueryTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_QUERY_TAB [{}]: {}, TabId: {}, ShowTabs: {}"
        , GetPlayerInfo(), packet.Banker.ToString(), packet.Tab, packet.FullUpdate);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->SendBankList(this, packet.Tab, true /*packet.FullUpdate*/);
                                                          // HACK: client doesn't query entire tab content if it had received SMSG_GUILD_BANK_LIST in this session
                                                          // but we broadcast bank updates to entire guild when *ANYONE* changes anything, incorrectly initializing clients
                                                          // tab content with only data for that change
}

void WorldSession::HandleGuildBankDepositMoney(WorldPackets::Guild::GuildBankDepositMoney& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_DEPOSIT_MONEY [{}]: [{}], money: {}",
        GetPlayerInfo(), packet.Banker.ToString(), packet.Money);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (packet.Money && GetPlayer()->HasEnoughMoney(packet.Money))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleMemberDepositMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPackets::Guild::GuildBankWithdrawMoney& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_WITHDRAW_MONEY [{}]: [{}], money: {}",
        GetPlayerInfo(), packet.Banker.ToString(), packet.Money);

    if (packet.Money && GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleMemberWithdrawMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankSwapItems(WorldPackets::Guild::GuildBankSwapItems& packet)
{
    if (!GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        return;

    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    if (packet.BankOnly)
        guild->SwapItems(GetPlayer(), packet.BankTab1, packet.BankSlot1, packet.BankTab, packet.BankSlot, packet.BankItemCount);
    else
    {
        uint8 playerBag = NULL_BAG;
        uint8 playerSlotId = NULL_SLOT;
        uint8 toChar = 1;
        uint32 splitedAmount = 0;

        if (!packet.AutoStore)
        {
            playerBag = packet.ContainerSlot;
            playerSlotId = packet.ContainerItemSlot;
            toChar = packet.ToSlot;
            splitedAmount = packet.StackCount;
        }

        // Player <-> Bank
        // Allow to work with inventory only
        if (!Player::IsInventoryPos(playerBag, playerSlotId) && !(playerBag == NULL_BAG && playerSlotId == NULL_SLOT))
            GetPlayer()->SendEquipError(EQUIP_ERR_INTERNAL_BAG_ERROR, nullptr);
        else
            guild->SwapItemsWithInventory(GetPlayer(), toChar != 0, packet.BankTab, packet.BankSlot, playerBag, playerSlotId, splitedAmount);
    }
}

void WorldSession::HandleGuildBankBuyTab(WorldPackets::Guild::GuildBankBuyTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_BUY_TAB [{}]: [{}[, TabId: {}", GetPlayerInfo(), packet.Banker .ToString(), packet.BankTab);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleBuyBankTab(this, packet.BankTab);
}

void WorldSession::HandleGuildBankUpdateTab(WorldPackets::Guild::GuildBankUpdateTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_UPDATE_TAB [{}]: [{}], TabId: {}, Name: {}, Icon: {}"
        , GetPlayerInfo(), packet.Banker.ToString(), packet.BankTab, packet.Name, packet.Icon);

    if (!packet.Name.empty() && !packet.Icon.empty())
        if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleSetBankTabInfo(this, packet.BankTab, packet.Name, packet.Icon);
}

void WorldSession::HandleGuildBankLogQuery(WorldPackets::Guild::GuildBankLogQuery& packet)
{
    TC_LOG_DEBUG("guild", "MSG_GUILD_BANK_LOG_QUERY [{}]: TabId: {}", GetPlayerInfo(), packet.Tab);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankLog(this, packet.Tab);
}

void WorldSession::HandleGuildBankTextQuery(WorldPackets::Guild::GuildBankTextQuery& packet)
{
    TC_LOG_DEBUG("guild", "MSG_QUERY_GUILD_BANK_TEXT [{}]: TabId: {}", GetPlayerInfo(), packet.Tab);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankTabText(this, packet.Tab);
}

void WorldSession::HandleGuildBankSetTabText(WorldPackets::Guild::GuildBankSetTabText& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_SET_GUILD_BANK_TEXT [{}]: TabId: {}, Text: {}", GetPlayerInfo(), packet.Tab, packet.TabText);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SetBankTabText(packet.Tab, packet.TabText);
}

void WorldSession::HandleGuildSetGuildMaster(WorldPackets::Guild::GuildSetGuildMaster& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_LEADER [{}]: Target: {}", GetPlayerInfo(), packet.NewMasterName);

    if (normalizePlayerName(packet.NewMasterName))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetNewGuildMaster(this, packet.NewMasterName);
}
