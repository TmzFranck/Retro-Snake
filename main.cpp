#include <cstddef>
#include <deque>
#include <raylib.h>
#include <raymath.h>

// Game State
enum class GameState : bool { RUNNING = true, PAUSE = false };

// cell Game
constexpr int CELL_SIZE = 30;
constexpr int CELL_COUNT = 25;
constexpr int OFFSET = 75;

// time
double last_update_time = 0;

// screen resolution
constexpr int HEIGHT = CELL_SIZE * CELL_COUNT;
constexpr int WIDTH = CELL_SIZE * CELL_COUNT;

// custome Colors
Color Green = {173, 204, 96, 255};
Color Darkgreen = {43, 51, 24, 255};

bool EventTriggered(double interval) {
  double currentTime = GetTime();
  if (currentTime - last_update_time >= interval) {
    last_update_time = currentTime;
    return true;
  }
  return false;
}

bool ElementInQueue(std::deque<Vector2> body, Vector2 pos) {

  for (size_t i = 0; i < body.size(); ++i) {
    if (Vector2Equals(body[i], pos)) {
      return true;
    }
  }
  return false;
}

class Food {
public:
  Food(std::deque<Vector2> &body) {
    Image image = LoadImage("Graphics/food.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    pos = generate_random_pos(body);
  }
  ~Food() { UnloadTexture(texture); }

  Vector2 pos;
  Texture2D texture;

  void Draw() {
    DrawTexture(texture, OFFSET + pos.x * CELL_SIZE, OFFSET + pos.y * CELL_SIZE,
                WHITE);
  }

  Vector2 generate_random_pos(std::deque<Vector2> const &snake_body) {
    Vector2 pos;
    do {
      pos = {(float)(GetRandomValue(0, CELL_COUNT - 1)),
             (float)(GetRandomValue(0, CELL_COUNT - 1))};
    } while (ElementInQueue(snake_body, pos));
    return pos;
  }
};

class Snake {
public:
  std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  Vector2 direction{1, 0};
  bool addSegment = false;

  void Draw() {
    for (size_t i = 0; i < body.size(); ++i) {
      Rectangle segment{OFFSET + body[i].x * (float)CELL_SIZE,
                        OFFSET + body[i].y * (float)CELL_SIZE,
                        (float)(CELL_SIZE), (float)CELL_SIZE};
      DrawRectangleRounded(segment, 0.5, 6, Darkgreen);
    }
  }

  void Update() {
    body.push_front(Vector2Add(body[0], direction));
    if (addSegment) {
      addSegment = false;
    } else {
      body.pop_back();
    }
  }

  void Reset() {
    body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    direction = {1, 0};
  }
};

class Game {
public:
  Snake snake;
  Food food{snake.body};
  GameState game_stage = GameState::RUNNING;
  unsigned int score = 0;
  Sound eatSound;
  Sound wallSound;

  Game() {
    InitAudioDevice();
    eatSound = LoadSound("Sounds/eat.mp3");
    wallSound = LoadSound("Sounds/wall.mp3");
  }
  ~Game() {
    UnloadSound(eatSound);
    UnloadSound(wallSound);
    CloseAudioDevice();
  }

  void Draw() {
    food.Draw();
    snake.Draw();
  }

  void Update() {

    if (game_stage == GameState::RUNNING) {
      snake.Update();
      CheckCollisionWithFood();
      CheckCollisionWithEdges();
      CheckCollisionWithTail();
    }
  }

  void CheckCollisionWithFood() {
    if (Vector2Equals(snake.body[0], food.pos)) {
      food.pos = food.generate_random_pos(snake.body);
      snake.addSegment = true;
      score++;
      PlaySound(eatSound);
    }
  }

  void CheckCollisionWithEdges() {
    if ((snake.body[0].x == CELL_COUNT || snake.body[0].x == -1) ||
        (snake.body[0].y == CELL_COUNT || snake.body[0].y == -1)) {
      GameOver();
    }
  }

  void GameOver() {
    snake.Reset();
    food.pos = food.generate_random_pos(snake.body);
    game_stage = GameState::PAUSE;
    score = 0;
    PlaySound(wallSound);
  }

  void CheckCollisionWithTail() {

    std::deque<Vector2> tail = snake.body;
    tail.pop_front();
    if (ElementInQueue(tail, snake.body[0])) {
      GameOver();
    }
  }
};

int main() {
  InitWindow(WIDTH + (2 * OFFSET), HEIGHT + (2 * OFFSET), "Retro Snake ");
  SetTargetFPS(60);

  Game game;

  while (!WindowShouldClose()) {
    BeginDrawing();

    // Event Handling
    if (EventTriggered(0.2)) {
      game.Update();
    }
    if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
      game.snake.direction = Vector2{0, 1};
      game.game_stage = GameState::RUNNING;
    }
    if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
      game.snake.direction = Vector2{0, -1};
      game.game_stage = GameState::RUNNING;
    }
    if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
      game.snake.direction = Vector2{1, 0};
      game.game_stage = GameState::RUNNING;
    }
    if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
      game.snake.direction = Vector2{-1, 0};
      game.game_stage = GameState::RUNNING;
    }
    if (IsKeyPressed(KEY_SPACE)) {
      game.game_stage = GameState::PAUSE;
    }

    // drawing
    ClearBackground(Green);
    DrawRectangleLinesEx(Rectangle{float(OFFSET - 5), float(OFFSET - 5),
                                   float(WIDTH + 10), float(HEIGHT + 10)},
                         5, Darkgreen);
    DrawText("Retro Snake", OFFSET - 5, 20, 40, Darkgreen);
    DrawText(TextFormat("Score: %i", game.score), OFFSET - 5,
             OFFSET + HEIGHT + 10, 40, Darkgreen);
    game.Draw();

    EndDrawing();
  }
  CloseWindow();
}
