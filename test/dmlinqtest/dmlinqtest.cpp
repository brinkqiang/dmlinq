#include "dmlinq.hpp" // 引入我们要测试的库
#include "gtest.h"    // 引入 gtest

#include <string>
#include <vector>
#include <set>
#include <map>

// 定义测试用的数据结构
struct Player {
    std::string name;
    std::string team;
    int score = 0; // C26495 fix: Always initialize member variables.

    // 为 gtest 的 ASSERT_EQ 提供比较运算符
    bool operator==(const Player& other) const {
        return name == other.name && team == other.team && score == other.score;
    }
};

// 测试环境类
class env_dmlinq
{
public:
    void init() {}
    void uninit() {}
};

// 测试框架类，包含共享的测试数据
class frame_dmlinq : public testing::Test
{
public:
    virtual void SetUp() override
    {
        env.init();
        players = {
            {"Alice", "Eagles", 50}, {"David", "Bears", 90},
            {"Bob",   "Eagles", 80}, {"Eve",   "Bears", 80},
            {"Frank", "Bears", 75}, {"Carol", "Eagles", 50},
        };
        numbers = {5, 1, 4, 1, 3, -2};
        empty_numbers = {};
    }
    virtual void TearDown() override
    {
        env.uninit();
    }
protected:
    env_dmlinq env;
    std::vector<Player> players;
    std::vector<int> numbers;
    std::vector<int> empty_numbers;
};

// --- 开始测试 ---

TEST_F(frame_dmlinq, Filtering_Where)
{
    using namespace dmlinq;
    auto eagles_players = from(players)
        .where([](const Player& p) { return p.team == "Eagles"; })
        .toVector();

    ASSERT_EQ(eagles_players.size(), 3);
    EXPECT_TRUE(std::all_of(eagles_players.begin(), eagles_players.end(),
        [](const Player& p) { return p.team == "Eagles"; }));

    auto high_score_bears = from(players)
        .where([](const Player& p) { return p.team == "Bears"; })
        .where([](const Player& p) { return p.score > 80; })
        .toVector();

    ASSERT_EQ(high_score_bears.size(), 1);
    EXPECT_EQ(high_score_bears[0].name, "David");
}

TEST_F(frame_dmlinq, Sorting_OrderByThenBy)
{
    using namespace dmlinq;
    auto sorted_players = from(players)
        .orderBy([](const Player& p) { return p.team; })
        .thenByDescending([](const Player& p) { return p.score; })
        .toVector();

    ASSERT_EQ(sorted_players.size(), 6);
    // 验证 Bears 团队
    EXPECT_EQ(sorted_players[0].name, "David"); // Bears, 90
    EXPECT_EQ(sorted_players[1].name, "Eve");   // Bears, 80
    EXPECT_EQ(sorted_players[2].name, "Frank"); // Bears, 75
    // 验证 Eagles 团队
    EXPECT_EQ(sorted_players[3].name, "Bob");   // Eagles, 80
    EXPECT_EQ(sorted_players[4].name, "Alice"); // Eagles, 50
    EXPECT_EQ(sorted_players[5].name, "Carol"); // Eagles, 50
}

TEST_F(frame_dmlinq, Projection_SelectAndSelectMany)
{
    using namespace dmlinq;
    auto player_names = from(players)
        .select([](const Player& p) { return p.name; })
        .toVector();

    ASSERT_EQ(player_names.size(), 6);
    EXPECT_EQ(player_names[0], "Alice");
    EXPECT_EQ(player_names[5], "Carol");

    // For SelectMany
    struct Team { std::string name; std::vector<Player> members; };
    std::vector<Team> teams = {
        {"Eagles", {players[0], players[2], players[5]}},
        {"Bears", {players[1], players[3], players[4]}}
    };

    auto all_players_flat = from(teams)
        .selectMany([](const Team& t) { return t.members; })
        .toVector();
    ASSERT_EQ(all_players_flat.size(), 6);
    EXPECT_EQ(all_players_flat[0].name, "Alice");
    EXPECT_EQ(all_players_flat[5].name, "Frank");
}

TEST_F(frame_dmlinq, Partitioning_TakeAndSkip)
{
    using namespace dmlinq;
    auto skipped = from(numbers).skip(2).toVector();
    ASSERT_EQ(skipped.size(), 4);
    EXPECT_EQ(skipped[0], 4);

    auto taken = from(numbers).take(3).toVector();
    ASSERT_EQ(taken.size(), 3);
    EXPECT_EQ(taken[0], 5);
    EXPECT_EQ(taken[2], 4);

    auto skipped_and_taken = from(numbers).skip(2).take(2).toVector();
    ASSERT_EQ(skipped_and_taken.size(), 2);
    EXPECT_EQ(skipped_and_taken[0], 4);
    EXPECT_EQ(skipped_and_taken[1], 1);

    // Edge cases
    EXPECT_TRUE(from(numbers).skip(100).toVector().empty());
    EXPECT_EQ(from(numbers).take(100).toVector().size(), 6);
}

TEST_F(frame_dmlinq, Element_FirstLastSingle)
{
    using namespace dmlinq;
    // First
    EXPECT_EQ(from(players).first().name, "Alice");
    auto first_bear = from(players).first([](const Player& p) { return p.team == "Bears"; });
    EXPECT_EQ(first_bear.name, "David");
    EXPECT_THROW(from(empty_numbers).first(), std::runtime_error);

    // FirstOrDefault
    auto no_one = from(players).firstOrDefault([](const Player& p) { return p.score > 100; });
    ASSERT_FALSE(no_one.has_value());
    auto empty_first = from(empty_numbers).firstOrDefault();
    ASSERT_FALSE(empty_first.has_value());

    // Last
    EXPECT_EQ(from(players).last().name, "Carol");

    // Single
    auto single_david = from(players).single([](const Player& p) { return p.name == "David"; });
    EXPECT_EQ(single_david.score, 90);
    EXPECT_THROW(from(players).single([](const Player& p) { return p.team == "Eagles"; }), std::runtime_error); // More than one
    EXPECT_THROW(from(players).single([](const Player& p) { return p.score > 100; }), std::runtime_error); // None
}

TEST_F(frame_dmlinq, Aggregation_CountSumAverageMaxMin)
{
    using namespace dmlinq;
    // Count
    EXPECT_EQ(from(players).count(), 6);
    auto count = from(players).count([](const Player& p) { return p.score == 80; });

    EXPECT_EQ(count, 2);

    // Sum
    EXPECT_EQ(from(numbers).sum(), 12);
    auto total_score = from(players).sum([](const Player& p) { return p.score; });
    EXPECT_EQ(total_score, 425);

    // Average
    EXPECT_DOUBLE_EQ(from(numbers).average(), 2.0);
    double avg_score = from(players).average([](const Player& p) { return p.score; });
    EXPECT_NEAR(avg_score, 70.8333, 0.0001);

    // Max/Min
    EXPECT_EQ(from(numbers).max(), 5);
    EXPECT_EQ(from(numbers).min(), -2);
}

TEST_F(frame_dmlinq, Quantifiers_AnyAll)
{
    using namespace dmlinq;
    // Any
    EXPECT_TRUE(from(players).any());
    EXPECT_FALSE(from(empty_numbers).any());
    EXPECT_TRUE(from(players).any([](const Player& p) { return p.name == "Frank"; }));
    EXPECT_FALSE(from(players).any([](const Player& p) { return p.score < 0; }));

    // All
    EXPECT_TRUE(from(players).all([](const Player& p) { return p.score >= 50; }));
    EXPECT_FALSE(from(players).all([](const Player& p) { return p.team == "Eagles"; }));
    EXPECT_TRUE(from(empty_numbers).all([](const int& n) { return n > 0; })); // All on empty is true
}

TEST_F(frame_dmlinq, Conversion_ToSetToMap)
{
    using namespace dmlinq;
    // ToSet
    auto number_set = from(numbers).toSet();
    EXPECT_EQ(number_set.size(), 5); // 1 is duplicated

    // ToMap
    auto score_map = from(players).toMap(
        [](const Player& p) { return p.name; },
        [](const Player& p) { return p.score; }
    );
    ASSERT_EQ(score_map.size(), 6);
    EXPECT_EQ(score_map.at("David"), 90);
    EXPECT_EQ(score_map["Alice"], 50);
}

TEST_F(frame_dmlinq, Combined_ComplexQuery)
{
    using namespace dmlinq;
    // Get the names of all players from the team with the highest average score.

    // 1. Group players by team (not a direct LINQ op, so we do it manually)
    std::map<std::string, std::vector<Player>> teams;
    for (const auto& p : players) {
        teams[p.team].push_back(p);
    }

    // 2. Calculate average score for each team
    // C2679 FIX: Manually create a vector of non-const pairs from the map.
    std::vector<std::pair<std::string, std::vector<Player>>> teams_vec;
    for (const auto& pair : teams) {
        teams_vec.push_back(pair);
    }

    auto team_avg_scores = from(teams_vec)
        .select([](const std::pair<std::string, std::vector<Player>>& team_pair) {
        return std::make_pair(
            team_pair.first,
            from(team_pair.second).average([](const Player& p) { return p.score; })
        );
            })
        .toVector();

    // 3. Find the team with the max average score
    auto best_team_info = from(team_avg_scores)
        .orderByDescending([](const auto& pair) { return pair.second; })
        .first();

    ASSERT_EQ(best_team_info.first, "Bears");
    EXPECT_NEAR(best_team_info.second, 81.6667, 0.0001);

    // 4. Get the names of players from that team
    auto best_team_player_names = from(players)
        .where([&best_team_info](const Player& p) { return p.team == best_team_info.first; })
        .select([](const Player& p) { return p.name; })
        .toVector();

    ASSERT_EQ(best_team_player_names.size(), 3);
    // Convert to set for easy checking without regard to order
    std::set<std::string> name_set(best_team_player_names.begin(), best_team_player_names.end());
    EXPECT_TRUE(name_set.count("David"));
    EXPECT_TRUE(name_set.count("Eve"));
    EXPECT_TRUE(name_set.count("Frank"));
}