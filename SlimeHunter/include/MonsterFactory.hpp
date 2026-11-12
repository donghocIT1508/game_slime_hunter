#pragma once
// MonsterFactory.hpp - Factory Pattern để sinh các loại quái vật.
// Tách biệt hoàn toàn việc khởi tạo quái ra khỏi Game controller.
// Thể hiện Factory Method Pattern trong OOP.

#include "Balance.hpp"
#include "Monster.hpp"

#include <memory>

class Assets;

enum class MonsterType {
    SlimeGreen,
    SlimePurple,
    SlimeTank,
    SlimeKing,
    Skeleton,
    Bat
};

class MonsterFactory {
public:
    static std::unique_ptr<Monster> createMonster(MonsterType type, sf::Vector2f pos,
                                                 const balance::MonsterScale& scale,
                                                 const Assets& assets);
};
