#pragma once
// Shop.hpp - Hệ thống Bảng điểm cao (Leaderboard) và Cửa hàng vĩnh viễn (Shop).
// Đọc dữ liệu từ UserRepository CSV và hỗ trợ nâng cấp khởi đầu cho nhân vật.

#include "UserRepository.hpp"

#include <string>
#include <vector>

struct LeaderboardEntry {
    std::string username;
    int score;
    int kills;
    int maxLevel;
};

struct ShopItem {
    int id;
    std::string name;
    std::string desc;
    int cost;
    bool purchased;
};

class ShopSystem {
public:
    static std::vector<LeaderboardEntry> getTopScores(UserRepository& repo, int limit = 5);
    static std::vector<ShopItem> getShopItems(const ProgressRecord& progress);
    static bool buyItem(int itemId, ProgressRecord& progress, UserRepository& repo);
};
