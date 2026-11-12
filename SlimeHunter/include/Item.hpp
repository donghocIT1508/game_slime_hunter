#pragma once
// Item.hpp - Hệ thống vật phẩm rơi (Loot Drop / Pickups).
// Thể hiện tính Đa hình và Kế thừa trong OOP:
// Lớp cơ sở trừu tượng Item với hàm thuần ảo apply(Player&),
// và các lớp con cụ thể: HealthPotion, SpeedBooster, MagnetItem, BombPickup, ExpGem, CoinItem.

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class Player;

enum class ItemType {
    HealthPotion,
    SpeedBooster,
    Magnet,
    Bomb,
    ExpGem,
    Coin
};

class Item {
public:
    Item(sf::Vector2f pos, ItemType type);
    virtual ~Item() = default;

    Item(const Item&) = delete;
    Item& operator=(const Item&) = delete;

    virtual void apply(Player& player) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

    void update(float dt, const sf::Vector2f& playerPos, float magnetRadius);
    bool checkCollect(const sf::Vector2f& playerPos, float playerRadius);
    void pullTowards(sf::Vector2f targetPos);

    bool isCollected() const { return m_collected; }
    void markCollected() { m_collected = true; }
    sf::Vector2f getPosition() const { return m_position; }
    ItemType getType() const { return m_type; }

protected:
    sf::Vector2f m_position;
    ItemType m_type;
    float m_bobTimer = 0.f;
    bool m_collected = false;
    bool m_beingPulled = false;
    float m_pullSpeed = 0.f;
};

// ---- CÁC LỚP CON ĐA HÌNH ----

class HealthPotion : public Item {
public:
    explicit HealthPotion(sf::Vector2f pos, float healAmount = 35.f);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
private:
    float m_healAmount;
};

class SpeedBooster : public Item {
public:
    explicit SpeedBooster(sf::Vector2f pos, float duration = 5.f, float speedBoost = 1.35f);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
private:
    float m_duration;
    float m_speedBoost;
};

class MagnetItem : public Item {
public:
    explicit MagnetItem(sf::Vector2f pos);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
};

class BombPickup : public Item {
public:
    explicit BombPickup(sf::Vector2f pos, float damage = 120.f);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
    float getDamage() const { return m_damage; }
private:
    float m_damage;
};

class ExpGem : public Item {
public:
    explicit ExpGem(sf::Vector2f pos, int expAmount = 15);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
private:
    int m_expAmount;
};

class CoinItem : public Item {
public:
    explicit CoinItem(sf::Vector2f pos, int value = 10);
    void apply(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
    int getValue() const { return m_value; }
private:
    int m_value;
};

// ---- ITEM FACTORY (Design Pattern) ----
class ItemFactory {
public:
    static std::unique_ptr<Item> createRandomDrop(sf::Vector2f pos, float dropChance = 0.40f);
};
