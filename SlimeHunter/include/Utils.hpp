#pragma once
// Utils.hpp - Các hàm toán học nhỏ dùng chung (header-only).
// SFML 3 có sẵn sf::Vector2f::length()/normalized(), nhưng normalized() của vector (0,0)
// là hành vi không xác định. Hàm của ta xử lý an toàn trường hợp đó.

#include <SFML/Graphics.hpp>

#include <cmath>
#include <random>
#include <string>

namespace util {

inline float length(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

inline float distance(sf::Vector2f a, sf::Vector2f b) { return length(b - a); }

// Chuẩn hóa vector về độ dài 1 (chỉ giữ hướng). Vector (0,0) trả về (0,0), không bị chia cho 0.
inline sf::Vector2f normalized(sf::Vector2f v) {
    const float len = length(v);
    if (len < 0.00001f) return {0.f, 0.f};
    return v / len;
}

inline float clampf(float value, float lo, float hi) { return value < lo ? lo : (value > hi ? hi : value); }

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

// Hiệu ứng "nhanh lúc đầu, chậm dần về cuối" - dùng cho cú vung gậy.
inline float easeOutQuad(float t) { return 1.f - (1.f - t) * (1.f - t); }

// Nội suy góc (độ) theo đường ngắn nhất, tránh việc xoay vòng 350 độ khi chỉ cần xoay 10 độ.
inline float lerpAngleDeg(float a, float b, float t) {
    float diff = std::fmod(b - a, 360.f);
    if (diff > 180.f) diff -= 360.f;
    if (diff < -180.f) diff += 360.f;
    return a + diff * t;
}

inline std::mt19937& rng() {
    static std::mt19937 generator{std::random_device{}()};
    return generator;
}

inline float randomFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng());
}

inline int randomInt(int lo, int hi) {  // gồm cả lo và hi
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng());
}

// File nguồn lưu UTF-8; sf::String cần được báo rõ đây là UTF-8 thì tiếng Việt mới hiển thị đúng dấu.
inline sf::String utf8(const std::string& text) { return sf::String::fromUtf8(text.begin(), text.end()); }

}  // namespace util
