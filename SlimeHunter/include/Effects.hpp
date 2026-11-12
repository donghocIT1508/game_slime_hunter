#pragma once
// Effects.hpp - Hiệu ứng hình ảnh ngắn hạn: vệt chém, tia lửa khi trúng đòn, số sát thương bay, hạt môi trường.
//
// Mọi hiệu ứng đều có "tuổi thọ": update() tăng tuổi theo dt, hết tuổi thì bị XÓA khỏi vector.
// Mỗi loại có giới hạn số lượng tối đa (kMax...) nên dù đánh liên tục cũng không tạo ra hàng nghìn đối tượng.

#include "Levels.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

class Effects {
public:
    void clear();  // xóa hết (khi đổi level / chơi lại)
    void update(float dt);

    // dir: hướng chém (vector đơn vị). Vệt cung quét qua 'spanDeg' độ quanh hướng đó.
    void addSlash(sf::Vector2f origin, sf::Vector2f dir, float radius, float spanDeg, sf::Color color);
    void addSparks(sf::Vector2f position, sf::Color color, int count);
    void addText(sf::Vector2f position, const std::string& text, sf::Color color, unsigned size = 22);

    void drawWorld(sf::RenderTarget& target, const sf::Font* font) const;  // vệt chém, tia lửa, số bay (toạ độ thế giới)

    // Hạt trang trí theo chủ đề của level (đom đóm, tàn lửa, bụi)
    void setAmbient(AmbientType type, sf::Color color, sf::Vector2f mapSize);
    void updateAmbient(float dt);
    void drawAmbient(sf::RenderTarget& target, const sf::FloatRect& view) const;

    std::size_t activeCount() const { return m_slashes.size() + m_sparks.size() + m_texts.size(); }

    static constexpr std::size_t kMaxSlashes = 12;
    static constexpr std::size_t kMaxSparks = 300;
    static constexpr std::size_t kMaxTexts = 48;

private:
    struct Slash {
        sf::Vector2f origin;
        float angle = 0.f;   // radian
        float radius = 60.f;
        float span = 2.f;    // radian
        sf::Color color;
        float age = 0.f;
        float life = 0.22f;
    };
    struct Spark {
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Color color;
        float age = 0.f;
        float life = 0.4f;
        float size = 3.f;
    };
    struct FloatingText {
        sf::Vector2f position;
        std::string text;
        sf::Color color;
        unsigned size = 22;
        float age = 0.f;
        float life = 0.9f;
    };
    struct AmbientParticle {
        sf::Vector2f position;
        float phase = 0.f;
        float speed = 1.f;
        float size = 2.f;
    };

    std::vector<Slash> m_slashes;
    std::vector<Spark> m_sparks;
    std::vector<FloatingText> m_texts;

    std::vector<AmbientParticle> m_ambient;
    AmbientType m_ambientType = AmbientType::None;
    sf::Color m_ambientColor = sf::Color::White;
    sf::Vector2f m_mapSize{1800.f, 1200.f};
    float m_ambientTime = 0.f;
};
