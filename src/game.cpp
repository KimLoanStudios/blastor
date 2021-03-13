#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

int run_game(int argc __attribute__((unused)), const char* argv[] __attribute__((unused))) {
    std::cout << "Running game!\n";

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
