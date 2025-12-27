#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <SFML/Graphics.hpp>

struct Vec2 {
    float x, y;
};
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};
namespace Config {
    const int ScreenWidth = 1920;
    const int ScreenHeight = 1080;
    const float LanesY[6] = { 240.f, 360.f, 480.f, 610.f, 730.f, 845.f };
}
#endif