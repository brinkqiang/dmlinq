#include "dmlinq.hpp"
#include <iostream>
#include <string>
#include <vector>

// Define a sample data structure
struct Player {
    std::string name;
    std::string team;
    int score;
};

// Overload the << operator for the Player struct to allow easy printing.
std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "Player<Name: " << p.name << ", Team: " << p.team << ", Score: " << p.score << ">";
    return os;
}

// A helper function to print the title and contents of any vector.
template <typename T>
void print_query_result(const std::string& title, const std::vector<T>& data) {
    std::cout << "--- " << title << " ---\n";
    if (data.empty()) {
        std::cout << "(No results)\n";
    }
    else {
        for (const auto& item : data) {
            std::cout << item << "\n";
        }
    }
    std::cout << "\n";
}

int main() {
    using namespace dmlinq;

    std::vector<Player> players = {
        {"Alice", "Eagles", 50}, {"David", "Bears", 90},
        {"Bob",   "Eagles", 80}, {"Eve",   "Bears", 80},
        {"Frank", "Bears", 75}, {"Carol", "Eagles", 50},
    };

    std::cout << "************ DM-LINQ DEMO (C++17 Corrected) ************\n\n";
    print_query_result("Original Data", players);

    // 1. Filtering & Element: Find the first player from "Eagles" with score > 60
    std::cout << "--- 1. Find first Eagles player with score > 60 ---\n";
    try {
        auto top_eagle = from(players)
            .first([](const Player& p) { return p.team == "Eagles" && p.score > 60; });
        std::cout << top_eagle << "\n\n";
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n\n";
    }

    // 2. Sorting: Get all players, sort by team, then by score descending
    auto sorted_players = from(players)
        .orderBy([](const Player& p) { return p.team; })
        .thenByDescending([](const Player& p) { return p.score; })
        .toVector();
    print_query_result("2. Sorted by Team, then by Score DESC", sorted_players);

    // 3. Projection & Partitioning: Get the names of the top 2 scorers
    auto top_2_scorers_names = from(players)
        .orderByDescending([](const Player& p) { return p.score; })
        .take(2)
        .select([](const Player& p) { return p.name + " (" + std::to_string(p.score) + ")"; })
        .toVector();
    print_query_result("3. Names of Top 2 Scorers", top_2_scorers_names);

    // 4. Aggregation: Count players on the "Bears" team and calculate their average score
    auto bears_query = from(players).where([](const Player& p) { return p.team == "Bears"; });
    size_t bears_count = bears_query.count();
    double avg_score = bears_query.average([](const Player& p) { return p.score; });
    std::cout << "--- 4. Bears Team Stats ---\n";
    std::cout << "Count: " << bears_count << ", Average Score: " << avg_score << "\n\n";

    // 5. Quantifiers: Check if any player scored a perfect 100, or if all players scored at least 50.
    bool any_perfect_score = from(players).any([](const Player& p) { return p.score == 100; });
    bool all_scored_50_plus = from(players).all([](const Player& p) { return p.score >= 50; });
    std::cout << "--- 5. Quantifier Checks ---\n";
    std::cout << "Any player with a perfect score? " << (any_perfect_score ? "Yes" : "No") << "\n";
    std::cout << "All players scored 50+? " << (all_scored_50_plus ? "Yes" : "No") << "\n\n";

    // 6. Conversion & C++17 Structured Bindings
    auto player_score_map = from(players).toMap(
        [](const Player& p) { return p.name; },
        [](const Player& p) { return p.score; }
    );
    std::cout << "--- 6. Player Score Map ---\n";
    for (const auto& [name, score] : player_score_map) { // C++17 Structured Binding
        std::cout << name << ": " << score << "\n";
    }
    std::cout << "\n";

    // 7. SelectMany: Flatten a list of teams and their players
    auto teams_and_players = from(players)
        .selectMany([](const Player& p) {
        return std::vector<std::string>{p.team, p.name};
            })
        .toVector();
    print_query_result("7. SelectMany Demo (Team, Player, Team, Player...)", teams_and_players);

    return 0;
}