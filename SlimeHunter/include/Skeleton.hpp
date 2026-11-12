#pragma once
// Skeleton.hpp - Quái tầm xa: Kế thừa Monster, bắn cung / ném xương về phía Player.
// Ghi đè tính đa hình: isRanged() = true, performRangedAttack().

#include "Monster.hpp"

class Assets;

class Skeleton : public Monster {
public:
    Skeleton(const Assets& assets, sf::Vector2f startPosition, const balance::MonsterScale& scale = balance::MonsterScale());

    bool isRanged() const override { return true; }
    void performRangedAttack(std::vector<Projectile>& projectiles, const sf::Vector2f& playerPos) override;

protected:
    float movementFactor() const override { return 1.f; }
    float visualLift() const override { return 0.f; }

private:
    static MonsterStats makeStats(const balance::MonsterScale& scale);
    static MonsterVisual makeVisual();
    static const sf::Texture& pickTexture(const Assets& assets);
};
