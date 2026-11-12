#pragma once
// Bat.hpp - Dơi quỷ: Kế thừa Monster, có khả năng bay lượn (ignoresObstacles() = true)
// vượt qua toàn bộ tường và hồ nước, tốc độ cao nhưng máu thấp.

#include "Monster.hpp"

class Assets;

class Bat : public Monster {
public:
    Bat(const Assets& assets, sf::Vector2f startPosition, const balance::MonsterScale& scale = balance::MonsterScale());

    bool ignoresObstacles() const override { return true; }

protected:
    float movementFactor() const override { return 1.f; }
    float visualLift() const override;

private:
    static MonsterStats makeStats(const balance::MonsterScale& scale);
    static MonsterVisual makeVisual();
    static const sf::Texture& pickTexture(const Assets& assets);

    mutable float m_wingTimer = 0.f;
};
