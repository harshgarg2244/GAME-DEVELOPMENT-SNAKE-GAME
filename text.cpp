#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

const int WIDTH = 600, HEIGHT = 600;
const int SIZE = 20;
const int SPEED = 150; // Speed in milliseconds

struct SnakeSegment {
    int x, y;
};

class SnakeGame {
private:
    RenderWindow window;
    std::vector<SnakeSegment> snake;
    SnakeSegment food;
    int dx, dy;
    Clock clock;
    Font font;
    Text gameOverText;
    SoundBuffer eatBuffer, gameOverBuffer;
    Sound eatSound, gameOverSound;
    bool isGameOver;
    int speed;
    
public:
    SnakeGame() : window(VideoMode(WIDTH, HEIGHT), "Snake Game"), dx(SIZE), dy(0), isGameOver(false), speed(SPEED) {
        window.setFramerateLimit(60);
        
        // Load sounds
        eatBuffer.loadFromFile("eat.wav");
        gameOverBuffer.loadFromFile("gameover.wav");
        eatSound.setBuffer(eatBuffer);
        gameOverSound.setBuffer(gameOverBuffer);
        
        // Load font
        font.loadFromFile("arial.ttf");
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setString("Game Over! Press R to Restart");
        gameOverText.setPosition(WIDTH / 6, HEIGHT / 2 - 20);
        
        // Initialize snake
        snake.push_back({WIDTH / 2, HEIGHT / 2});
        spawnFood();
    }
    
    void spawnFood() {
        food.x = (rand() % (WIDTH / SIZE)) * SIZE;
        food.y = (rand() % (HEIGHT / SIZE)) * SIZE;
    }
    
    void handleInput() {
        if (Keyboard::isKeyPressed(Keyboard::Up) && dy == 0) { dx = 0; dy = -SIZE; }
        else if (Keyboard::isKeyPressed(Keyboard::Down) && dy == 0) { dx = 0; dy = SIZE; }
        else if (Keyboard::isKeyPressed(Keyboard::Left) && dx == 0) { dx = -SIZE; dy = 0; }
        else if (Keyboard::isKeyPressed(Keyboard::Right) && dx == 0) { dx = SIZE; dy = 0; }
    }
    
    void update() {
        if (isGameOver) return;
        
        if (clock.getElapsedTime().asMilliseconds() < speed) return;
        clock.restart();
        
        SnakeSegment newHead = { snake[0].x + dx, snake[0].y + dy };
        
        // Check collisions
        if (newHead.x < 0 || newHead.y < 0 || newHead.x >= WIDTH || newHead.y >= HEIGHT) {
            gameOver();
            return;
        }
        
        for (auto& segment : snake) {
            if (segment.x == newHead.x && segment.y == newHead.y) {
                gameOver();
                return;
            }
        }
        
        snake.insert(snake.begin(), newHead);
        
        if (newHead.x == food.x && newHead.y == food.y) {
            eatSound.play();
            spawnFood();
            if (speed > 50) speed -= 5; // Increase difficulty
        } else {
            snake.pop_back();
        }
    }
    
    void gameOver() {
        isGameOver = true;
        gameOverSound.play();
    }
    
    void draw() {
        window.clear();
        
        if (isGameOver) {
            window.draw(gameOverText);
        } else {
            RectangleShape shape(Vector2f(SIZE, SIZE));
            
            for (auto& segment : snake) {
                shape.setPosition(segment.x, segment.y);
                shape.setFillColor(Color::Green);
                window.draw(shape);
            }
            
            shape.setPosition(food.x, food.y);
            shape.setFillColor(Color::Red);
            window.draw(shape);
        }
        
        window.display();
    }
    
    void restart() {
        isGameOver = false;
        snake.clear();
        snake.push_back({WIDTH / 2, HEIGHT / 2});
        dx = SIZE;
        dy = 0;
        speed = SPEED;
        spawnFood();
    }
    
    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();
                if (isGameOver && event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                    restart();
                }
            }
            handleInput();
            update();
            draw();
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    SnakeGame game;
    game.run();
    return 0;
}
