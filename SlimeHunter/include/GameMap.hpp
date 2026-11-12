#pragma once
// GameMap.hpp - Bản đồ rừng: hình nền + mặt nạ va chạm.
//
// Va chạm dùng "mặt nạ" (mask): một ảnh đen/trắng nhỏ (600x400), mỗi pixel ứng với một ô 3x3 pixel
// của bản đồ. Pixel trắng = có cây/đá/nước, không đi vào được. Kiểm tra va chạm chỉ là tra một ô
// trong mảng - nhanh và dễ hiểu, không cần thuật toán phức tạp.

#include <SFML/Graphics.hpp>

#include <vector>

class Assets;

class GameMap {
public:
    explicit GameMap(const Assets& assets, int levelId = 1);
    GameMap(const GameMap&) = delete;
    GameMap& operator=(const GameMap&) = delete;

    // Đổi sang bản đồ của level khác: thay nền, mặt nạ va chạm và lưới dẫn đường (dùng lại cùng một đối tượng GameMap).
    void loadLevel(const Assets& assets, int levelId);

    void draw(sf::RenderTarget& target) const;
    void drawCollisionOverlay(sf::RenderTarget& target) const;  // chế độ debug (phím F1)

    sf::Vector2f getSize() const { return m_size; }

    // Điểm (toạ độ thế giới) có nằm trong vùng cấm hoặc ngoài bản đồ không?
    bool isBlocked(sf::Vector2f worldPos) const;

    // Hình tròn tâm 'center', bán kính 'radius' (vùng chân nhân vật) có nằm hoàn toàn trên đất trống không?
    bool isCircleFree(sf::Vector2f center, float radius) const;

    // Di chuyển 'delta' nhưng tách riêng trục X và trục Y: gặp vật cản ở trục nào thì chỉ dừng trục đó,
    // nhờ vậy nhân vật "trượt" dọc theo thân cây thay vì bị kẹt cứng.
    sf::Vector2f moveAndSlide(sf::Vector2f position, sf::Vector2f delta, float radius) const;

    // Tìm điểm trống gần 'target' nhất (xoắn ốc ra ngoài). Dùng để đặt Player lúc bắt đầu.
    sf::Vector2f findFreeSpotNear(sf::Vector2f target, float radius) const;

    // Đoạn thẳng a->b có đi được hết không (không đụng vật cản)? Dùng để quyết định "đi thẳng" hay "tìm đường vòng".
    bool isLineFree(sf::Vector2f a, sf::Vector2f b, float radius) const;

    // ---- Lưới dẫn đường thô (mỗi ô 12x12 px), dùng cho thuật toán BFS trong PathField ----
    int getNavWidth() const { return m_navW; }
    int getNavHeight() const { return m_navH; }
    float getNavCellSize() const { return kNavCellSize; }
    bool isNavBlocked(int cx, int cy) const;  // ô ngoài lưới cũng tính là bị chặn

    // Thử ngẫu nhiên một điểm trống trên toàn bản đồ. Trả về false nếu thử nhiều lần mà không được.
    bool findRandomFreeSpot(float radius, sf::Vector2f& outPosition) const;

private:
    sf::Texture m_overlayTexture;  // khai báo TRƯỚC m_overlay vì sprite cần texture đã tồn tại
    sf::Sprite m_background;
    sf::Sprite m_overlay;

    static constexpr float kNavCellSize = 12.f;
    std::vector<unsigned char> m_navBlocked;  // lưới thô: 1 = ô này quái KHÔNG đứng vừa được
    int m_navW = 0;
    int m_navH = 0;

    std::vector<unsigned char> m_blocked;  // 1 = cấm, 0 = đi được; xếp theo hàng
    int m_cellsW = 0;
    int m_cellsH = 0;
    float m_cellSize = 3.f;  // 1 ô mặt nạ = bao nhiêu pixel bản đồ
    sf::Vector2f m_size;
};
