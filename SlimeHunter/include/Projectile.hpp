#pragma once
// Projectile.hpp - Lớp quản lý đạn bay (Mũi tên của Skeleton, Cầu phép của Wand, Boomerang, Cầu độc).
// Dùng chung cho cả đòn đánh tầm xa của quái và vũ khí tầm xa của Player.

#include <SFML/Graphics.hpp>

class GameMap;

enum class ProjectileType {
    Arrow,       // Skeleton bắn
    MagicOrb,    // Player dùng Magic Wand bắn
    Boomerang,   // Player ném Boomerang
    PoisonBall   // Quái/Boss bắn
};

class Projectile {
public:
    Projectile(sf::Vector2f startPos, sf::Vector2f velocity, float damage, float lifetime,
               float radius, bool fromPlayer, ProjectileType type, sf::Color color = sf::Color::White);

    void update(float dt, const sf::Vector2f& playerPos, const GameMap& map);
    void draw(sf::RenderTarget& target) const;

    bool checkHit(const sf::FloatRect& targetBox) const;
    void destroy() { m_lifetime = 0.f; }

    bool isDead() const { return m_lifetime <= 0.f; }
    bool isFromPlayer() const { return m_fromPlayer; }
    float getDamage() const { return m_damage; }
    sf::Vector2f getPosition() const { return m_position; }
    float getRadius() const { return m_radius; }
    ProjectileType getType() const { return m_type; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_damage;
    float m_lifetime;
    float m_maxLifetime;
    float m_radius;
    bool m_fromPlayer;
    ProjectileType m_type;
    sf::Color m_color;
    float m_rotation = 0.f;
    bool m_isReturning = false;
};
