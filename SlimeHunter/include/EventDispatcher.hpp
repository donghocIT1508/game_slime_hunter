#pragma once
// EventDispatcher.hpp - Observer Pattern cho các sự kiện trong Game.
// Cho phép tách rời hoàn toàn Game logic với Audio, Thành tựu (Achievements), Analytics.
// Thể hiện Observer Pattern trong OOP.

#include <algorithm>
#include <vector>

enum class GameEvent {
    PlayerAttack,
    PlayerDamaged,
    PlayerDashed,
    PlayerLeveledUp,
    MonsterKilled,
    BossSpawned,
    ItemCollected,
    BombExploded,
    GateOpened,
    LevelCleared,
    GameOver
};

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onGameEvent(GameEvent event, const void* data) = 0;
};

class EventDispatcher {
public:
    void addObserver(IGameObserver* observer) {
        if (observer) {
            m_observers.push_back(observer);
        }
    }

    void removeObserver(IGameObserver* observer) {
        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
    }

    void dispatch(GameEvent event, const void* data = nullptr) {
        for (IGameObserver* obs : m_observers) {
            if (obs) obs->onGameEvent(event, data);
        }
    }

private:
    std::vector<IGameObserver*> m_observers;
};
