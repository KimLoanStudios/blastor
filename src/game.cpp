#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>

using namespace sf;

int run_game(int argc __attribute__((unused)), const char* argv[] __attribute__((unused))) {
    std::cout << "Running game!\n";

	unsigned window_width = 200;
	unsigned window_height = 200;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

	//TODO(Stanisz): This should be taken from the game state
	Vector2 position(100.0f, 100.0f);
	Clock delta_clock;
	float dt = 0.0f;

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

		sf::Vector2f moving_dir(0.0f, 0.0f);
		if (pressed_keys[sf::Keyboard::W])
		{
			moving_dir += Vector2f(0.0f, -1.0f);
		}
		if (pressed_keys[sf::Keyboard::D])
		{
			moving_dir += Vector2f(1.0f, 0.0f);
		}
		if (pressed_keys[sf::Keyboard::A])
		{
			moving_dir += Vector2f(-1.0f, 0.0f);
		}
		if (pressed_keys[sf::Keyboard::S])
		{
			moving_dir += Vector2f(0.0f, 1.0f);
		}
		
		float moving_dir_len = (float)moving_dir.x * moving_dir.x + moving_dir.y * moving_dir.y;
		moving_dir_len = sqrt(moving_dir_len);

		if (moving_dir_len > 0.0f)
		{
			moving_dir /= moving_dir_len;
		}

		float speed = 0.5f;
		position += speed * dt * moving_dir;

		
		shape.setPosition(position.x, position.y);		

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
