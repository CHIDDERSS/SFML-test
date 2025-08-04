#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML TEST");
    sf::CircleShape shape(200);
    
    while (window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
            window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}