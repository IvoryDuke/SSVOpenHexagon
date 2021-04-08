// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#include "SSVOpenHexagon/Online/Database.hpp"

#include "SSVOpenHexagon/Global/Assert.hpp"
#include "SSVOpenHexagon/Utils/Concat.hpp"

#include <SSVUtils/Core/Log/Log.hpp>

static auto& dlog(const char* funcName)
{
    return ::ssvu::lo(::hg::Utils::concat("hg::Database::", funcName));
}

#define SSVOH_DLOG ::dlog(__func__)

#define SSVOH_DLOG_VERBOSE \
    if(_verbose) ::dlog(__func__)

#define SSVOH_DLOG_ERROR ::dlog(__func__) << "[ERROR] "

#define SSVOH_DLOG_VAR(x) '\'' << #x << "': '" << x << '\''

namespace hg::Database
{

void addUser(const User& user)
{
    const int id = Impl::getStorage().insert(user);

    SSVOH_DLOG << "Added user with id '" << id << "' to storage:\n"
               << Impl::getStorage().dump(user) << '\n';
}

void removeUser(const std::uint32_t id)
{
    Impl::getStorage().remove<User>(id);

    SSVOH_DLOG << "Removed user with id '" << id << "' from storage\n";
}

void dumpUsers()
{
    SSVOH_DLOG << "Dumping all users\n";

    const auto users = Impl::getStorage().get_all<User>();

    SSVOH_DLOG << "users (" << users.size() << "):\n";

    for(const auto& user : users)
    {
        SSVOH_DLOG << Impl::getStorage().dump(user) << '\n';
    }
}

[[nodiscard]] bool anyUserWithSteamId(const std::uint64_t steamId)
{
    return !getAllUsersWithSteamId(steamId).empty();
}

[[nodiscard]] bool anyUserWithName(const std::string& name)
{
    using namespace sqlite_orm;

    auto query =
        Impl::getStorage().get_all<User>(where(name == c(&User::name)));

    return !query.empty();
}

[[nodiscard]] std::optional<User> getUserWithSteamIdAndName(
    const std::uint64_t steamId, const std::string& name)
{
    using namespace sqlite_orm;

    auto query = Impl::getStorage().get_all<User>(
        where(steamId == c(&User::steamId) && name == c(&User::name)));

    if(query.empty())
    {
        return std::nullopt;
    }

    if(query.size() > 1)
    {
        SSVOH_DLOG_ERROR
            << "Database integrity error, multiple users with same steamId '"
            << steamId << "' and name '" << name << "'\n";

        return std::nullopt;
    }

    SSVOH_ASSERT(query.size() == 1);
    return {query[0]};
}

void removeAllLoginTokensForUser(const std::uint32_t userId)
{
    using namespace sqlite_orm;

    Impl::getStorage().remove_all<LoginToken>(
        where(userId == c(&LoginToken::userId)));
}

void addLoginToken(const LoginToken& loginToken)
{
    const int id = Impl::getStorage().insert(loginToken);

    SSVOH_DLOG << "Added login token with id '" << id << "' to storage:\n"
               << Impl::getStorage().dump(loginToken) << '\n';
}

[[nodiscard]] std::vector<User> getAllUsersWithSteamId(
    const std::uint64_t steamId)
{
    using namespace sqlite_orm;

    auto query =
        Impl::getStorage().get_all<User>(where(steamId == c(&User::steamId)));

    return query;
}

[[nodiscard]] std::optional<User> getUserWithSteamId(
    const std::uint64_t steamId)
{
    const auto query = getAllUsersWithSteamId(steamId);

    if(query.empty())
    {
        return std::nullopt;
    }

    if(query.size() > 1)
    {
        SSVOH_DLOG_ERROR
            << "Database integrity error, multiple users with same steamId '"
            << steamId << "'\n";

        return std::nullopt;
    }

    SSVOH_ASSERT(query.size() == 1);
    return {query[0]};
}

constexpr int tokenValiditySeconds = 3600;

[[nodiscard]] static bool isLoginTokenTimestampValid(const LoginToken& lt)
{
    const TimePoint now = Clock::now();

    return (now - toTimepoint(lt.timestamp)) <
           std::chrono::seconds(tokenValiditySeconds);
}

[[nodiscard]] std::vector<LoginToken> getAllStaleLoginTokens()
{
    using namespace sqlite_orm;

    auto query = Impl::getStorage().get_all<LoginToken>();

    query.erase(std::remove_if(query.begin(), query.end(),
                    [&](const LoginToken& lt) {
                        return isLoginTokenTimestampValid(lt);
                    }),
        std::end(query));

    return query;
}

void removeAllStaleLoginTokens()
{
    using namespace sqlite_orm;

    const auto staleTokens = getAllStaleLoginTokens();
    for(const LoginToken& lt : staleTokens)
    {
        Impl::getStorage().remove<LoginToken>(lt.id);
    }
}

[[nodiscard]] std::vector<ProcessedScore> getTopScores(
    const int topLimit, const std::string& levelValidator)
{
    using namespace sqlite_orm;

    auto query = Impl::getStorage().select(
        columns(&User::name, &Score::timestamp, &Score::value),
        join<Score>(on(c(&User::steamId) == &Score::userSteamId)),
        where(levelValidator == c(&Score::levelValidator)),
        order_by(&Score::value).desc(), limit(topLimit));

    std::vector<ProcessedScore> result;

    std::uint32_t index = 0;
    for(const auto& row : query)
    {
        result.push_back( //
            ProcessedScore{
                .position = index,                  //
                .userName = std::get<0>(row),       //
                .scoreTimestamp = std::get<1>(row), //
                .scoreValue = std::get<2>(row),     //
            });

        ++index;
    }

    return result;
}

[[nodiscard]] bool isLoginTokenValid(std::uint64_t token)
{
    using namespace sqlite_orm;

    const auto query = Impl::getStorage().get_all<LoginToken>(
        where(token == c(&LoginToken::token)));

    if(query.empty() || query.size() > 1)
    {
        return false;
    }

    return isLoginTokenTimestampValid(query.at(0));
}

void addScore(const std::string& levelValidator, const std::uint64_t timestamp,
    const std::uint64_t userSteamId, const double value)
{
    using namespace sqlite_orm;

    Score score{
        .levelValidator = levelValidator, //
        .timestamp = timestamp,           //
        .userSteamId = userSteamId,       //
        .value = value                    //
    };

    const auto query = Impl::getStorage().get_all<Score>(
        where(userSteamId == c(&Score::userSteamId) &&
              levelValidator == c(&Score::levelValidator)));

    if(query.empty())
    {
        const int id = Impl::getStorage().insert(score);

        SSVOH_DLOG << "Added score with id '" << id << "' to storage:\n"
                   << Impl::getStorage().dump(score) << '\n';

        return;
    }

    const Score& existingScore = query.at(0);
    if(existingScore.value >= value)
    {
        return;
    }

    score.id = existingScore.id;

    Impl::getStorage().update(score);

    SSVOH_DLOG << "Updated score with id '" << score.id << "' to storage:\n"
               << Impl::getStorage().dump(score) << '\n';
}

[[nodiscard]] std::optional<ProcessedScore> getScore(
    const std::string& levelValidator, const std::uint64_t userSteamId)
{
    using namespace sqlite_orm;

    const auto query =
        Impl::getStorage().select(columns(&User::name, &Score::timestamp,
                                      &Score::value, &Score::userSteamId),
            join<Score>(on(c(&User::steamId) == &Score::userSteamId)),
            where(levelValidator == c(&Score::levelValidator)),
            order_by(&Score::value).desc());

    if(query.empty())
    {
        return std::nullopt;
    }

    std::uint32_t index = 0;
    for(const auto& row : query)
    {
        if(std::get<3>(row) == userSteamId)
        {
            return {ProcessedScore{
                .position = index,                  //
                .userName = std::get<0>(row),       //
                .scoreTimestamp = std::get<1>(row), //
                .scoreValue = std::get<2>(row),     //
            }};
        }

        ++index;
    }

    return std::nullopt;
}

} // namespace hg::Database
