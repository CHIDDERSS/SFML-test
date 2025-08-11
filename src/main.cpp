#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <list>
#include <functional>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int frames = 30;

// -------------------------------------------------- ACTION CLASSES --------------------------------------------------
class Action {
public:
    enum Type {
        RealTime = 1,
        Pressed = 1 << 1,
        Released = 1 << 2
    };
    Action (const sf::Keyboard::Key& key, int type = Type::RealTime|Type::Pressed);
    Action (const sf::Mouse::Button& button, int type = Type::RealTime|Type::Pressed);

    Action(const Action& other); // <-- Add this line
    Action& operator=(const Action& other); // <-- Add this line

    bool test() const;
    bool operator==(const sf::Event& event) const;
    bool operator==(const Action& other) const;

private:
    friend class ActionTarget;
    sf::Event _event;
    int _type;
};

class ActionTarget {
public:
    using FuncType = std::function<void(const sf::Event&)>;
    ActionTarget();

    bool processEvent(const sf::Event& event) const;
    void processEvents();

    void bind(const Action& action, const FuncType& callback);
    void unbind(const Action& action);

private:
    std::list<std::pair<Action, FuncType>> _eventRealTime;
    std::list<std::pair<Action, FuncType>> _eventPoll;
};

// -------------------------------------------------- PLAYER CLASS --------------------------------------------------
class Player  : public sf::Drawable, public ActionTarget {
public:
    Player(const Player&) = delete;
    Player& operator = (const Player&) = delete;
    Player();

    template<typename ... Args>
    void setPosition(Args&& ... args) {
        _shape.setPosition(std::forward<Args>(args)...);
    }

    void update(sf::Time deltaTime);
    bool _isMoving;
    int _rotation;

public:
    void processEvents() {
        ActionTarget::processEvents();
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    sf::RectangleShape _shape;
    sf::Vector2f _velocity;
};

// -------------------------------------------------- GAME CLASS --------------------------------------------------
class Game {
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game();
    void run(int frames_per_seconds);
private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    sf::RenderWindow _window;
    Player _player;
};

// -------------------------------------------------- GAME CONSTRUCTOR --------------------------------------------------
// constructor
Game::Game() : _window(sf::VideoMode(800,600), "GAME"), _player() {
    _player.setPosition(150,150);
}

// -------------------------------------------------- GAME METHODS -------------------------------------------------- 
void Game::processEvents() {
    sf::Event event;    
    
    while(_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
       } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                _window.close();
            }
       } 
    }
    _player.processEvents(); 
}

void Game::update(sf::Time deltaTime) {
    _player.update(deltaTime);
};

void Game::render() {
    _window.clear();
    _window.draw(_player);
    _window.display();
};

void Game::run(int frames_per_second) {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time TimePerFrame = sf::seconds(1.0f/frames_per_second);

    while(_window.isOpen()) {
        processEvents();

        bool repaint = false;

        timeSinceLastUpdate += clock.restart();
        while(timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            repaint = true;
            update(TimePerFrame);
        }
        update(timeSinceLastUpdate);
        if(repaint) {
            render();
        }
    }
}

// -------------------------------------------------- PLAYER METHODS --------------------------------------------------
Player::Player() : _shape(sf::Vector2f(32, 32)), _isMoving(false), _rotation(0) {
    _shape.setFillColor(sf::Color::Blue);
    _shape.setOrigin(16,16);

    bind(Action(sf::Keyboard::Up), [this](const sf::Event&) {
        _isMoving = true;
    });

    bind(Action(sf::Keyboard::Left), [this](const sf::Event&) {
        _rotation -= 1;
    });

    bind(Action(sf::Keyboard::Right), [this](const sf::Event&) {
        _rotation += 1;
    });
}

void Player::update(sf::Time deltaTime) {
    float seconds = deltaTime.asSeconds();
    if (_rotation != 0) {
        float angle = _rotation*180*seconds;
        _shape.rotate(angle);
    }
    if(_isMoving) {
        float angle = _shape.getRotation() / 180 * M_PI - M_PI / 2;
        _velocity += sf::Vector2(std::cos(angle), std::sin(angle)) * 60.0f * seconds;
    }
    _shape.move(seconds * _velocity);

    // Reset flags at the end of the frame
    _isMoving = false;
    _rotation = 0;
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_shape, states);
}

// -------------------------------------------------- ACTION METHODS --------------------------------------------------
/* copy action to another instance of action since no pointers in sf::Keyboard */
Action::Action(const Action& other) : _type(other._type) {
    std::memcpy(&_event, &other._event, sizeof(sf::Event));
}

Action& Action::operator=(const Action& other) {
    std::memcpy(&_event, &other._event, sizeof(sf::Event));
    _type = other._type;
    return *this;
}   

Action::Action(const sf::Keyboard::Key& key, int type) : _type(type) {
    _event.type = sf::Event::EventType::KeyPressed;
    _event.key.code = key;
}

Action::Action(const sf::Mouse::Button& button, int type) : _type(type) {
    _event.type = sf::Event::EventType::MouseButtonPressed;
    _event.mouseButton.button = button;
}

bool Action::operator==(const sf::Event& event) const {
    bool res = false;
    switch(event.type) {
        case sf::Event::EventType::KeyPressed:
        {
            if (_type & Type::Pressed && _event.type == sf::Event::EventType::KeyPressed)
                res = event.key.code == _event.key.code;
        }
        break;
        case sf::Event::EventType::KeyReleased:
        {
            if(_type & Type::Released && _event.type == sf::Event::EventType::KeyPressed) {
                res = event.key.code == _event.key.code;
            }
        }
        case sf::Event::EventType::MouseButtonPressed:
        {
            if(_type & Type::Pressed && _event.type == sf::Event::EventType::MouseButtonPressed) {
                res = event.mouseButton.button == _event.mouseButton.button;
                std::cout << "MOUSE BUTTON PRESSED! \n";
            }
        }
        case sf::Event::EventType::MouseButtonReleased:
        {
            if(_type & Type::Released && _event.type == sf::Event::EventType::MouseButtonReleased) {
                res = event.mouseButton.button == _event.mouseButton.button;
                std::cout << "Mouse button released! \n";
            }
        } break;
        default: break;
    }
    return res;
}

bool Action::operator==(const Action& other) const {
    return _type == other._type && other == _event;
}

bool Action::test () const {
    bool res = false;
    if (_event.type == sf::Event::EventType::KeyPressed) {
        if (_type & Type::Pressed) {
            res = sf::Keyboard::isKeyPressed(_event.key.code);
        } else if (_event.type == sf::Event::EventType::MouseButtonPressed) {
            if (_type & Type::Pressed) {
                res = sf::Mouse::isButtonPressed(_event.mouseButton.button);
            }
            return res;
        }
    }
    return res;
}

ActionTarget::ActionTarget() {

}

bool ActionTarget::processEvent(const sf::Event& event) const {
    bool res = false;
    for (auto& i : _eventPoll){
        if (i.first == event) {
            i.second(event);
            res = true;
            break;
        }
    }
    return res;
}

void ActionTarget::processEvents() {
    for (auto& i : _eventRealTime) {
        if(i.first.test()) {
            i.second(i.first._event);
        }
    }
}

void ActionTarget::bind(const Action& action, const FuncType& callback) {
    if(action._type & Action::Type::RealTime) {
        _eventRealTime.emplace_back(action, callback);
    } else {
        _eventPoll.emplace_back(action, callback);
    }
}

void ActionTarget::unbind(const Action& action) {
    auto remove_func = [&action](const std::pair<Action, FuncType>& pair) -> bool {
        return pair.first == action;
    };

    if (action._type& Action::Type::RealTime) {
        _eventRealTime.remove_if(remove_func);
    } else {
        _eventPoll.remove_if(remove_func);
    }
}

// -------------------------------------------------- MAIN --------------------------------------------------
int main(int argc, char* argv[]) {
    Game game;
    game.run(frames); // Run the game at 60 frames per second
}