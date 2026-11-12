#pragma once
// Balance.hpp - TOÀN BỘ công thức/hằng số cân bằng game nằm ở đây (header-only).
// Muốn game dễ hơn/khó hơn: chỉ cần sửa các con số trong file này, không phải tìm khắp chương trình.

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace balance {

// ============================================================================
//  1) NÂNG CẤP PLAYER khi hoàn thành màn LẦN ĐẦU
// ============================================================================
constexpr int kBasePlayerMaxHp = 100;     // chỉ số lúc tạo tài khoản mới
constexpr int kBasePlayerDamage = 20;
constexpr float kPlayerHpGrowth = 1.15f;      // +15% máu tối đa mỗi màn
constexpr float kPlayerDamageGrowth = 1.15f;  // +15% sát thương mỗi màn

inline int upgradedMaxHp(int currentMaxHp) { return static_cast<int>(std::lround(static_cast<float>(currentMaxHp) * kPlayerHpGrowth)); }
inline int upgradedDamage(int currentDamage) { return static_cast<int>(std::lround(static_cast<float>(currentDamage) * kPlayerDamageGrowth)); }

// Bonus từ cấp độ kinh nghiệm TRONG PHIÊN chơi (đã có từ trước, không lưu vào CSV)
constexpr float kExpLevelHpBonus = 10.f;
constexpr float kExpLevelDamageBonus = 4.f;

// ============================================================================
//  2) QUÁI MẠNH HƠN Ở LEVEL CAO (level 1 = hệ số 1.0)
// ============================================================================
struct MonsterScale {
    float hp = 1.f;
    float damage = 1.f;
    float speed = 1.f;
    float cooldown = 1.f;  // nhân với thời gian hồi chiêu: nhỏ hơn 1 = đánh nhanh hơn
};

constexpr float kMonsterHpGrowth = 1.20f;       // máu x1.20 mỗi level
constexpr float kMonsterDamageGrowth = 1.10f;   // sát thương x1.10 mỗi level
constexpr float kMonsterSpeedGrowth = 0.04f;    // tốc độ +4% mỗi level...
constexpr float kMonsterSpeedMax = 1.25f;       // ...nhưng không quá +25%
constexpr float kMonsterCooldownDecay = 0.94f;  // hồi chiêu x0.94 mỗi level...
constexpr float kMonsterCooldownMin = 0.65f;    // ...nhưng không dưới 65%

inline MonsterScale monsterScaleForLevel(int level) {
    const float n = static_cast<float>(std::max(level, 1) - 1);
    MonsterScale s;
    s.hp = std::pow(kMonsterHpGrowth, n);
    s.damage = std::pow(kMonsterDamageGrowth, n);
    s.speed = std::min(1.f + kMonsterSpeedGrowth * n, kMonsterSpeedMax);
    s.cooldown = std::max(std::pow(kMonsterCooldownDecay, n), kMonsterCooldownMin);
    return s;
}

// ============================================================================
//  3) HIỆU ỨNG ĐÒN CHÉM (mở khóa dần khi hoàn thành màn)
//     tier = max_level_unlocked (kẹp trong 1..5): chưa qua màn nào = tier 1.
// ============================================================================
constexpr int kMaxSlashTier = 5;

struct SlashProfile {
    const char* name = "Chém cơ bản";
    const char* unlockText = "";            // thông báo khi mở khóa
    float reach = 55.f;                     // hộp đánh đặt cách ngực Player bao xa theo hướng chém
    float size = 90.f;                      // cạnh hộp đánh vuông
    float damageMultiplier = 1.f;
    float slowDuration = 0.f;               // 0 = không làm chậm
    float slowMultiplier = 1.f;             // tốc độ còn lại (0.55 = chậm 45%)
    float dotDuration = 0.f;                // 0 = không có sát thương theo thời gian
    float dotTickInterval = 0.5f;           // cứ 0.5 giây gây sát thương một lần (KHÔNG phải mỗi frame)
    float dotDamageFactor = 0.f;            // sát thương mỗi nhịp = factor x sát thương cơ bản của Player
    float arcRadius = 62.f;                 // bán kính vệt chém hiển thị
    float arcSpanDeg = 110.f;               // độ rộng góc vệt chém
    std::uint8_t r = 255, g = 255, b = 255; // màu vệt chém
};

inline int slashTierForProgress(int maxLevelUnlocked) { return std::min(std::max(maxLevelUnlocked, 1), kMaxSlashTier); }

inline SlashProfile slashProfileForTier(int tier) {
    SlashProfile p;
    tier = std::min(std::max(tier, 1), kMaxSlashTier);
    if (tier >= 2) {  // vệt chém mạnh: rộng hơn, sát thương +20%
        p.name = "Vệt chém mạnh";
        p.unlockText = "Vệt chém mạnh: tầm xa hơn, sát thương +20%";
        p.reach = 62.f; p.size = 102.f; p.damageMultiplier = 1.2f;
        p.arcRadius = 74.f; p.arcSpanDeg = 125.f; p.r = 255; p.g = 220; p.b = 110;
    }
    if (tier >= 3) {  // làm chậm
        p.name = "Chém làm chậm";
        p.unlockText = "Chém làm chậm: quái trúng đòn bị chậm 2 giây";
        p.slowDuration = 2.0f; p.slowMultiplier = 0.55f;
        p.r = 120; p.g = 200; p.b = 255;
    }
    if (tier >= 4) {  // sát thương theo thời gian (thiêu đốt)
        p.name = "Chém thiêu đốt";
        p.unlockText = "Chém thiêu đốt: quái bị bỏng, mất máu theo thời gian";
        p.dotDuration = 3.0f; p.dotTickInterval = 0.5f; p.dotDamageFactor = 0.22f;
        p.r = 255; p.g = 150; p.b = 60;
    }
    if (tier >= 5) {  // nâng cấp tất cả hiệu ứng đã có
        p.name = "Chém tối thượng";
        p.unlockText = "Chém tối thượng: tầm xa, chậm lâu hơn, thiêu đốt mạnh hơn";
        p.reach = 72.f; p.size = 118.f; p.damageMultiplier = 1.35f;
        p.slowDuration = 3.0f; p.slowMultiplier = 0.45f;
        p.dotDuration = 4.0f; p.dotDamageFactor = 0.28f;
        p.arcRadius = 88.f; p.arcSpanDeg = 140.f; p.r = 255; p.g = 110; p.b = 210;
    }
    return p;
}

}  // namespace balance
