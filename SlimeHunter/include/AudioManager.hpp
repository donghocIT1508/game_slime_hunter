#pragma once
// AudioManager.hpp - Quản lý toàn bộ âm thanh (SFX & BGM) bằng SFML Audio.
// Kế thừa IGameObserver (Observer Pattern): tự động phát âm thanh khi có sự kiện từ EventDispatcher.
// Tự động tổng hợp âm thanh bằng thuật toán sóng âm (Procedural Sound Synthesis) khi không có file âm thanh.

#include "EventDispatcher.hpp"

#include <SFML/Audio.hpp>
#include <deque>
#include <memory>
#include <vector>

class AudioManager : public IGameObserver {
public:
    AudioManager();
    ~AudioManager() override = default;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void init();
    void update(float dt);

    void onGameEvent(GameEvent event, const void* data) override;

    void playSlash();
    void playHit();
    void playMonsterDie();
    void playPickup();
    void playLevelUp();
    void playDash();
    void playBomb();
    void playGateOpen();

    void toggleMute();
    bool isMuted() const { return m_muted; }

private:
    void playSound(const sf::SoundBuffer& buffer, float pitch = 1.0f, float volume = 80.f);
    void generateProceduralSounds();

    bool m_initialized = false;
    bool m_muted = false;

    sf::SoundBuffer m_bufSlash;
    sf::SoundBuffer m_bufHit;
    sf::SoundBuffer m_bufDie;
    sf::SoundBuffer m_bufPickup;
    sf::SoundBuffer m_bufLevelUp;
    sf::SoundBuffer m_bufDash;
    sf::SoundBuffer m_bufBomb;
    sf::SoundBuffer m_bufGate;

    // Pool các Sound đang phát
    std::deque<sf::Sound> m_activeSounds;
    const std::size_t kMaxConcurrentSounds = 16;

    // BGM Synth loop
    sf::SoundBuffer m_bufBgm;
    std::unique_ptr<sf::Sound> m_bgmSound;
};
