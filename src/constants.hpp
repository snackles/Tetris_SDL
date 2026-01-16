#ifndef TETRIS_CONSTANTS_HPP
#define TETRIS_CONSTANTS_HPP

#include <SDL2/SDL.h>

// Размеры игрового поля
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;

// Размеры блоков и окна
const int BLOCK_SIZE = 30;
const int BLOCK_OUTLINE = 2;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 650;

// Позиции элементов UI
const int BOARD_OFFSET_X = 50;
const int BOARD_OFFSET_Y = 50;
const int NEXT_PIECE_X = 400;
const int NEXT_PIECE_Y = 50;

// Цвета (RGBA)
const SDL_Color COLOR_BACKGROUND = {211, 211, 211, 255};
const SDL_Color COLOR_BOARD = {20, 20, 40, 255};
const SDL_Color COLOR_GRID = {40, 40, 60, 175};

// Цвета фигур тетриса
const SDL_Color COLOR_I = {0, 255, 255, 255};     // Голубой
const SDL_Color COLOR_O = {255, 255, 0, 255};     // Желтый
const SDL_Color COLOR_T = {160, 0, 160, 255};     // Фиолетовый
const SDL_Color COLOR_S = {0, 255, 0, 255};       // Зеленый
const SDL_Color COLOR_Z = {255, 0, 0, 255};       // Красный
const SDL_Color COLOR_J = {0, 0, 255, 255};       // Синий
const SDL_Color COLOR_L = {255, 165, 0, 255};     // Оранжевый

// Игровые состояния
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
};

// Типы тетромино
enum TetrominoType {
    TYPE_I,
    TYPE_O,
    TYPE_T,
    TYPE_S,
    TYPE_Z,
    TYPE_J,
    TYPE_L,
    TYPE_NONE
};

// Скорость игры
const float INITIAL_FALL_SPEED = 1000.0f;  // 1 секунда
const float SPEED_INCREASE_PER_LEVEL = 0.9f;
const int LINES_PER_LEVEL = 10;

// Коды клавиш
const SDL_Keycode KEY_MOVE_LEFT = SDLK_LEFT;
const SDL_Keycode KEY_MOVE_RIGHT = SDLK_RIGHT;
const SDL_Keycode KEY_MOVE_DOWN = SDLK_DOWN;
const SDL_Keycode KEY_HARD_DROP = SDLK_SPACE;
const SDL_Keycode KEY_ROTATE_CW = SDLK_x;
const SDL_Keycode KEY_ROTATE_CCW = SDLK_z;

// Время
const float FRAME_TIME = 1.0f / 60.0f;  // 60 FPS
const float DAS_DELAY = 0.16f;         // Delayed Auto Shift
const float DAS_SPEED = 0.03f;         // Auto repeat rate

#endif // TETRIS_CONSTANTS_HPP
