#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <vector>

#include "gamestate.hpp"
#include "gamedrawer.hpp"
#include "event.hpp"
#include "config.hpp"

void try_receiveing_events(std::vector<Event>& cur_events);
void send_our_events(std::vector<Event>& our_events);
std::vector<Event> handle_input(GameState& game_state, sf::RenderWindow& window);

int run_game(int argc __attribute__((unused)), const char* argv[] __attribute__((unused))) {
    std::cout << "Running game!\n";

	unsigned window_width = 200;
	unsigned window_height = 200;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");

    GameState game_state;
    GameDrawer game_drawer;
	game_drawer.fill_dirt(window);

    std::vector<Event> events;

	sf::Clock delta_clock;
	float dt = 0.0f;

    sf::Clock tick_clock;

	std::map<sf::Keyboard::Key, bool> pressed_keys;

    while (window.isOpen())
    {
		dt = delta_clock.restart().asMilliseconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized)
			{
				auto sz = window.getSize();
				window_width = sz.x;
				window_height = sz.y;
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				pressed_keys[event.key.code] = 1;
			}
			if (event.type == sf::Event::KeyReleased)
			{
				pressed_keys[event.key.code] = 0;
			}
        }

        try_receiveing_events(events);

        if(tick_clock.getElapsedTime().asSeconds() >= TICK_TIME) {
            tick_clock.restart();

            game_state.apply_events(events);
            events.clear();

            std::vector<Event> our_events = handle_input(game_state, window);

            game_state.apply_events(our_events);

            send_our_events(our_events);
        }

        game_drawer.draw(game_state, window);
    }

    return 0;
}

void try_receiveing_events(std::vector<Event>& cur_events) {
    // TODO
}

void send_our_events(std::vector<Event>& our_events) {
    // TODO
}

std::vector<Event> handle_input(GameState& game_state, sf::RenderWindow& window) {
    // vec2f mouse_pos = vec2f(sf::Mouse::getPosition(window));

    vec2f moving_dir(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moving_dir += vec2f(0.0f, -1.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moving_dir += vec2f(1.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moving_dir += vec2f(-1.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moving_dir += vec2f(0.0f, 1.0f);
		
    float moving_dir_len = (float)moving_dir.x * moving_dir.x + moving_dir.y * moving_dir.y;
    moving_dir_len = sqrt(moving_dir_len);

    if (moving_dir_len > 0.0f)
    {
        moving_dir /= moving_dir_len;
    }   

    f32 sped = 1000.0;

    u64 my_id = game_state.my_player_id;
    vec2f my_new_pos = game_state.players[my_id].pos + float(TICK_TIME) * sped *  moving_dir;

    std::vector<Event> my_events;

    my_events.push_back(Event {
        .tick = 3,
        .content = PlayerPos {
            .player_id = my_id,
            .pos = my_new_pos
        }
    });

    return my_events;
}
