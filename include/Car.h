#ifndef CAR_H
#define CAR_H

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Car {
public:
    Car(const Vec2& pos, const Vec2& vel, sf::Texture& tex, const sf::IntRect& imgRect, float maxSpeed, float damping, int startLane);
    virtual ~Car() {}
    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);

    sf::Sprite sprite;
    Vec2 position;
    Vec2 previousPosition;
    Vec2 velocity;
    Vec2 acceleration;

    float maxSpeed;
    float damping;
    float laneChangeCooldown;
    float targetY;
    int currentLane;
    bool isChangingLane;
    bool isBraking;
    bool canOvertake;
};

#endif

