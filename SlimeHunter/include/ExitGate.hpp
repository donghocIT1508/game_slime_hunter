#pragma once
// ExitGate.hpp - Cổng thoát của mỗi màn, vẽ hoàn toàn bằng hình SFML (không cần ảnh).
//   * ĐÓNG: vòng tròn đỏ xám mờ có dấu X (chưa đủ điều kiện).
//   * MỞ  : vòng phép thuật xanh phát sáng, cột sáng và các tia lấp lánh bay lên -> nổi bật hơn mọi vật trang trí.

#include <SFML/Graphics.hpp>

class ExitGate {
public:
    void setup(sf::Vector2f position, float radius);  // đặt cổng cho màn mới (luôn bắt đầu ở trạng thái ĐÓNG)
    void setOpen(bool open);
    bool isOpen() const { return m_open; }
    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    // Chân Player (hình tròn bán kính footRadius) có chạm vùng kích hoạt của cổng không?
    bool touches(sf::Vector2f footPosition, float footRadius) const;

    sf::Vector2f getPosition() const { return m_position; }
    float getRadius() const { return m_radius; }

private:
    sf::Vector2f m_position;
    float m_radius = 40.f;
    bool m_open = false;
    float m_time = 0.f;      // đồng hồ cho hiệu ứng nhấp nháy/xoay
    float m_openAnim = 1.f;  // 0 -> 1 trong ~0.8 giây sau khi mở: vòng sáng lan rộng
};
