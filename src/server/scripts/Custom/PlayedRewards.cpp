/*
    Written by Alistar@AC-WEB
    Discord: Alistar#2047
*/

#include "PlayedRewards.h"

PlayedRewards* PlayedRewards::instance()
{
    static PlayedRewards instance;
    return &instance;
}

void PlayedRewards::LoadConfig()
{
    m_IsEnabled   = sConfigMgr->GetBoolDefault("PlayedRewards.Enable", false);
    m_SiteData[0] = sConfigMgr->GetStringDefault("PlayedRewards.SiteDB", "");
    m_SiteData[1] = sConfigMgr->GetStringDefault("PlayedRewards.SiteTable", "");
    m_SiteData[2] = sConfigMgr->GetStringDefault("PlayedRewards.SiteColumnID", "");
    m_SiteData[3] = sConfigMgr->GetStringDefault("PlayedRewards.SiteColumnVP", "");
    m_SiteData[4] = sConfigMgr->GetStringDefault("PlayedRewards.SiteColumnDP", "");
}

void PlayedRewards::LoadFromDB()
{
    // H:M:S
    // 02:13:55 = 8035 seconds
    const auto timeToSec = [](const std::string_view timeStr) -> uint32
    {
        const auto tokenize = Trinity::Tokenize(timeStr, ':', true);
        const uint32 hour   = std::stoi(tokenize[0].data());
        const uint32 min    = std::stoi(tokenize[1].data());
        const uint32 sec    = std::stoi(tokenize[2].data());

        return (((hour * 60) + min) * 60) + sec;
    };

    uint32 count = 0;
    if (const QueryResult rewardList = CharacterDatabase.PQuery("SELECT * FROM played_rewards"))
    {
        do
        {
            const Field* fields          = rewardList->Fetch();
            const uint32 rewardId        = fields[0].GetUInt32();
            const std::string playedTime = fields[1].GetString();
            const uint32 gold            = fields[2].GetUInt32();
            const uint32 title           = fields[3].GetUInt32();
            const uint32 achievement     = fields[4].GetUInt32();
            const uint32 exp             = fields[5].GetUInt32();
            const uint32 level           = fields[6].GetUInt32();
            const std::string items      = fields[7].GetString();
            const uint32 honorPoints     = fields[8].GetUInt32();
            const uint32 arenaPoints     = fields[9].GetUInt32();
            const uint32 votePoints      = fields[10].GetUInt32();
            const uint32 donationPoints  = fields[11].GetUInt32();

            const std::array<std::pair<uint32, RewardType>, 9> rewardPair =
            {{
                {gold,           RewardType::GOLD},
                {title,          RewardType::TITLE},
                {achievement,    RewardType::ACHIEVEMENT},
                {exp,            RewardType::EXP},
                {level,          RewardType::LEVEL},
                {honorPoints,    RewardType::HONOR_POINTS},
                {arenaPoints,    RewardType::ARENA_POINTS},
                {votePoints,     RewardType::VOTE_POINTS},
                {donationPoints, RewardType::DONATION_POINTS}
            }};

            // insert to our reward map
            for (const auto& [reward, type] : rewardPair)
                if (reward)
                    m_RewardMap.insert({ rewardId, {playedTime, timeToSec(playedTime), type, reward}});

            // Process any items
            if (items != "0")
            {
                auto tokens = Trinity::Tokenize(items, ' ', true);
                if (tokens.size() % 2 == 0)
                {
                    std::unordered_map<uint32, uint32> items;
                    for (auto it = std::begin(tokens); it != std::end(tokens); ++it)
                    {
                        items.insert({ atoi(it->data()), atoi((it + 1)->data()) });
                        ++it;
                    }
                    m_RewardMap.insert({ rewardId, {playedTime, timeToSec(playedTime), RewardType::ITEM, 0, items} });
                }
                else
                    TC_LOG_ERROR("", "[PlayedRewards]: Incorrect items format for rewardId %u.", rewardId);
            }

            ++count;

        } while (rewardList->NextRow());
    }
    TC_LOG_INFO("server.loading", ">> Loaded %u played rewards", count);
}

bool PlayedRewards::IsEnabled() const
{
    return m_IsEnabled;
}

bool PlayedRewards::IsEligible(Player* player, const uint32 playTime, const uint32 rewardId)
{
    // Check if has passed reward played time
    if (player->GetTotalPlayedTime() < playTime)
        return false;

    // Check if has already claimed that reward
    const uint64 plrGUID = player->GetGUID().GetCounter();
    if (m_RewardedMap.find(plrGUID) != std::end(m_RewardedMap))
    {
        auto it = std::find(std::begin(m_RewardedMap[plrGUID]), std::end(m_RewardedMap[plrGUID]), rewardId);
        if (it != std::end(m_RewardedMap[plrGUID]))
            return false;
    }

    return true;
}

void PlayedRewards::AnnounceReward(Player* player, const std::string_view str, const std::string_view playedTime)
{
    std::string msg = "[|cffff0000Награда игроку|r]: За достижение! ";
    msg += playedTime.data();
    msg += " за сыгранное время, вы были награждены: ";
    msg += str;
    msg += ".";
    ChatHandler(player->GetSession()).PSendSysMessage(msg.c_str());
}

void PlayedRewards::ConcatRewardMsg(Player* player, const RewardData& data,
    std::string& message,
    const ItemTemplate* item,
    const CharTitlesEntry* const title,
    const AchievementEntry* const achiv)
{
    switch (data.type)
    {
        case RewardType::ITEM:
        {
            message += "x [" + item->Name1 + "], ";
            break;
        }
        case RewardType::TITLE:
        {
            message += "1x ";
            message += RewardTypeToStr.at(data.type).data();
            message += " [";
            std::string titleName = player->GetGender() == GENDER_FEMALE ?
                title->Name1[SERVER_LOCALE] : title->Name[SERVER_LOCALE];
            message += titleName.replace(titleName.find("%s"), 2, player->GetName());
            message += "], ";
            break;
        }
        case RewardType::ACHIEVEMENT:
        {
            message += "1x ";
            message += RewardTypeToStr.at(data.type).data();
            message += " [";
            message += achiv->Title[SERVER_LOCALE];
            message += "], ";
            break;
        }
        default:
        {
            message += std::to_string(data.amount) + "x " + RewardTypeToStr.at(data.type).data() + ", ";
            break;
        }
    }
}

void PlayedRewards::SendReward(Player* player)
{
    std::unordered_set<uint32> claimedRewards{};
    std::string message{};
    std::string_view playedTimeStr{};

    for (const auto& [id, data] : m_RewardMap)
    {
        // Check if the player is eligible for reward
        if (!IsEligible(player, data.playedTime, id))
            continue;

        switch (data.type)
        {
        case RewardType::GOLD:
            player->ModifyMoney(data.amount * GOLD);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::ITEM:
            for (const auto& [id, count] : data.items)
            {
                if (auto item = sObjectMgr->GetItemTemplate(id))
                {
                    player->AddItem(id, count);
                    message += std::to_string(count);
                    ConcatRewardMsg(player, data, message, item);
                }
            }
            break;
        case RewardType::TITLE:
            if (const auto title = sCharTitlesStore.LookupEntry(data.amount))
            {
                if (!player->HasTitle(title))
                {
                    player->SetTitle(title);
                    ConcatRewardMsg(player, data, message, nullptr, title);
                }
            }
            break;
        case RewardType::ACHIEVEMENT:
            if (const auto achiv = sAchievementStore.LookupEntry(data.amount))
            {
                if (!player->HasAchieved(achiv->ID))
                {
                    player->CompletedAchievement(achiv);
                    ConcatRewardMsg(player, data, message, nullptr, nullptr, achiv);
                }
            }
            break;
        case RewardType::EXP:
            player->GiveXP(data.amount, nullptr);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::LEVEL:
            player->GiveLevel(player->GetLevel() + data.amount);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::HONOR_POINTS:
            player->ModifyHonorPoints(data.amount);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::ARENA_POINTS:
            player->ModifyArenaPoints(data.amount);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::VOTE_POINTS:
            SendWebReward(player, data.type, data.amount);
            ConcatRewardMsg(player, data, message);
            break;
        case RewardType::DONATION_POINTS:
            SendWebReward(player, data.type, data.amount);
            ConcatRewardMsg(player, data, message);
            break;
        }

        playedTimeStr = data.playedTimeStr;
        claimedRewards.insert(id);
    }

    if (claimedRewards.empty())
        return;

    // We make sure to save the reward id
    for (const auto& i : claimedRewards)
        m_RewardedMap[player->GetGUID()].insert(i);

    // eat the last two characters and announce to player
    AnnounceReward(player, message.substr(0, message.size() - 2), playedTimeStr);
}

void PlayedRewards::SendWebReward(Player* player, const RewardType type, const uint32 amount)
{
    std::string rewardCol = type == RewardType::VOTE_POINTS ? m_SiteData[3].data() : m_SiteData[4].data();
    // It would be better if we had a site worker but for now we can use loginDatabase
    LoginDatabase.PQuery("UPDATE `%s`.`%s` SET `%s` = `%s` + '%u' WHERE `%s` = '%u'",
        m_SiteData[0], m_SiteData[1], rewardCol, rewardCol, amount, m_SiteData[2], player->GetSession()->GetAccountId());
}

void PlayedRewards::LoadRewardedMap(Player* player)
{
    const uint64 plrGUID = player->GetGUID().GetCounter();
    if (const QueryResult rewarded = CharacterDatabase.PQuery("SELECT `claimedRewards` FROM `played_rewarded` WHERE `guid` = '%u'", plrGUID))
    {
        const Field* fields = rewarded->Fetch();
        const std::string claimedRewards = fields[0].GetString();

        if (claimedRewards.empty())
            return;

        // convert std::string_view to int and push to rewardedMap
        for (const auto& token : Trinity::Tokenize(claimedRewards, ' ', true))
            m_RewardedMap[plrGUID].insert(atoi(token.data()));
    }
}

void PlayedRewards::SaveRewardedMap(Player* player)
{
    const uint64 plrGUID = player->GetGUID().GetCounter();
    if (m_RewardedMap.find(plrGUID) != std::end(m_RewardedMap))
    {
        std::string rewards{};
        for (const auto& claimedRewards : m_RewardedMap[plrGUID])
        {
            rewards += std::to_string(claimedRewards);
            rewards += ' ';
        }

        CharacterDatabase.PQuery("REPLACE INTO `played_rewarded` (`guid`, `claimedRewards`) VALUES ('%u', '%s');", plrGUID, rewards);
    }
}

