#pragma once
// PowerUp.hpp - Cơ chế nâng cấp Rogue-lite chọn 1 trong 3 thẻ bài.
// Mang lại giá trị chơi lại (Replayability) cực cao cho thể loại Survival Roguelike.

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <string>
#include <vector>

class Player;

enum class PowerUpType {
    AttackSpeed,     // Tăng tốc độ đánh
    CriticalStrike,  // Tăng tỉ lệ chí mạng
    BurnAura,        // Quái lại gần tự bị thiêu đốt
    SwiftBoots,      // Tăng tốc độ chạy vĩnh viễn
    SuperMagnet,     // Mở rộng bán kính hút ngọc / vàng
    DragonBlood,     // Tăng máu tối đa và hồi đầy máu
    WeaponWand,      // Mở khóa / trang bị Gậy Phép Thuật
    WeaponBoomerang  // Mở khóa / trang bị Boomerang
};

struct PowerUpCard {
    PowerUpType type;
    std::string title;
    std::string description;
    std::string icon;
};

// Cấu trúc bố cục thẻ bài Rogue-lite: dùng chung giữa render và xử lý click chuột
// Đảm bảo ở mọi độ phân giải (cửa sổ thu nhỏ, Fullscreen 1080p, 2K, 4K) thì hitbox và hình vẽ luôn khớp nhau 100%
struct PowerUpCardLayout {
    float cardW = 340.f;
    float cardH = 260.f;
    float spacing = 35.f;
    float startX = 0.f;
    float cardY = 0.f;

    [[nodiscard]] sf::FloatRect getBounds(int index) const {
        return sf::FloatRect({startX + static_cast<float>(index) * (cardW + spacing), cardY}, {cardW, cardH});
    }

    [[nodiscard]] bool contains(int index, sf::Vector2f pt) const {
        return getBounds(index).contains(pt);
    }
};

inline PowerUpCardLayout getPowerUpCardLayout(const sf::Vector2f& viewSize) {
    PowerUpCardLayout layout;
    const float totalW = 3.f * layout.cardW + 2.f * layout.spacing;
    layout.startX = (viewSize.x - totalW) * 0.5f;
    // Căn giữa theo chiều dọc hài hòa trên mọi độ phân giải
    layout.cardY = std::max(220.f, (viewSize.y - layout.cardH) * 0.46f);
    return layout;
}

class PowerUpManager {
public:
    static std::vector<PowerUpCard> getRandomCards(int count = 3);
    static void applyCard(Player& player, const PowerUpCard& card);
};
