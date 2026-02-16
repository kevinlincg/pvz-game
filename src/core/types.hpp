// ============================================
// Plant Legends - 核心類型定義
// ============================================

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cmath>

// 前向聲明 SF3 類型
namespace SF3 {
    struct Rect {
        float x, y, w, h;
        Rect() : x(0), y(0), w(0), h(0) {}
        Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    };
    
    struct Color {
        unsigned char r, g, b, a;
        Color() : r(255), g(255), b(255), a(255) {}
        Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) 
            : r(r), g(g), b(b), a(a) {}
    };
}

namespace PL {

// 基本類型
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

// 2D 向量
struct Vec2 {
    f32 x = 0.0f;
    f32 y = 0.0f;
    
    Vec2() = default;
    Vec2(f32 x, f32 y) : x(x), y(y) {}
    
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(f32 scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(f32 scalar) const { return Vec2(x / scalar, y / scalar); }
    
    f32 length() const { return std::sqrt(x * x + y * y); }
    f32 lengthSq() const { return x * x + y * y; }
    Vec2 normalized() const {
        f32 len = length();
        return len > 0 ? *this / len : Vec2();
    }
    
    f32 distance(const Vec2& other) const {
        return (*this - other).length();
    }
    
    f32 distanceSq(const Vec2& other) const {
        return (*this - other).lengthSq();
    }
};

// 前向宣告 SF3 類型
namespace SF3 {
    struct Rect;
    struct Color;
}

// 使用 SF3 的 Rect 和 Color
using Rect = SF3::Rect;
using Color = SF3::Color;

// 網格座標
struct GridCoord {
    i32 col = 0;
    i32 row = 0;
    
    GridCoord() = default;
    GridCoord(i32 col, i32 row) : col(col), row(row) {}
    
    bool operator==(const GridCoord& other) const {
        return col == other.col && row == other.row;
    }
    
    bool operator!=(const GridCoord& other) const {
        return !(*this == other);
    }
};

// 稀有度
enum class Rarity {
    Common,
    Rare,
    Epic,
    Legendary
};

// 元素類型
enum class Element {
    None,
    Fire,
    Ice,
    Lightning,
    Poison
};

// 狀態效果類型
enum class StatusType {
    None,
    Burn,      // 燃燒
    Slow,      // 減速
    Freeze,    // 凍結
    Poison,    // 中毒
    Stun       // 暈眩
};

// 狀態效果
struct StatusEffect {
    StatusType type = StatusType::None;
    f32 duration = 0.0f;
    f32 value = 0.0f;  // 傷害/減速百分比等
    f32 timer = 0.0f;
    
    StatusEffect() = default;
    StatusEffect(StatusType type, f32 duration, f32 value)
        : type(type), duration(duration), value(value), timer(0.0f) {}
    
    bool active() const { return timer < duration; }
    f32 remaining() const { return duration - timer; }
    f32 progress() const { return timer / duration; }
};

// 統計數據
struct Stats {
    f32 hp = 100.0f;
    f32 maxHp = 100.0f;
    f32 damage = 10.0f;
    f32 attackSpeed = 1.0f;  // 每秒攻擊次數
    f32 range = 100.0f;
    f32 speed = 50.0f;       // 移動速度
    f32 critRate = 0.1f;     // 爆擊率
    f32 critMult = 2.0f;     // 爆擊倍率
    f32 armor = 0.0f;        // 減傷
    
    Stats() = default;
};

// 實體類型
enum class EntityType {
    None,
    Plant,
    Enemy,
    Projectile,
    Effect
};

// 前向宣告
class Entity;
class Plant;
class Enemy;
class Projectile;
class Game;

// 實體指標
using EntityPtr = std::shared_ptr<Entity>;
using PlantPtr = std::shared_ptr<Plant>;
using EnemyPtr = std::shared_ptr<Enemy>;
using ProjectilePtr = std::shared_ptr<Projectile>;

// 回調函數類型
using OnDeathCallback = std::function<void()>;
using OnAttackCallback = std::function<void()>;
using OnHitCallback = std::function<void(f32 damage)>;

} // namespace PL
