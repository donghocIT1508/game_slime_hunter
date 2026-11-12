#pragma once
// PathField.hpp - "Bản đồ chỉ đường" tới một mục tiêu (thường là Player), tính bằng thuật toán BFS.
//
// Vấn đề: nếu quái chỉ đi thẳng về phía Player, chúng sẽ kẹt sau thân cây/vách đá/hồ nước.
// Giải pháp: BFS (duyệt theo chiều rộng) lan ra từ ô chứa Player trên lưới thô của bản đồ.
// Mỗi ô nhận một con số = "số bước tới Player". Quái chỉ cần đi sang ô lân cận có số NHỎ HƠN
// là chắc chắn đi đúng đường, kể cả khi phải vòng qua vật cản.

#include <SFML/Graphics.hpp>

#include <vector>

class GameMap;

class PathField {
public:
    // Tính lại toàn bộ trường khoảng cách với mục tiêu mới. Chi phí nhỏ (lưới ~15.000 ô) nên gọi ~5 lần/giây.
    void build(const GameMap& map, sf::Vector2f target);

    // Hướng (vector độ dài 1) cần đi từ vị trí 'from' để tới mục tiêu. Trả (0,0) nếu không có đường.
    // 'radius' là bán kính chân của người đi (dùng để kiểm tra đường thẳng tới điểm đích tạm thời có thông thoáng không).
    sf::Vector2f getDirection(const GameMap& map, sf::Vector2f from, float radius = 12.f) const;

    // Số ô (bước) từ 'from' tới mục tiêu; -1 nếu không có đường. Dùng để kiểm tra lối ra có tới được không.
    int getDistance(sf::Vector2f from) const;

private:
    int m_width = 0;
    int m_height = 0;
    float m_cellSize = 12.f;
    std::vector<int> m_dist;  // số bước tới mục tiêu; -1 = chưa lan tới / bị chặn
};
