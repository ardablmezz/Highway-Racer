#include "../include/Car.h"
#include <cmath>

Car::Car(const Vec2& pos, const Vec2& vel, sf::Texture& tex, const sf::IntRect& imgRect, float maxSpeed_, float damping_, int startLane)
{
    position = pos;
    previousPosition = pos;
    velocity = vel;
    acceleration = { 0.f, 0.f };
    maxSpeed = maxSpeed_;
    damping = damping_;
    laneChangeCooldown = 0.f;
    currentLane = startLane;
    targetY = pos.y;

    isChangingLane = false;
    isBraking = false;
    canOvertake = false;

    sprite.setTexture(tex);
    sprite.setTextureRect(imgRect);

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setScale(1.1f, 1.1f);
}

void Car::update(float dt) {
    previousPosition = position;
    if (laneChangeCooldown > 0.f) laneChangeCooldown -= dt;

    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;

    if (acceleration.x == 0.f) velocity.x *= damping;
    if (acceleration.y == 0.f) velocity.y *= damping;

    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed && speed > 0.f) {
        velocity.x = (velocity.x / speed) * maxSpeed;
        velocity.y = (velocity.y / speed) * maxSpeed;
    }

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
    sprite.setPosition(position.x, position.y);
}

void Car::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}