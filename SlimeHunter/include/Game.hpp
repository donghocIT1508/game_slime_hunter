#pragma once
// Game.hpp - Quản lý vòng đời game: cửa sổ, game loop, các trạng thái, tài khoản/tiến trình, level, sinh quái,
// chiến đấu, camera, âm thanh, vũ khí và vẽ giao diện.

#include "Assets.hpp"
#include "AudioManager.hpp"
#include "Balance.hpp"
#include "Effects.hpp"
#include "EventDispatcher.hpp"
#include "ExitGate.hpp"
#include "GameMap.hpp"
#include "Item.hpp"
#include "Levels.hpp"
#include "Monster.hpp"
#include "MonsterFactory.hpp"
#include "PathField.hpp"
#include "Player.hpp"
#include "PowerUp.hpp"
#include "Projectile.hpp"
#include "Shop.hpp"
#include "Slime.hpp"
#include "UserRepository.hpp"
#include "Weapon.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum class GameState { Login, Menu, Playing, Paused, LevelComplete, GameOver, Victory, Leaderboard, Shop, PowerUpSelect };

// Tiến trình bên trong MỘT màn chơi
enum class LevelPhase { Fighting, GateOpen, Completed };

enum class FadeStage { None, FadeOut, FadeIn };

class Game {
public:
    explicit Game(const std::string& dataDirOverride = "");
    void run();

    int runSmokeTest(const std::string& screenshotPrefix);

private:
    enum class Align { Left, Center, Right };

    void processEvents();
    void update(float dt);
    void render();

    void setState(GameState state);
    void toggleFullscreen();

    void handleKeyPressed(sf::Keyboard::Key key);
    void handleTextEntered(char32_t unicode);
    void handleLoginClick(const sf::Vector2f& mpos);
    void handleMenuClick(const sf::Vector2f& mpos);
    void handleShopClick(const sf::Vector2f& mpos);
    void cycleWeapon(int direction);

    // ---- Tài khoản và tiến trình ----
    void submitLoginForm();
    void startGuestSession();
    void logout();
    void enterMenuWithProgress();
    bool saveProgress();
    void flushTotals();
    void saveOnExit();

    // ---- Level ----
    void previewLevel(int levelId);
    void startRun(int levelId);
    void loadLevel(int levelId);
    void updateLevelFlow(float dt);
    void completeLevel();
    void advanceToNextLevel();
    void enterGameOver();
    void goToMenu();
    void showToast(const std::string& text, float seconds);
    int monstersRemaining() const;

    // ---- Logic khi đang chơi ----
    PlayerInput readInput();
    PlayerInput autopilotInput();
    void updatePlaying(float dt);
    void updateLevelComplete(float dt);
    void updateFade(float dt);
    void updateSpawner(float dt);
    bool findSpawnPosition(float radius, sf::Vector2f& out) const;
    MonsterType chooseMonsterType() const;
    bool spawnMonster(MonsterType type);
    void separateMonsters();
    void resolvePlayerAttack();
    void resolveMonsterAttacks();
    void resolveProjectiles();
    void updateProjectiles(float dt);
    void updateItems(float dt);
    void collectDeathRewards();
    void removeDeadMonsters();
    void updateCamera(float dt, bool snap);
    int countAliveMonsters() const;
    const Monster* findBoss() const;

    // ---- Rogue-lite & Shop & Leaderboard ----
    void triggerPowerUpSelection();
    void selectPowerUp(int index);
    void openLeaderboard();
    void openShop();

    // ---- Vẽ ----
    void drawWorld();
    void drawGateArrow();
    void drawDebug();
    void drawHud();
    void drawLoginScreen();
    void drawEyeIcon(sf::Vector2f center, bool open, bool hovered);
    void drawMenu();
    void drawPauseOverlay();
    void drawLevelCompleteOverlay();
    void drawGameOverOverlay();
    void drawVictoryOverlay();
    void drawLeaderboard();
    void drawShop();
    void drawPowerUpSelectOverlay();
    void drawToastAndHints();
    void drawFade();
    void drawText(const std::string& text, unsigned size, sf::Vector2f pos, sf::Color color = sf::Color::White,
                  Align align = Align::Left);
    void drawBar(sf::Vector2f pos, sf::Vector2f size, float ratio, sf::Color fill);
    void drawDim(sf::Color color);
    static std::string formatTime(float seconds);

    // THỨ TỰ KHAI BÁO = THỨ TỰ KHỞI TẠO
    sf::RenderWindow m_window;
    Assets m_assets;
    GameMap m_map;
    Player m_player;
    UserRepository m_repo;
    AudioManager m_audio;
    EventDispatcher m_dispatcher;

    std::vector<std::unique_ptr<Monster>> m_monsters;
    std::vector<Projectile> m_projectiles;
    std::vector<std::unique_ptr<Item>> m_items;
    PathField m_pathToPlayer;
    float m_pathTimer = 0.f;
    Effects m_effects;
    ExitGate m_gate;

    sf::View m_camera;
    sf::View m_uiView;
    sf::Vector2f m_cameraPos{0.f, 0.f};

    GameState m_state = GameState::Login;
    bool m_debug = false;

    // ---- Dữ liệu lâu dài ----
    std::optional<UserRecord> m_user;
    bool m_guest = false;
    ProgressRecord m_progress;
    int m_unsavedKills = 0;
    int m_unsavedScore = 0;
    std::string m_loginUser;
    std::string m_loginPass;
    bool m_showPassword = false;
    int m_loginField = 0;
    bool m_registerMode = false;
    std::string m_loginMessage;
    bool m_loginMessageIsError = false;
    int m_selectedLevel = 1;

    // ---- Dữ liệu trong phiên ----
    int m_levelId = 1;
    LevelPhase m_phase = LevelPhase::Fighting;
    int m_levelTotalToSpawn = 0;
    int m_spawnedCount = 0;
    int m_levelKills = 0;
    float m_spawnTimer = 0.f;
    float m_survivalTime = 0.f;
    int m_score = 0;
    int m_kills = 0;
    float m_gameOverDelay = 0.f;
    bool m_gameOverHandled = false;
    float m_levelUpTimer = 0.f;
    float m_levelCompleteTimer = 0.f;
    bool m_lastSaveOk = true;
    std::vector<std::string> m_rewardLines;

    // Roguelite & Shop & Leaderboard
    std::vector<PowerUpCard> m_powerUpChoices;
    std::vector<LeaderboardEntry> m_topScores;
    GameState m_leaderboardPrevState = GameState::Menu;
    std::vector<ShopItem> m_shopItems;
    int m_selectedShopIndex = 0;

    FadeStage m_fadeStage = FadeStage::None;
    float m_fadeAlpha = 0.f;

    std::string m_toast;
    float m_toastTimer = 0.f;
    float m_gateHintTimer = 0.f;

    float m_menuTime = 0.f;
    float m_fps = 60.f;

    // Autopilot
    bool m_autopilot = false;
    bool m_autopilotPassive = false;
    bool m_autopilotGod = false;
    PathField m_autopilotPath;
    float m_autopilotPathTimer = 0.f;
    sf::Vector2f m_autoLastPos{0.f, 0.f};
    float m_autoStuckTime = 0.f;
    float m_autoSidestepTime = 0.f;
    sf::Vector2f m_autoSidestepDir{0.f, 0.f};
    std::string m_pendingScreenshot;
};
