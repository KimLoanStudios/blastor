#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

int run_game(int argc __attribute__((unused)), const char* argv[] __attribute__((unused))) {
    std::cout << "Running game!\n";

	unsigned window_width = 200;
	unsigned window_height = 200;

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
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
			}
        }

		

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
