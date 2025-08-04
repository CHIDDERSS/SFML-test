#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Game {
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game();
    void run();
private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow _window;
    sf::CircleShape _player;
};

int main(int argc, char* argv[]) {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML TEST");
    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
    
    sf::CircleShape circle(150);
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition(10, 20);

    sf::Text text;
    text.setPosition(100, 100);

    sf::CircleShape shape(150);

    std::string userInput;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if ((event.type == sf::Event::Closed) || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        window.clear();
        window.draw(circle);
        window.display();
    }

    return 0;
}