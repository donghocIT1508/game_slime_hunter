#pragma once
// Slime.hpp - Quái Slime: kế thừa Monster, thêm kiểu di chuyển "nảy từng bước".

#include "Monster.hpp"

class Assets;

enum class SlimeType {
    Green,   // cơ bản
    Purple,  // nhanh, ít máu
    Tank,    // slime xanh dương: nhiều máu, đòn đau, chậm hơn
    King     // boss: to, nhiều máu, đội vương miện
};

class Slime : public Monster {
public:
    // 'scale' là hệ số độ khó theo level (máu, sát thương, tốc độ, hồi chiêu); mặc định = level 1.
    Slime(const Assets& assets, SlimeType type, sf::Vector2f startPosition, const balance::MonsterScale& scale = balance::MonsterScale());

protected:
    // Ghi đè (override) hai hàm virtual của Monster:
    float movementFactor() const override;  // chỉ lao tới khi đang ở giữa cú nảy
    float visualLift() const override;      // nhấc sprite lên khỏi mặt đất khi nảy

private:
    static MonsterStats makeStats(SlimeType type, const balance::MonsterScale& scale);
    static MonsterVisual makeVisual(SlimeType type);
    static const sf::Texture& pickTexture(const Assets& assets, SlimeType type);
};
