#pragma once
// Entity.hpp - Lớp cơ sở trừu tượng cho mọi thực thể sống trong thế giới game (Player, Monster).
//
// Thể hiện tính TRỪU TƯỢNG (Abstraction) và ĐÓNG GÓI (Encapsulation) trong OOP:
// Gom toàn bộ thuộc tính chung (máu, vị trí, sát thương, tốc độ, va chạm chân)
// và các phương thức cốt lõi (nhận sát thương, vẽ, kiểm tra còn sống) vào một nơi.

#include <SFML/Graphics.hpp>

class GameMap;

class Entity {
public:
    Entity(sf::Vector2f startPosition, float hp, float maxHp, float damage, float speed, float footRadius)
        : m_position(startPosition),
          m_hp(hp),
          m_maxHp(maxHp),
          m_damage(damage),
          m_speed(speed),
          m_footRadius(footRadius) {}

    virtual ~Entity() = default;

    // Không cho phép sao chép để tránh lỗi tài nguyên con trỏ / texture
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // ---- CÁC PHƯƠNG THỨC ẢO THUẦN TÚY (Pure Virtual) ----
    virtual void update(float dt, const GameMap& map) = 0;
    virtual void drawShadow(sf::RenderTarget& target) const = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual bool takeDamage(float amount, sf::Vector2f fromPosition) = 0;
    virtual sf::FloatRect getHitbox() const = 0;

    // ---- CÁC PHƯƠNG THỨC DÙNG CHUNG ----
    sf::Vector2f getPosition() const { return m_position; }
    void setPosition(sf::Vector2f pos) { m_position = pos; }

    float getHp() const { return m_hp; }
    float getMaxHp() const { return m_maxHp; }
    float getDamage() const { return m_damage; }
    float getSpeed() const { return m_speed; }
    float getFootRadius() const { return m_footRadius; }
    bool isFacingRight() const { return m_facingRight; }
    bool isDead() const { return m_hp <= 0.f; }

    void setHp(float hp) { m_hp = std::min(hp, m_maxHp); }
    void setMaxHp(float maxHp) { m_maxHp = maxHp; m_hp = std::min(m_hp, m_maxHp); }
    void setDamage(float dmg) { m_damage = dmg; }
    void setSpeed(float spd) { m_speed = spd; }

protected:
    sf::Vector2f m_position{0.f, 0.f};
    sf::Vector2f m_knockback{0.f, 0.f};
    float m_hp = 100.f;
    float m_maxHp = 100.f;
    float m_damage = 20.f;
    float m_speed = 120.f;
    float m_footRadius = 16.f;
    bool m_facingRight = true;
};
