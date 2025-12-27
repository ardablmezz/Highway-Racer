#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include "AssetManager.h"
#include "Player.h"
#include "Constants.hpp"

struct Prop {
    sf::Sprite sprite;
    float x, y;
};

class Game {
public:
    Game();
    ~Game(); 
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void resetGame(bool fullReset);
    void setupText(sf::Text& text, const std::string& str, int size, sf::Color color, bool bold = false);

    sf::RenderWindow _window;
    sf::View _camera;
    AssetManager _assets;
    std::mt19937 _gen;
    std::random_device _rd;

    Player* _player;
    std::vector<Car> _traffic;
    std::vector<Prop> _forest;

    sf::Sound _engineSound;
    sf::Sound _menuSound;
    sf::Sound _crashSound;

    sf::Text _gameOverText;
    sf::Text _restartText;
    sf::Text _titleText;
    sf::Text _startText;
    sf::Text _pauseText;
    sf::Text _pauseSubText;
    sf::Text _returnMenuText;
    sf::Text _scoreText;
    sf::Text _highScoreText;
    sf::Text _modeText;

    GameState _currentState;
    bool _isNight;
    bool _isNewRecord;
    bool _isFullscreen;

    float _score;
    float _highScore;
    float _menuTimer;
    float _accumulator;
    float _currentWaterOffset;
};

#endif
