#pragma once
#include <SFML/Graphics.hpp>
#include "gamestate.hpp"

struct GameDrawer {
    sf::CircleShape shape;
	sf::Sprite dirt;
	sf::Texture dirt_tex;

	void fill_dirt(sf::RenderWindow& window)
	{
		auto win_sz = window.getSize();

		dirt_tex.create(win_sz.x, win_sz.y);

		unsigned pixels_dimensions = dirt_tex.getSize().x * 
			dirt_tex.getSize().y;
		unsigned pixels_size = pixels_dimensions * sizeof(unsigned);
		unsigned *pixels = (unsigned *)malloc(pixels_size);
		for (unsigned i = 0; i < pixels_dimensions; ++i)
		{
			pixels[i] = 0xabafabaf;
		}

		dirt_tex.update((unsigned char*)pixels);
		dirt.setTexture(dirt_tex);

		dirt.setPosition(0, 0);

		free(pixels);
	}

    void draw(GameState& game_state, sf::RenderWindow& window) {
        window.clear();

		//NOTE(Stanisz): background
		window.draw(dirt);
			

        for(auto&& [player_id, player] : game_state.players) {
			shape.setFillColor(sf::Color::Green);

			//NOTE(Stanisz): head
            shape.setPosition(player.pos);
			shape.setRadius(20.0f);
            window.draw(shape);
			
			//NOTE(Stanisz): arms
			//TODO: arms should be oriented around the facing direction
            shape.setPosition(player.pos + vec2f(25.0f, 0.0f));
			shape.setRadius(10.0f);
            window.draw(shape);
            shape.setPosition(player.pos - vec2f(5.0f, 0.0f));
			shape.setRadius(10.0f);
            window.draw(shape);
        }

        window.display();
    }
};
