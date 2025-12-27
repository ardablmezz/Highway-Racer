#include "../include/Player.h"
#include <cmath>

void Player::handleInput() {
    acceleration = { 0.f, 0.f };

    const float ACCEL_GAS = 600.f;
    const float ACCEL_BRAKE = 600.f;
    const float ACCEL_STEER = 500.f;
    float speedFactor = std::abs(velocity.x) / maxSpeed;
    float maneuverability = 1.0f - (speedFactor * 0.32f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) acceleration.y = -ACCEL_STEER * maneuverability;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) acceleration.y = ACCEL_STEER * maneuverability;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) acceleration.x = ACCEL_GAS;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) acceleration.x = -ACCEL_BRAKE;
    isBraking = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
}

void Player::update(float dt) {
    handleInput();
    Car::update(dt);
    sprite.setRotation(90.f);
}
void Player::draw(sf::RenderWindow& window, bool isNight) {
    window.draw(sprite);  
    drawLights(window, isNight);
}
void Player::drawLights(sf::RenderWindow& window, bool isNight) {
    sf::Vector2f pos = sprite.getPosition();
    if (isNight) {
        sf::Color lightColorCenter = sf::Color(255, 255, 220, 120);
        sf::Color lightColorEdge = sf::Color(255, 255, 220, 0);
        sf::VertexArray rightHeadlight(sf::TrianglesFan, 4);

        rightHeadlight[0].position = sf::Vector2f(pos.x + 60.f, pos.y - 20.f);  rightHeadlight[0].color = lightColorCenter;
        rightHeadlight[1].position = sf::Vector2f(pos.x + 550.f, pos.y - 150.f); rightHeadlight[1].color = lightColorEdge;
        rightHeadlight[2].position = sf::Vector2f(pos.x + 600.f, pos.y - 20.f);  rightHeadlight[2].color = lightColorEdge;
        rightHeadlight[3].position = sf::Vector2f(pos.x + 550.f, pos.y + 50.f);  rightHeadlight[3].color = lightColorEdge;
        window.draw(rightHeadlight);

        sf::VertexArray leftHeadlight(sf::TrianglesFan, 4);

        leftHeadlight[0].position = sf::Vector2f(pos.x + 60.f, pos.y + 20.f);   leftHeadlight[0].color = lightColorCenter;
        leftHeadlight[1].position = sf::Vector2f(pos.x + 550.f, pos.y - 50.f);   leftHeadlight[1].color = lightColorEdge;
        leftHeadlight[2].position = sf::Vector2f(pos.x + 600.f, pos.y + 20.f);   leftHeadlight[2].color = lightColorEdge;
        leftHeadlight[3].position = sf::Vector2f(pos.x + 550.f, pos.y + 150.f);  leftHeadlight[3].color = lightColorEdge;
        window.draw(leftHeadlight);
    }

    sf::RectangleShape stopLight(sf::Vector2f(6.f, 10.f));
    sf::Color stopColor = sf::Color(255, 0, 0);
    if (isNight) {
        stopColor.a = isBraking ? 255 : 100;
    }
    else {
        stopColor.a = isBraking ? 255 : 0;
    }
    stopLight.setFillColor(stopColor);
    stopLight.setPosition(pos.x - 55.f, pos.y - 20.f); window.draw(stopLight);
    stopLight.setPosition(pos.x - 55.f, pos.y + 10.f); window.draw(stopLight);
}