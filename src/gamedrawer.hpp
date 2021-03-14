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
	sf::RectangleShape rect;
	sf::Sprite dirt;
	sf::Texture dirt_tex;
	sf::Texture box_tex;
	sf::Color bg_color;
	sf::Font font;

	void fill_dirt(sf::RenderWindow& window)
	{
		bg_color = sf::Color(0xfa, 0xba, 0xba, 0xfa);
		box_tex.loadFromFile("assets/box.jpg", sf::Rect(0, 0, 400, 400));

		auto win_sz = window.getSize();
		dirt_tex.create(1024, 1024);

		unsigned pixels_dimensions = dirt_tex.getSize().x * 
			dirt_tex.getSize().y;
		unsigned pixels_size = pixels_dimensions * sizeof(unsigned);
		unsigned *pixels = (unsigned *)malloc(pixels_size);

		const int n_metaballs = 30;
		vec2f metaballs_positions[n_metaballs];
		unsigned metaballs_sizes[n_metaballs];
		sf::Color metaballs_colors[n_metaballs];

		for (int i = 0; i < n_metaballs; ++i)
		{
			metaballs_positions[i].x = rand() % dirt_tex.getSize().x;
			metaballs_positions[i].y = rand() % dirt_tex.getSize().y;
			
			metaballs_sizes[i] = 100 + rand() % 100;

			metaballs_colors[i].r = rand() % 100 + 5;
			metaballs_colors[i].g = rand() % 100 + 10;
			metaballs_colors[i].b = rand() % 100 + 5;
		}

		for (unsigned i = 0; i < pixels_dimensions; ++i)
		{
			unsigned x_pixel = i % dirt_tex.getSize().x;
			unsigned y_pixel = i / dirt_tex.getSize().y;
			sf::Color color = bg_color;
			float total_factor = 0;

			sf::Color total_color(0, 0, 0, 0);
			for (unsigned j = 0; j < n_metaballs; ++j)
			{
				vec2f pixel_to_metaball_center = vec2f((float)x_pixel, (float)y_pixel) - metaballs_positions[j];
				float length_p_meta = pixel_to_metaball_center.x * pixel_to_metaball_center.x + 
					pixel_to_metaball_center.y * pixel_to_metaball_center.y;
				length_p_meta = sqrt(length_p_meta);

				if (length_p_meta < (float)metaballs_sizes[j])
				{
					float factor = length_p_meta / ( 2.0f * metaballs_sizes[j] ) ;
					total_color.r += factor * factor * metaballs_colors[j].r;
					total_color.g += factor * factor * metaballs_colors[j].g;
					total_color.b += factor * factor * metaballs_colors[j].b;
					total_factor += factor;
				}
			}
			
			color = sf::Color(255 - total_color.r, 255 - total_color.g , total_color.b, 255 - total_factor);

			pixels[i] = color.toInteger();
		}

		dirt_tex.update((unsigned char*)pixels);
		dirt.setTexture(dirt_tex);
		font.loadFromFile("assets/cs.ttf");

		dirt.setPosition(0, 0);



		free(pixels);
	}

    void draw(GameState& game_state, sf::RenderWindow& window, u64 player_id) {
		vec2f player_pos = game_state.players[player_id].pos;
		//dirt.setPosition(player_pos.x - dirt_tex.getSize().x / 2, player_pos.y - dirt_tex.getSize().y / 2);
		create_view(window, player_pos);

        window.clear();

		//NOTE(Stanisz): background
		window.draw(dirt);
			
	
		if (game_state.players[player_id].dead)
		{
			sf::Text text("Very dead!\nPress R xd", font);
			text.setCharacterSize(120);
			text.setFillColor(sf::Color::Red);
			text.setPosition(10000, 10000);

			window.draw(text);
			game_state.players[player_id].pos = vec2f(10300.0f, 10200.0f);
		}
		else
		{
			for(auto&& [box_id, box] : game_state.boxes) {
				rect.setTexture(&box_tex);
				rect.setSize(box.size);
				rect.setPosition(box.pos);

				window.draw(rect);
			}

			for(auto&& [player_id, player] : game_state.players) {
				shape.setFillColor(sf::Color::Green);
				shape.setScale(1.0f, 1.0f);
				
				if (player.dead) continue;

				//NOTE(Stanisz): head
				shape.setPosition(player.pos);
				shape.setRadius(20.0f);
				shape.setOrigin(20.f, 20.f);
				window.draw(shape);
				
				//NOTE(Stanisz): arms
				//TODO: arms should be oriented around the facing direction

				auto ortho = vec2f(player.look_dir.y, -player.look_dir.x);
				shape.setPosition(player.pos + 15.0f * player.look_dir + 15.0f * ortho);
				shape.setRadius(10.0f);
				shape.setOrigin(10, 10);
				window.draw(shape);
				shape.setPosition(player.pos + 15.0f * player.look_dir - 15.0f * ortho);
				shape.setRadius(10.0f);
				shape.setOrigin(10, 10);
				window.draw(shape);


				sf::Text text(player.name.empty() ? "unknown" : player.name, font);
				text.setCharacterSize(30);
				text.setFillColor(sf::Color::Black);
				text.setPosition(player.pos + vec2f(-60.0f, -60.0f));

				window.draw(text);
			}



			for(auto&& [bullet_id, bullet] : game_state.bullets) {
				//shape.setScale(0.5f, 2.0f);
				shape.setRadius(7.5f);
				shape.setPosition(bullet.pos);
				shape.setFillColor(sf::Color(255, 0, 0, 255));
				window.draw(shape);
			}

		}

		std::string to_display = "Score:";
		to_display += std::to_string(game_state.players[player_id].score);
		sf::Text text(to_display, font);
		text.setCharacterSize(80);
		text.setFillColor(sf::Color::Green);
		text.setPosition(game_state.players[player_id].pos - vec2f(500, 500));

		window.draw(text);
        window.display();
    }

	void create_view(sf::RenderWindow& window, vec2f player_pos) {
		f32 view_width = 1200;
		f32 view_height = f32(window.getSize().y)/f32(window.getSize().x) * view_width;

		f32 view_x = player_pos.x - view_width/2;
		f32 view_y = player_pos.y - view_height/2;

		sf::View view(sf::FloatRect(view_x, view_y, view_width, view_height));
		window.setView(view);
	}
};
