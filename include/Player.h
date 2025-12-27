#ifndef PLAYER_H
#define PLAYER_H
#include "Car.h"

class Player : public Car {
public:
    using Car::Car;
    void update(float dt) override;
    void draw(sf::RenderWindow& window, bool isNight);
private:
    void handleInput();
    void drawLights(sf::RenderWindow& window, bool isNight);
};
#endif