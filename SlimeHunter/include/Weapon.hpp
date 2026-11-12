#pragma once
// Weapon.hpp - Strategy Pattern cho hệ thống vũ khí.
// Cho phép Player thay đổi chiến lược tấn công linh hoạt (Cận chiến gậy, Cầu phép tầm xa, Boomerang).
// Thể hiện nguyên lý Open-Closed Principle (OCP) và Strategy Pattern trong OOP.

#include "Projectile.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class Player;
class Effects;

enum class WeaponType {
    BaseballBat,
    MagicWand,
    Boomerang
};

class Weapon {
public:
    virtual ~Weapon() = default;

    virtual void attack(Player& player, const sf::Vector2f& aimDir,
                        std::vector<Projectile>& projectiles, Effects& effects) = 0;
    virtual sf::FloatRect getHitbox(const Player& player) const = 0;
    virtual float getCooldown() const = 0;
    virtual float getDamageMultiplier() const = 0;
    virtual WeaponType getType() const = 0;
    virtual std::string getName() const = 0;
};

// ---- CÁC CHIẾN LƯỢC VŨ KHÍ CỤ THỂ ----

class BaseballBat : public Weapon {
public:
    BaseballBat();
    void attack(Player& player, const sf::Vector2f& aimDir,
                std::vector<Projectile>& projectiles, Effects& effects) override;
    sf::FloatRect getHitbox(const Player& player) const override;
    float getCooldown() const override { return 0.38f; }
    float getDamageMultiplier() const override { return 1.0f; }
    WeaponType getType() const override { return WeaponType::BaseballBat; }
    std::string getName() const override { return "Gậy Bóng Chày"; }
};

class MagicWand : public Weapon {
public:
    MagicWand();
    void attack(Player& player, const sf::Vector2f& aimDir,
                std::vector<Projectile>& projectiles, Effects& effects) override;
    sf::FloatRect getHitbox(const Player& player) const override;
    float getCooldown() const override { return 0.50f; }
    float getDamageMultiplier() const override { return 0.95f; }
    WeaponType getType() const override { return WeaponType::MagicWand; }
    std::string getName() const override { return "Gậy Phép Thuật"; }
};

class BoomerangWeapon : public Weapon {
public:
    BoomerangWeapon();
    void attack(Player& player, const sf::Vector2f& aimDir,
                std::vector<Projectile>& projectiles, Effects& effects) override;
    sf::FloatRect getHitbox(const Player& player) const override;
    float getCooldown() const override { return 0.70f; }
    float getDamageMultiplier() const override { return 1.30f; }
    WeaponType getType() const override { return WeaponType::Boomerang; }
    std::string getName() const override { return "Boomerang Săn Quái"; }
};
