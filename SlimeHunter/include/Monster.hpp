#pragma once
// Monster.hpp - Lớp cơ sở trừu tượng cho mọi loại quái vật trong game.
// Kế thừa từ Entity.
//
// Monster::update() là "bộ não" chung: máy trạng thái Idle -> Chase -> Attack -> Dead.
// Các loại quái khác nhau (Slime, Skeleton, Bat...) kế thừa Monster và ghi đè những
// phần khác biệt qua các hàm virtual (movementFactor, visualLift, ignoresObstacles, isRanged).
// Đây là nơi thể hiện trọn vẹn tính ĐA HÌNH (Polymorphism) và KẾ THỪA (Inheritance).

#include "Animation.hpp"
#include "Balance.hpp"
#include "Entity.hpp"
#include "Projectile.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

class Player;
class GameMap;
class PathField;

enum class MonsterState { Idle, Chase, Attack, Dead };

// Dữ liệu chỉ số của quái - đặt trong struct để dễ chỉnh cân bằng game.
struct MonsterStats {
    float maxHp = 40.f;
    float damage = 8.f;
    float speed = 120.f;           // pixel / giây
    float detectRange = 900.f;     // thấy Player trong khoảng này thì đuổi theo
    float attackRange = 55.f;      // gần Player trong khoảng này thì tấn công
    float attackCooldown = 1.3f;   // giây giữa hai đòn
    float attackDuration = 0.48f;  // thời gian cả đòn
    float attackHitTime = 0.26f;   // thời điểm gây sát thương
    float footRadius = 16.f;       // bán kính va chạm với cây/đá/quái khác
    sf::Vector2f bodySize{56.f, 50.f};  // hộp thân: Player đánh trúng khi chạm hộp này
    float knockbackSpeed = 380.f;  // vận tốc bị đẩy lùi khi trúng đòn
    float stunTime = 0.25f;        // choáng ngắn sau khi trúng đòn
    int scoreValue = 10;
    int expValue = 12;
};

// Dữ liệu hiển thị (kích thước frame, điểm chân, các clip animation...)
struct MonsterVisual {
    sf::Vector2i frameSize{48, 40};
    sf::Vector2f origin{24.f, 38.f};  // điểm chân trong frame
    float scale = 3.f;
    float shadowScale = 3.f;
    float hpBarHeight = 90.f;         // thanh máu nằm cao hơn chân bao nhiêu pixel
    AnimationClip idle;
    AnimationClip move;
    AnimationClip attack;
    AnimationClip hurt;
    AnimationClip death;
};

class Monster : public Entity {
public:
    Monster(const sf::Texture& sheet, const sf::Texture& shadow, const MonsterStats& stats,
            const MonsterVisual& visual, sf::Vector2f startPosition);
    ~Monster() override = default;

    Monster(const Monster&) = delete;
    Monster& operator=(const Monster&) = delete;

    void update(float dt, const GameMap& map) override;
    void update(float dt, const Player& player, const GameMap& map, const PathField& pathToPlayer);
    void updateWithPlayer(float dt, const Player& player, const GameMap& map,
                          const PathField& pathToPlayer, std::vector<Projectile>& projectiles);

    void drawShadow(sf::RenderTarget& target) const override;
    void draw(sf::RenderTarget& target) const override;

    // Trả về true nếu đòn này làm quái chết.
    bool takeDamage(float amount, sf::Vector2f fromPosition) override;
    // Trả về true ĐÚNG MỘT LẦN mỗi đòn, vào thời điểm quái gây sát thương. Game kiểm tra tầm rồi trừ máu Player.
    bool consumeAttackHit();

    // ---- Hiệu ứng trạng thái do đòn chém đặc biệt của Player gây ra ----
    void applySlow(float duration, float speedMultiplier);
    void applyDot(float duration, float tickInterval, float damagePerTick);
    int consumeDotDamage();      // tổng sát thương DoT từ lần gọi trước (để hiện số bay), rồi đặt về 0
    bool consumeDeathReward();   // true ĐÚNG MỘT LẦN sau khi quái chết
    bool isSlowed() const { return m_slowTimer > 0.f; }
    bool isBurning() const { return m_dotTimer > 0.f; }
    float getSlowFactor() const { return m_slowTimer > 0.f ? m_slowMultiplier : 1.f; }
    float getSlowTimer() const { return m_slowTimer; }
    float getDotTimer() const { return m_dotTimer; }
    sf::Vector2f getBodyCenter() const { return {m_position.x, m_position.y - m_stats.bodySize.y * 0.5f}; }

    // Đẩy quái ra khỏi chỗ chồng lấn với quái khác
    void nudge(sf::Vector2f delta, const GameMap& map);

    bool isDead() const { return m_state == MonsterState::Dead || m_hp <= 0.f; }
    bool shouldRemove() const;  // đã chết và hiệu ứng tan biến đã xong
    sf::FloatRect getHitbox() const override;
    float getAttackRange() const { return m_stats.attackRange; }
    int getScoreValue() const { return m_stats.scoreValue; }
    int getExpValue() const { return m_stats.expValue; }
    MonsterState getState() const { return m_state; }
    bool isBoss() const { return m_isBoss; }

    // ---- Các điểm mở rộng đa hình (Virtual) cho các chủng loài quái khác nhau ----
    virtual float movementFactor() const { return 1.f; }
    virtual float visualLift() const { return 0.f; }
    virtual bool ignoresObstacles() const { return false; }  // Bat bay qua tường
    virtual bool isRanged() const { return false; }          // Skeleton tấn công tầm xa
    virtual void performRangedAttack(std::vector<Projectile>& projectiles, const sf::Vector2f& playerPos);

protected:
    int getAnimationRow() const { return m_animation.getCurrentRow(); }
    int getAnimationFrame() const { return m_animation.getFrameIndex(); }
    const MonsterVisual& getVisual() const { return m_visual; }
    const MonsterStats& getStats() const { return m_stats; }
    void setBoss(bool boss) { m_isBoss = boss; }

    sf::Sprite m_sprite;
    sf::Sprite m_shadow;
    Animation m_animation;
    MonsterStats m_stats;
    MonsterVisual m_visual;

    MonsterState m_state = MonsterState::Idle;
    bool m_isBoss = false;

    float m_attackCooldown = 0.f;
    float m_attackTimer = 0.f;
    bool m_attackHitTriggered = false;
    bool m_attackHitPending = false;
    float m_stunTimer = 0.f;
    float m_flashTimer = 0.f;
    float m_deathTimer = 0.f;

    // Hiệu ứng trạng thái
    float m_slowTimer = 0.f;
    float m_slowMultiplier = 1.f;
    float m_dotTimer = 0.f;
    float m_dotTickTimer = 0.f;
    float m_dotTickInterval = 0.5f;
    float m_dotDamage = 0.f;
    int m_dotDamageAccum = 0;
    bool m_deathRewardPending = false;

    // Chống kẹt
    float m_stuckTimer = 0.f;
    float m_sidestepTimer = 0.f;
    sf::Vector2f m_sidestepDir{0.f, 0.f};

    float m_wanderTimer = 0.f;
    sf::Vector2f m_wanderDir{0.f, 0.f};

private:
    void startAttack();
    sf::Vector2f chooseChaseDirection(const Player& player, const GameMap& map, const PathField& pathToPlayer) const;
    void moveToward(sf::Vector2f direction, float speedScale, float dt, const GameMap& map);
    void wander(float dt, const GameMap& map);
    void applyKnockback(float dt, const GameMap& map);
    void updateVisuals();
    void updateStatusEffects(float dt);
    void die();
};
