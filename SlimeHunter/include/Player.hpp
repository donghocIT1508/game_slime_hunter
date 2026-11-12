#pragma once
// Player.hpp - Nhân vật chính, kế thừa Entity.
//
// Áp dụng Strategy Pattern cho Vũ khí (Weapon),
// Kỹ năng chủ động Dash (Lướt né đòn với I-frames),
// Hệ thống Buffs & Roguelike Powerups.

#include "Animation.hpp"
#include "Balance.hpp"
#include "Entity.hpp"
#include "Projectile.hpp"
#include "Weapon.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Assets;
class GameMap;
class Effects;

struct PlayerInput {
    sf::Vector2f move{0.f, 0.f};
    bool run = false;
    bool attack = false;
    bool dash = false;
};

enum class PlayerState { Idle, Walk, Attack, Dead };

class Player : public Entity {
public:
    explicit Player(const Assets& assets);
    ~Player() override = default;

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    void reset(sf::Vector2f startPosition);
    void teleportTo(sf::Vector2f position);

    // ---- Chỉ số LÂU DÀI ----
    void setBaseStats(int maxHp, int damage);
    void applyLevelUpgrade(int newMaxHp, int newDamage);
    int getBaseMaxHp() const { return m_baseMaxHp; }
    int getBaseDamage() const { return m_baseDamage; }
    void heal(float amount);
    void setSlashTier(int tier);
    int getSlashTier() const { return m_slashTier; }
    const balance::SlashProfile& getSlashProfile() const { return m_slash; }

    // ---- Vòng đời cập nhật ----
    void update(float dt, const GameMap& map) override;
    void update(float dt, const PlayerInput& input, const GameMap& map);
    void updateWithCombat(float dt, const PlayerInput& input, const GameMap& map,
                          std::vector<Projectile>& projectiles, Effects& effects);

    void drawShadow(sf::RenderTarget& target) const override;
    void draw(sf::RenderTarget& target) const override;

    // ---- Vũ khí (Strategy Pattern) ----
    void setWeapon(std::unique_ptr<Weapon> weapon);
    Weapon* getWeapon() const { return m_weapon.get(); }

    // ---- Kỹ năng Dash (Lướt né đòn) ----
    bool tryDash();
    bool isDashing() const { return m_dashTimer > 0.f; }
    float getDashCooldownRatio() const;

    // ---- Chiến đấu ----
    bool hasPendingHit() const { return m_attackHitPending; }
    void confirmHit() { m_attackHitPending = false; }
    sf::FloatRect getAttackHitbox() const;
    sf::FloatRect getHitbox() const override;
    bool isAttacking() const { return m_state == PlayerState::Attack; }
    sf::Vector2f getAimDirection() const { return m_aim; }

    bool takeDamage(float amount, sf::Vector2f fromPosition) override;
    bool addExp(int amount);

    // ---- Buffs & Roguelike Power-ups ----
    void applySpeedBoost(float duration, float multiplier);
    void triggerMagnetPulse() { m_magnetPulseTimer = 1.0f; }
    bool isMagnetPulseActive() const { return m_magnetPulseTimer > 0.f; }
    void addCoins(int count) { m_coins += count; }
    int getCoins() const { return m_coins; }

    void addCritChance(float amount) { m_critChance = std::min(m_critChance + amount, 0.8f); }
    float getCritChance() const { return m_critChance; }
    void addAttackSpeedBonus(float amount) { m_attackSpeedBonus += amount; }
    float getAttackSpeedMultiplier() const { return 1.0f + m_attackSpeedBonus; }
    void setBurnAura(bool enable) { m_hasBurnAura = enable; }
    bool hasBurnAura() const { return m_hasBurnAura; }
    float getMagnetRadius() const { return m_isMagnetPulseActive() ? 5000.f : m_magnetRadius; }
    void setMagnetRadius(float r) { m_magnetRadius = r; }

    // ---- Truy vấn ----
    int getLevel() const { return m_level; }
    int getExp() const { return m_exp; }
    int getExpToNext() const;

private:
    void startAttack(std::vector<Projectile>& projectiles, Effects& effects);
    void recalculateStats();
    void chooseAnimation(bool running);
    void updateVisuals();
    bool m_isMagnetPulseActive() const { return m_magnetPulseTimer > 0.f; }

    sf::Sprite m_sprite;
    sf::Sprite m_bat;
    sf::Sprite m_shadow;
    Animation m_animation;

    PlayerState m_state = PlayerState::Idle;
    sf::Vector2f m_aim{1.f, 0.f};

    int m_baseMaxHp = balance::kBasePlayerMaxHp;
    int m_baseDamage = balance::kBasePlayerDamage;
    int m_slashTier = 1;
    balance::SlashProfile m_slash = balance::slashProfileForTier(1);

    int m_level = 1;
    int m_exp = 0;
    int m_coins = 0;

    // Strategy Pattern: Vũ khí hiện tại
    std::unique_ptr<Weapon> m_weapon;

    // Kỹ năng Dash
    float m_dashTimer = 0.f;
    float m_dashCooldown = 0.f;
    const float kDashDuration = 0.22f;
    const float kDashCooldownMax = 1.6f;
    sf::Vector2f m_dashDir{1.f, 0.f};

    // Buffs & Roguelite upgrades
    float m_speedBoostTimer = 0.f;
    float m_speedBoostMultiplier = 1.f;
    float m_magnetPulseTimer = 0.f;
    float m_magnetRadius = 120.f;
    float m_critChance = 0.05f;
    float m_attackSpeedBonus = 0.f;
    bool m_hasBurnAura = false;

    float m_attackCooldown = 0.f;
    float m_attackTimer = 0.f;
    bool m_attackHitTriggered = false;
    bool m_attackHitPending = false;
    float m_invincibleTimer = 0.f;
    float m_hurtTimer = 0.f;
};
