#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML TEST");
    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
    
    sf::Text text;
    text.setPosition(100, 100);

    sf::CircleShape shape(200);

    std::string userInput;

    while (window.isOpen()) {
      sf::Event event;
      while(window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !userInput.empty()) {
                userInput.pop_back();
            } else if (event.text.unicode < 120 && event.text.unicode != '\r') {
                userInput += static_cast<char>(event.text.unicode);
            }
            text.setString(userInput);
        }
      }

      window.clear();
      window.draw(text);
      window.draw(shape);
      window.display();
    }

    return 0;
}