#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <SDL2/SDL.h>
#include <vector>
#include <array>
#include <string>

// Структуры данных
struct Point {
    int x;
    int y;
};

struct Tetromino {
    int type;                         // TetrominoType
    std::array<Point, 4> blocks;      // 4 блока фигуры
//    int rotation;                     // Текущий поворот (0-3)
    SDL_Color color;                  // Цвет фигуры
    Point position;                   // Позиция на поле
};

struct GameBoard {
    std::vector<std::vector<int>> grid;  // TYPE_NONE или тип фигуры
    int width;
    int height;
};

// Основная структура игры
struct GameData {
    // SDL объекты
    SDL_Window* window;
    SDL_Renderer* renderer;
	// Игровые объекты
    GameBoard board;
    Tetromino current_piece;
    Tetromino next_piece;
	
	// Состояние игры
    int game_state;
	float fall_speed;
    float fall_timer;
    bool game_over;
	bool is_paused;
    bool is_running;

	// Время
    uint32_t last_time;
    float delta_time;
};

// Прототипы функций
// Инициализация
bool init_game(GameData &game);
void cleanup(GameData &game);

// Игровая логика
void update_game(GameData& game, float dt);
void spawn_new_piece(GameData &game);
bool move_piece(Tetromino &piece, int dx, int dy, const GameBoard &board);
bool check_collision(const Tetromino &piece, const GameBoard &board);

// Графика
void render_game(GameData& game);
void draw_board(GameData& game);
void draw_current_piece(GameData& game);
void draw_next_piece(GameData &game);
void draw_grid(GameData& game);
void draw_block(GameData &game, int x, int y, const SDL_Color &color,
                bool is_ghost = false);

// Ввод
void process_events(GameData &game);

// Утилиты
SDL_Color get_color_for_type(int type);
Tetromino create_tetromino(int type);

#endif // TETRIS_HPP
