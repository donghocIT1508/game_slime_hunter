#pragma once
// Levels.hpp - Dữ liệu cấu hình các màn chơi. Mỗi màn là MỘT phần tử trong bảng ở Levels.cpp;
// logic (Game, GameMap, ExitGate...) dùng chung cho mọi màn, không viết riêng từng màn.

#include <SFML/Graphics.hpp>

constexpr int kLevelCount = 5;

enum class AmbientType { None, Fireflies, Embers, Dust };  // hạt trang trí môi trường

struct LevelConfig {
    int id = 1;
    const char* name = "";
    const char* background = "";   // đường dẫn tương đối trong assets/
    const char* collision = "";    // mặt nạ va chạm (trắng = cấm), 1 pixel = 3px bản đồ
    sf::Vector2f spawn;            // nơi Player xuất hiện
    sf::Vector2f exit;             // tâm cổng thoát
    float exitRadius = 40.f;       // vùng kích hoạt cổng
    sf::FloatRect spawnZone;       // vùng quái được phép xuất hiện (kích thước 0 = toàn bản đồ)

    // --- Đợt quái: sinh đủ 'totalMonsters' con (không quá 'maxAlive' con cùng lúc), diệt hết thì mở cổng ---
    int totalMonsters = 12;
    int maxAlive = 6;
    float spawnInterval = 1.6f;    // giây giữa hai lần sinh
    float purpleChance = 0.f;      // tỉ lệ Slime tím (nhanh)
    float tankChance = 0.f;        // tỉ lệ Slime xanh dương (trâu bò)
    bool bossAtEnd = false;        // sinh thêm Slime King sau khi sinh đủ quái thường

    AmbientType ambient = AmbientType::None;
    sf::Color ambientColor = sf::Color::White;
    sf::Color clearColor = sf::Color(20, 28, 24);  // màu nền phía ngoài bản đồ
};

// Trả về cấu hình của màn 'levelId' (được kẹp trong 1..kLevelCount).
const LevelConfig& getLevelConfig(int levelId);
