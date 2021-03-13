#pragma once
#include <SFML/Graphics.hpp>
#include "gamestate.hpp"

template<typename T>
T lerp(const T& a, const T& b, float t)
{
	return (1.0f - t) * a + t * b;
}

struct GameDrawer {
    sf::CircleShape shape;
	sf::Sprite dirt;
	sf::Texture dirt_tex;
	sf::Color bg_color;

	void fill_dirt(sf::RenderWindow& window)
	{
		bg_color = sf::Color(0xfa, 0xba, 0xba, 0xfa);

		auto win_sz = window.getSize();
		dirt_tex.create(win_sz.x, win_sz.y);

		unsigned pixels_dimensions = dirt_tex.getSize().x * 
			dirt_tex.getSize().y;
		unsigned pixels_size = pixels_dimensions * sizeof(unsigned);
		unsigned *pixels = (unsigned *)malloc(pixels_size);

		const int n_metaballs = 12;
		vec2f metaballs_positions[n_metaballs];
		unsigned metaballs_sizes[n_metaballs];

		for (int i = 0; i < n_metaballs; ++i)
		{
			metaballs_positions[i].x = rand() % win_sz.x;
			metaballs_positions[i].y = rand() % win_sz.y;
			
			metaballs_sizes[i] = 200 + rand() % 300;
		}

		for (unsigned i = 0; i < pixels_dimensions; ++i)
		{
			unsigned x_pixel = i % win_sz.x;
			unsigned y_pixel = i / win_sz.y;
			sf::Color color = bg_color;

			float total_factor = 0;
			for (unsigned j = 0; j < n_metaballs; ++j)
			{
				vec2f pixel_to_metaball_center = vec2f((float)x_pixel, (float)y_pixel) - metaballs_positions[j];
				float length_p_meta = pixel_to_metaball_center.x * pixel_to_metaball_center.x + 
					pixel_to_metaball_center.y * pixel_to_metaball_center.y;
				length_p_meta = sqrt(length_p_meta);

				if (length_p_meta < (float)metaballs_sizes[j])
				{
					float factor = length_p_meta / (float)metaballs_sizes[j];
					total_factor += factor;
				}
			}
			
			unsigned char alpha = 255 - lerp(0, 255, total_factor);
			color = sf::Color(bg_color.r, bg_color.g, bg_color.b, alpha);

			pixels[i] = color.toInteger();
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
