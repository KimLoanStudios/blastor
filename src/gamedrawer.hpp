#pragma once
#include <SFML/Graphics.hpp>
#include "gamestate.hpp"

struct GameDrawer {
    sf::CircleShape shape;

    GameDrawer(): shape(100.f) {
        shape.setFillColor(sf::Color::Green);

    }

    void draw(GameState& game_state, sf::RenderWindow& window) {
        window.clear();
        for(auto&& [player_id, player] : game_state.players) {
            shape.setPosition(player.pos);
            window.draw(shape);
        }

        window.display();
    }
};