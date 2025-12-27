#include "../include/Game.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

Vec2 Lerp(const Vec2& a, const Vec2& b, float alpha) {
    return { a.x * (1.0f - alpha) + b.x * alpha, a.y * (1.0f - alpha) + b.y * alpha };
}

static std::vector<sf::IntRect> npcTypes = {
    sf::IntRect(7, 16, 52, 135), sf::IntRect(129, 16, 49, 93), sf::IntRect(189, 16, 50, 97),
    sf::IntRect(426, 21, 60, 137), sf::IntRect(308, 20, 52, 92), sf::IntRect(247, 123, 52, 92),
    sf::IntRect(189, 120, 51, 100), sf::IntRect(65, 120, 51, 100), sf::IntRect(186, 226, 55, 103),
    sf::IntRect(308, 223, 55, 106), sf::IntRect(236, 346, 45, 86), sf::IntRect(10, 346, 45, 86)
};

static std::vector<sf::IntRect> treeTypes = {
    sf::IntRect(395, 5, 108, 122), sf::IntRect(258, 260, 116, 124), sf::IntRect(260, 385, 122, 126)
};

Game::Game() : _gen(_rd()), _currentState(GameState::MENU), _isNight(false), _isFullscreen(false),
_score(0.f), _highScore(0.f), _isNewRecord(false), _menuTimer(0.f),
_accumulator(0.f), _currentWaterOffset(0.f)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    _window.create(sf::VideoMode(Config::ScreenWidth, Config::ScreenHeight), "Highway Racer - v3.2", sf::Style::Titlebar | sf::Style::Close);
    _window.setFramerateLimit(144);
    _camera.reset(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f));

    _assets.LoadTexture("Cars", "assets/bk_cars1.a.png");
    _assets.LoadTexture("Tree", "assets/tree1.png");
    _assets.LoadTexture("Water", "assets/water.jpg");
    _assets.LoadFont("MainFont", "assets/FSEX300.ttf");
    _assets.LoadSoundBuffer("Engine", "assets/engine.wav");
    _assets.LoadSoundBuffer("Menu", "assets/menu.wav");
    _assets.LoadSoundBuffer("Crash", "assets/crash.wav");

    _engineSound.setBuffer(_assets.GetSoundBuffer("Engine"));
    _engineSound.setLoop(true);
    _engineSound.setVolume(85.f);

    _menuSound.setBuffer(_assets.GetSoundBuffer("Menu"));
    _menuSound.setLoop(true);
    _menuSound.setVolume(50.f);
    _menuSound.play();

    _crashSound.setBuffer(_assets.GetSoundBuffer("Crash"));
    _crashSound.setVolume(80.f);

    _player = new Player(
        { 300.f, 810.f },
        { 0.f, 0.f },
        _assets.GetTexture("Cars"),
        sf::IntRect(72, 446, 51, 96),
        800.f, 0.96f, 5
    );

    std::ifstream inputFile("highscore.txt");
    if (inputFile.is_open()) {
        inputFile >> _highScore;
        inputFile.close();
    }

    setupText(_gameOverText, "GAME OVER", 100, sf::Color::Red, true);
    _gameOverText.setOutlineThickness(5.f);
    setupText(_restartText, "PRESS 'R' TO RESTART", 40, sf::Color::White, false);
    _restartText.setOutlineThickness(3.f);
    setupText(_titleText, "HIGHWAY RACER", 100, sf::Color::Yellow, true);
    setupText(_startText, "PRESS 'ENTER' TO START", 40, sf::Color::White, false);
    setupText(_pauseText, "PAUSED", 80, sf::Color::Cyan, false);
    _pauseText.setOutlineThickness(3.f);
    setupText(_pauseSubText, "PRESS 'ESC' TO CONTINUE", 30, sf::Color::White, false);
    _pauseSubText.setOutlineThickness(2.f);
    setupText(_returnMenuText, "PRESS 'M' TO RETURN TO MENU", 30, sf::Color::White, false);
    _returnMenuText.setOutlineThickness(2.f);
    setupText(_scoreText, "", 35, sf::Color::White, false);
    _scoreText.setOutlineThickness(2.f);
    setupText(_highScoreText, "", 35, sf::Color::Yellow, false);
    _highScoreText.setOutlineThickness(2.f);
    setupText(_modeText, "PRESS 'N' FOR NIGHT MODE", 30, sf::Color::White, false);
    resetGame(false);
}

Game::~Game() {
    delete _player;
}

void Game::setupText(sf::Text& text, const std::string& str, int size, sf::Color color, bool bold) {
    text.setFont(_assets.GetFont("MainFont"));
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setOutlineColor(sf::Color::Black);
    if (bold) text.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Game::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        processEvents();
        float frameTime = clock.restart().asSeconds();
        _menuTimer += frameTime;
        if (frameTime > 0.25f) frameTime = 0.25f;
        if (_currentState == GameState::PLAYING) _accumulator += frameTime;
        else _accumulator = 0.f;
        update(frameTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) _window.close();
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
            _isFullscreen = !_isFullscreen;
            if (_isFullscreen) {
                _window.create(sf::VideoMode(Config::ScreenWidth, Config::ScreenHeight), "Highway Racer", sf::Style::Fullscreen);
            }
            else {
                _window.create(sf::VideoMode(Config::ScreenWidth, Config::ScreenHeight), "Highway Racer", sf::Style::Titlebar | sf::Style::Close);
            }
            _window.setFramerateLimit(144);
            _window.setView(_camera);
        }
        if (_currentState == GameState::MENU && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                _currentState = GameState::PLAYING;
                _menuSound.stop();
                if (_engineSound.getStatus() != sf::Sound::Playing) _engineSound.play();
            }
            if (event.key.code == sf::Keyboard::N) {
                _isNight = !_isNight;
                _modeText.setString(_isNight ? "PRESS 'N' FOR NIGHT MODE: ON" : "PRESS 'N' FOR NIGHT MODE: OFF");
                sf::FloatRect mb = _modeText.getLocalBounds();
                _modeText.setOrigin(mb.width / 2.f, mb.height / 2.f);
            }
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (_currentState == GameState::PLAYING) {
                _currentState = GameState::PAUSED;
                _engineSound.pause();
            }
            else if (_currentState == GameState::PAUSED) {
                _currentState = GameState::PLAYING;
                _engineSound.play();
            }
        }
        if (_currentState == GameState::GAMEOVER && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
            resetGame(true);
            _currentState = GameState::PLAYING;
            _engineSound.play();
        }
        if (_currentState == GameState::PAUSED && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
            _currentState = GameState::MENU;
            resetGame(true);
            _engineSound.stop();
            _menuSound.play();
        }
    }
}

void Game::resetGame(bool fullReset) {
    if (fullReset) {
        _score = 0.f;
        _isNewRecord = false;
        int restartLane = std::uniform_int_distribution<>(3, 5)(_gen);
        _player->position = { 300.f, Config::LanesY[restartLane] };
        _player->velocity = { 0.f, 0.f };
        _player->acceleration = { 0.f, 0.f };
    }
    _traffic.clear();
    std::uniform_real_distribution<float> speedDist(250.f, 500.f);
    for (int i = 0; i < 34; ++i) {
        int laneIndex;
        float dirX;
        if (i < 15) {
            laneIndex = std::uniform_int_distribution<>(0, 2)(_gen);
            dirX = -speedDist(_gen);
        }
        else {
            laneIndex = std::uniform_int_distribution<>(3, 5)(_gen);
            dirX = speedDist(_gen);
        }
        float spawnX = static_cast<float>((_gen() % 15000) - 5000);
        int chance = std::uniform_int_distribution<>(1, 100)(_gen);
        int selectedIdx = 0;
        if (chance <= 3) selectedIdx = 0;
        else if (chance <= 7) selectedIdx = 1;
        else if (chance <= 10) selectedIdx = 2;
        else if (chance <= 13) selectedIdx = 3;
        else selectedIdx = std::uniform_int_distribution<>(4, (int)npcTypes.size() - 1)(_gen);
        float npcSpeed = std::uniform_real_distribution<float>(350.f, 550.f)(_gen);
        _traffic.emplace_back(
            Vec2{ spawnX, Config::LanesY[laneIndex] },
            Vec2{ dirX, 0.f },
            _assets.GetTexture("Cars"),
            npcTypes[selectedIdx],
            npcSpeed, 1.0f, laneIndex
        );
        _traffic.back().canOvertake = (std::uniform_int_distribution<>(1, 3)(_gen) == 1);
    }
    _forest.clear();
    for (float y = 1000.f; y < 1100.f; y += 30.f) {
        for (float x = -5000.f; x < 20000.f; x += 130.f) {
            Prop p;
            p.sprite.setTexture(_assets.GetTexture("Tree"));
            int typeIdx = std::uniform_int_distribution<>(0, (int)treeTypes.size() - 1)(_gen);
            p.sprite.setTextureRect(treeTypes[typeIdx]);

            float offsetX = (float)(_gen() % 100);
            float offsetY = (float)(_gen() % 40);
            p.x = x + offsetX;
            p.y = y + offsetY;
            p.sprite.setPosition(p.x, p.y);

            sf::FloatRect b = p.sprite.getLocalBounds();
            p.sprite.setOrigin(b.width / 2.f, b.height);
            float depthScale = 1.0f + ((y - 920.f) / 230.f) * 0.5f;
            p.sprite.setScale(depthScale, depthScale);
            _forest.push_back(p);
        }
    }
    std::sort(_forest.begin(), _forest.end(), [](const Prop& a, const Prop& b) {
        return a.y < b.y;
        });
}

void Game::update(float dt) {
    if (_currentState == GameState::PLAYING || _currentState == GameState::MENU) {
        _currentWaterOffset += 80.f * dt;
    }
    const float fixedDt = 1.0f / 60.0f;
    while (_accumulator >= fixedDt) {
        _camera.setCenter(_player->position.x + 600.f, 540.f);
        float vL = _camera.getCenter().x - 1200.f;
        float vR = _camera.getCenter().x + 1200.f;

        if (_player->position.y < 205.f) {
            _player->position.y += 3.5f;
            _player->velocity.y = std::max(0.f, _player->velocity.y + 120.f) * 0.5f;
        }
        if (_player->position.y > 875.f) {
            _player->position.y -= 3.5f;
            _player->velocity.y = std::min(0.f, _player->velocity.y - 120.f) * 0.5f;
        }

        for (size_t i = 0; i < _traffic.size(); ++i) {
            float targetSpeed = 450.f;
            bool brake = false, gas = false;
            float currentAbs = std::abs(_traffic[i].velocity.x);

            for (size_t j = 0; j < _traffic.size(); ++j) {
                if (i == j) continue;
                if (std::abs(_traffic[i].position.y - _traffic[j].position.y) < 25.f) {
                    float dx = _traffic[j].position.x - _traffic[i].position.x;
                    bool isAhead = (_traffic[i].velocity.x * dx > 0);
                    float dist = std::abs(dx);

                    if (isAhead && dist < 400.f) {
                        bool overtaked = false;
                        if (_traffic[i].canOvertake && !_traffic[i].isChangingLane && _traffic[i].laneChangeCooldown <= 0.f) {
                            int side = (_traffic[i].currentLane <= 2) ? 0 : 1;
                            int minL = (side == 0) ? 0 : 3;
                            int maxL = (side == 0) ? 2 : 5;
                            bool upFree = (_traffic[i].currentLane > minL);
                            bool downFree = (_traffic[i].currentLane < maxL);

                            if (_traffic[i].currentLane == 2) downFree = false;
                            if (_traffic[i].currentLane == 3) upFree = false;

                            float safeZoneNPC = 310.f;
                            float safeZonePlayer = 370.f;

                            for (size_t k = 0; k < _traffic.size(); ++k) {
                                if (i == k) continue;
                                if (upFree && _traffic[k].currentLane == _traffic[i].currentLane - 1) {
                                    if (std::abs(_traffic[k].position.x - _traffic[i].position.x) < safeZoneNPC) upFree = false;
                                }
                                if (downFree && _traffic[k].currentLane == _traffic[i].currentLane + 1) {
                                    if (std::abs(_traffic[k].position.x - _traffic[i].position.x) < safeZoneNPC) downFree = false;
                                }
                            }

                            float pDist = _player->position.x - _traffic[i].position.x;
                            if (upFree && std::abs(_player->position.y - Config::LanesY[_traffic[i].currentLane - 1]) < 40.f) {
                                if (std::abs(pDist) < safeZonePlayer) upFree = false;
                            }
                            if (downFree && std::abs(_player->position.y - Config::LanesY[_traffic[i].currentLane + 1]) < 40.f) {
                                if (std::abs(pDist) < safeZonePlayer) downFree = false;
                            }

                            if (upFree) {
                                _traffic[i].targetY = Config::LanesY[_traffic[i].currentLane - 1];
                                _traffic[i].currentLane--;
                                _traffic[i].isChangingLane = true;
                                _traffic[i].laneChangeCooldown = 2.0f;
                                overtaked = true;
                            }
                            else if (downFree) {
                                _traffic[i].targetY = Config::LanesY[_traffic[i].currentLane + 1];
                                _traffic[i].currentLane++;
                                _traffic[i].isChangingLane = true;
                                _traffic[i].laneChangeCooldown = 2.0f;
                                overtaked = true;
                            }
                        }

                        if (!overtaked) {
                            if (dist < 180.f) {
                                _traffic[i].position.x = _traffic[j].position.x - (dx > 0 ? 180.f : -180.f);
                                _traffic[i].velocity.x = _traffic[j].velocity.x;
                                brake = true;
                            }
                            else {
                                targetSpeed = std::abs(_traffic[j].velocity.x) * 0.95f;
                                if (currentAbs > targetSpeed) brake = true;
                            }
                        }
                    }
                }
            }

            if (_traffic[i].isChangingLane) {
                float yDiff = _traffic[i].targetY - _traffic[i].position.y;
                if (std::abs(yDiff) > 2.0f) {
                    _traffic[i].velocity.y = (yDiff > 0 ? 195.f : -195.f);
                }
                else {
                    _traffic[i].position.y = _traffic[i].targetY;
                    _traffic[i].velocity.y = 0.f;
                    _traffic[i].isChangingLane = false;
                }
            }
            else {
                _traffic[i].velocity.y = 0.f;
            }

            if (std::abs(_traffic[i].position.y - _player->position.y) < 25.f) {
                float dx = _player->position.x - _traffic[i].position.x;
                if (_traffic[i].velocity.x * dx > 0) {
                    float radar = (_traffic[i].velocity.x > 0) ? 350.f : 500.f;
                    if (std::abs(dx) < radar) {
                        if (_traffic[i].velocity.x > 0) {
                            if (currentAbs < std::abs(_player->velocity.x)) gas = true; else brake = true;
                        }
                        else brake = true;
                    }
                }
            }

            if (brake) _traffic[i].velocity.x *= 0.95f;
            else if (gas || currentAbs < 450.f) {
                float acc = (_traffic[i].velocity.x >= 0) ? 220.f : -220.f;
                _traffic[i].velocity.x += acc * fixedDt;
            }
            _traffic[i].isBraking = brake;
            _traffic[i].update(fixedDt);

            if (_traffic[i].currentLane == 2 && _traffic[i].position.y > Config::LanesY[2] + 2.f) {
                _traffic[i].position.y = Config::LanesY[2]; _traffic[i].velocity.y = 0.f; _traffic[i].isChangingLane = false;
            }
            if (_traffic[i].currentLane == 3 && _traffic[i].position.y < Config::LanesY[3] - 2.f) {
                _traffic[i].position.y = Config::LanesY[3]; _traffic[i].velocity.y = 0.f; _traffic[i].isChangingLane = false;
            }

            if (_traffic[i].position.x < vL - 1500.f || _traffic[i].position.x > vR + 3500.f) {
                bool movingRight = (_traffic[i].velocity.x > 0);
                float randomOffset = (float)(_gen() % 2000);
                float newX = vR+1000.f+randomOffset;
                int newLane = movingRight ? std::uniform_int_distribution<>(3, 5)(_gen) : std::uniform_int_distribution<>(0, 2)(_gen);
                bool blocked = false;
                for (size_t k = 0; k < _traffic.size(); ++k) {
                    if (i == k) continue;
                    if (std::abs(_traffic[k].position.y - Config::LanesY[newLane]) < 25.f &&
                        std::abs(_traffic[k].position.x - newX) < 600.f) {
                        blocked = true; break;
                    }
                }

                if (!blocked) {
                    _traffic[i].position.x = newX;
                    _traffic[i].position.y = Config::LanesY[newLane];
                    _traffic[i].currentLane = newLane;
                    _traffic[i].targetY = Config::LanesY[newLane];
                    _traffic[i].isChangingLane = false;
                    _traffic[i].previousPosition = _traffic[i].position;
                }
            }
        }

        _player->update(fixedDt);
        if (_currentState == GameState::PLAYING) {
            float multiplier = 1.0f;
            if (_player->position.y < 545.f) multiplier *= 2.0f;

            bool isOvertaking = false;
            for (const auto& npc : _traffic) {
                float dx = std::abs(_player->position.x - npc.position.x);
                float dy = std::abs(_player->position.y - npc.position.y);
                if (dx < 220.f && dy < 90.f && std::abs(_player->velocity.x) > 400.f) {
                    isOvertaking = true; break;
                }
            }
            if (isOvertaking) multiplier *= 1.7f;
            _score += ((std::abs(_player->velocity.x) * fixedDt) / 100.f) * multiplier;
            if (_score > _highScore) {
                _highScore = _score;
                _isNewRecord = true;
            }

            for (auto& npc : _traffic) {
                sf::FloatRect pB = _player->sprite.getGlobalBounds();
                sf::FloatRect nB = npc.sprite.getGlobalBounds();
                sf::FloatRect overlap;
                if (pB.intersects(nB, overlap)) {
                    if (std::abs(_player->velocity.x) >= 800.f * 0.85f) {
                        _currentState = GameState::GAMEOVER;
                        _engineSound.stop();
                        _crashSound.play();
                        if (_isNewRecord) {
                            std::ofstream outputFile("highscore.txt");
                            if (outputFile.is_open()) {
                                outputFile << (int)_highScore;
                                outputFile.close();
                            }
                        }
                        break;
                    }
                    else {
                        if (overlap.width < overlap.height) {
                            _player->position.y += (_player->position.y < npc.position.y) ? -1.1f : 1.1f;
                            _player->velocity.y *= 0.75f; _player->velocity.x *= 0.995f;
                        }
                        else {
                            _player->position.x = npc.position.x + (_player->position.x < npc.position.x ? -105.f : 105.f);
                            _player->velocity.x = npc.velocity.x * (_player->position.x < npc.position.x ? 0.7f : 1.1f);
                        }
                    }
                }
            }
        }
        float forestChunk = 20000.f;
        for (auto& p : _forest) {
            if (p.x < _camera.getCenter().x - 3000.f) {
                p.x += forestChunk; p.sprite.setPosition(p.x, p.y);
            }
            else if (p.x > _camera.getCenter().x + 17000.f) {
                p.x -= forestChunk; p.sprite.setPosition(p.x, p.y);
            }
        }

        _accumulator -= fixedDt;
    }

    if (_currentState == GameState::PLAYING) {
        float speedRatio = std::abs(_player->velocity.x) / 800.f;
        _engineSound.setPitch(1.1f + (speedRatio * 1.2f));
    }
}

void Game::render() {
    float alpha = _accumulator / (1.0f / 60.0f);
    _window.clear(sf::Color(30, 30, 30));

    Vec2 pPos = (_currentState == GameState::PLAYING) ?
        Lerp(_player->previousPosition, _player->position, alpha) : _player->position;

    _camera.setCenter(pPos.x + 600.f, 540.f);
    _window.setView(_camera);

    float vL = _camera.getCenter().x - 960.f;
    float vR = _camera.getCenter().x + 960.f;

    sf::Texture& waterTex = _assets.GetTexture("Water");
    waterTex.setRepeated(true);

    sf::Sprite waterSp;
    waterSp.setTexture(waterTex);

    int texX = (int)(_currentWaterOffset + vL);
    int texY = (int)(_currentWaterOffset * 0.2f);
    waterSp.setTextureRect(sf::IntRect(texX, texY, (int)(vR - vL), 150));
    waterSp.setPosition(vL, 0.f);
    _window.draw(waterSp);

    sf::RectangleShape rect(sf::Vector2f(vR - vL, 150.f));
    rect.setPosition(vL, 930.f); rect.setFillColor(sf::Color(97, 59, 5)); _window.draw(rect);
    rect.setSize(sf::Vector2f(vR - vL, 720.f)); rect.setPosition(vL, 180.f); rect.setFillColor(sf::Color(45, 45, 45)); _window.draw(rect);
    rect.setSize(sf::Vector2f(vR - vL, 30.f)); rect.setFillColor(sf::Color(130, 130, 130)); rect.setPosition(vL, 150.f); _window.draw(rect);
    rect.setPosition(vL, 900.f); _window.draw(rect);
    rect.setSize(sf::Vector2f(vR - vL, 10.f)); rect.setPosition(vL, 540.f); rect.setFillColor(sf::Color(180, 150, 20)); _window.draw(rect);

    sf::RectangleShape line(sf::Vector2f(50.f, 5.f)); line.setFillColor(sf::Color(200, 200, 200, 150));
    for (int i = (int)(vL / 150.f) - 1; i < (int)(vR / 150.f) + 1; ++i) {
        for (float ly : {300.f, 420.f, 670.f, 790.f}) {
            line.setPosition(i * 150.f, ly);
            _window.draw(line);
        }
    }

    for (const auto& p : _forest) {
        if (p.x > vL - 200.f && p.x < vR + 200.f) _window.draw(p.sprite);
    }

    if (_currentState != GameState::MENU) {
        for (auto& npc : _traffic) {
            Vec2 rPos = (_currentState == GameState::PLAYING) ? Lerp(npc.previousPosition, npc.position, alpha) : npc.position;
            npc.sprite.setPosition(rPos.x, rPos.y);

            if (npc.currentLane <= 2) npc.sprite.setRotation(270.f);
            else npc.sprite.setRotation(90.f);

            npc.draw(_window);
        }

        _player->sprite.setPosition(pPos.x, pPos.y);
        _player->draw(_window, _isNight);

        if (_isNight) {
            sf::RectangleShape nightOverlay(sf::Vector2f(1920.f, 1080.f));
            nightOverlay.setOrigin(960.f, 540.f);
            nightOverlay.setPosition(_camera.getCenter());
            nightOverlay.setFillColor(sf::Color(0, 0, 10, 160));
            _window.draw(nightOverlay);
        }

        sf::Vector2f camCenter = _camera.getCenter();
        _scoreText.setString("SCORE: " + std::to_string((int)_score));
        _scoreText.setPosition(camCenter.x - 900.f, 20.f);
        _window.draw(_scoreText);

        _highScoreText.setString(_isNewRecord ? "NEW RECORD!" : "HIGH SCORE: " + std::to_string((int)_highScore));
        _highScoreText.setFillColor(_isNewRecord ? sf::Color::Red : sf::Color::Yellow);
        float blink = (std::sin(_menuTimer * 10.f) + 1.f) / 2.f * 255.f;
        if (_isNewRecord) _highScoreText.setFillColor(sf::Color(255, 50, 50, (sf::Uint8)blink));
        _highScoreText.setPosition(camCenter.x + 650.f, 20.f);
        _window.draw(_highScoreText);
    }
    sf::Vector2f center = _camera.getCenter();
    if (_currentState == GameState::MENU) {
        sf::RectangleShape overlay(sf::Vector2f(1920.f, 1080.f)); overlay.setOrigin(960.f, 540.f);
        overlay.setPosition(center); overlay.setFillColor(sf::Color(0, 0, 0, 200)); _window.draw(overlay);

        _titleText.setPosition(center.x, center.y - 80.f); _window.draw(_titleText);

        float blinkAlpha = (std::sin(_menuTimer * 5.f) + 1.f) / 2.f * 255.f;
        _startText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(blinkAlpha)));
        _startText.setPosition(center.x, center.y + 120.f); _window.draw(_startText);
        _modeText.setPosition(center.x, center.y + 170.f); _window.draw(_modeText);
    }
    else if (_currentState == GameState::GAMEOVER) {
        sf::RectangleShape overlay(sf::Vector2f(1920.f, 1080.f)); overlay.setOrigin(960.f, 540.f);
        overlay.setPosition(center); overlay.setFillColor(sf::Color(0, 0, 0, 150)); _window.draw(overlay);

        _gameOverText.setPosition(center.x, center.y - 50.f); _window.draw(_gameOverText);
        float blinkAlpha = (std::sin(_menuTimer * 5.f) + 1.f) / 2.f * 255.f;
        _restartText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(blinkAlpha)));
        _restartText.setPosition(center.x, center.y + 70.f); _window.draw(_restartText);
    }
    else if (_currentState == GameState::PAUSED) {
        sf::RectangleShape overlay(sf::Vector2f(1920.f, 1080.f)); overlay.setOrigin(960.f, 540.f);
        overlay.setPosition(center); overlay.setFillColor(sf::Color(0, 0, 0, 100)); _window.draw(overlay);

        _pauseText.setPosition(center.x, center.y - 20.f); _window.draw(_pauseText);
        float blinkAlpha = (std::sin(_menuTimer * 5.f) + 1.f) / 2.f * 255.f;
        _pauseSubText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(blinkAlpha)));
        _pauseSubText.setPosition(center.x, center.y + 100.f); _window.draw(_pauseSubText);
        _returnMenuText.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(blinkAlpha)));
        _returnMenuText.setPosition(center.x, center.y + 150.f); _window.draw(_returnMenuText);
    }

    _window.display();
}