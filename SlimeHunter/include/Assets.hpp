#pragma once
// Assets.hpp - Nơi DUY NHẤT tải và sở hữu mọi texture/font của game.
//
// Vì sao cần lớp này? Trong SFML 3, sf::Sprite KHÔNG có constructor mặc định: sprite phải được tạo
// cùng một texture, và texture đó phải sống lâu hơn sprite. Nếu mỗi nhân vật tự tạo texture cục bộ
// rồi bị hủy, sprite sẽ trỏ vào vùng nhớ đã chết (lỗi "hình trắng"). Gom tất cả vào Assets,
// được tạo MỘT lần trong Game, giải quyết triệt để vấn đề đó.
//
// Thiếu file thì sao? Mỗi hàm tải đều có phương án dự phòng (hình màu đơn sắc) và ghi lại một
// cảnh báo bằng tiếng Việt. Game vẫn chạy và hiển thị cảnh báo trên màn hình menu.

#include "Levels.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <filesystem>
#include <string>
#include <vector>

class Assets {
public:
    Assets();

    // Không cho sao chép: các Sprite giữ con trỏ tới texture nằm trong đối tượng này.
    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;

    sf::Font font;
    bool hasFont = false;

    // Nền và mặt nạ va chạm của từng level (trắng = vùng cấm, đen = đi được; mỗi pixel mặt nạ = 3px bản đồ)
    std::array<sf::Texture, kLevelCount> levelBackground;
    std::array<sf::Image, kLevelCount> levelCollision;
    const sf::Texture& getLevelBackground(int levelId) const;
    const sf::Image& getLevelCollision(int levelId) const;

    sf::Texture playerSheet;
    sf::Texture playerBat;
    sf::Texture shadow;

    sf::Texture slimeGreen;
    sf::Texture slimePurple;
    sf::Texture slimeBlue;
    sf::Texture slimeKing;
    sf::Texture skeletonSheet;
    sf::Texture batSheet;

    const std::vector<std::string>& getWarnings() const { return m_warnings; }
    const std::filesystem::path& getAssetDir() const { return m_assetDir; }

private:
    std::filesystem::path findAssetDirectory() const;
    bool loadTexture(sf::Texture& texture, const std::string& relativePath, sf::Vector2u fallbackSize,
                     sf::Color fallbackColor);
    void loadFont();
    void loadLevels();

    std::filesystem::path m_assetDir;
    std::vector<std::string> m_warnings;
};
