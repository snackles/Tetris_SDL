#include "tetris.hpp"
#include "constants.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>

const std::array<std::array<Point, 4>, 7> TETROMINO_SHAPES = {
    {// I
     {{{0, -1}, {0, 0}, {0, 1}, {0, 2}}},
     // O
     {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
     // T
     {{{0, 0}, {-1, 0}, {1, 0}, {0, -1}}},
     // S
     {{{0, 0}, {1, 0}, {-1, 1}, {0, 1}}},
     // Z
     {{{0, 0}, {-1, 0}, {0, 1}, {1, 1}}},
     // J
     {{{0, 0}, {-1, 0}, {1, 0}, {1, 1}}},
     // L
     {{{-1, 1}, {-1, 0}, {0, 0}, {1, 0}}}}};

// Инициализация игры
bool init_game(GameData& game) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

	// Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Создание окна
    game.window = SDL_CreateWindow(
        "Tetris (SDL2)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
		);
    
    if (!game.window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Создание рендерера
    game.renderer = SDL_CreateRenderer(
        game.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);
    
    if (!game.renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }	
    // Установка режима смешивания для прозрачности
    SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_BLEND);
    
    // Инициализация игрового поля
    game.board.width = BOARD_WIDTH;
    game.board.height = BOARD_HEIGHT;
    game.board.grid.clear();
	game.board.grid = std::vector<std::vector<int>>(
		BOARD_HEIGHT, 
        std::vector<int>(BOARD_WIDTH, TYPE_NONE)
		);
    
    // Инициализация состояния игры
    game.game_state = STATE_PLAYING;
	game.fall_speed = INITIAL_FALL_SPEED;
    game.fall_timer = 0.0f;
    game.game_over = false;
	game.is_paused = false;
    game.is_running = true;

	// Создание первой и следующей фигур
    game.next_piece = create_tetromino(std::rand() % 7);
    spawn_new_piece(game);

	return true;
}

// Очистка ресурсов
void cleanup(GameData& game) {
	
	// Освобождение SDL объектов
    if (game.renderer != NULL) {
        SDL_DestroyRenderer(game.renderer);
        game.renderer = NULL;
    }
    
    if (game.window != NULL) {
        SDL_DestroyWindow(game.window);
        game.window = NULL;
    }
    
    // Завершение SDL подсистем
	SDL_Quit();
}

// Создание новой фигуры
Tetromino create_tetromino(int type) {
    Tetromino piece;
    piece.type = type;
//    piece.rotation = 0;
    piece.color = get_color_for_type(type);
    piece.position = {BOARD_WIDTH / 2 - 1, 0};
    
    // Копирование формы
    for (int i = 0; i < 4; ++i) {
        piece.blocks[i] = TETROMINO_SHAPES[type][i];
    }
    
    return piece;
}

// Получение цвета для типа фигуры
SDL_Color get_color_for_type(int type) {
    switch (type) {
        case TYPE_I: return COLOR_I;
        case TYPE_O: return COLOR_O;
        case TYPE_T: return COLOR_T;
        case TYPE_S: return COLOR_S;
        case TYPE_Z: return COLOR_Z;
        case TYPE_J: return COLOR_J;
        case TYPE_L: return COLOR_L;
        default: return {255, 255, 255, 255};
    }
}

// Обновление игры
void update_game(GameData& game, float dt) {
    if (game.game_over) {
        game.game_state = STATE_GAME_OVER;
        return;
    }
    
    // Обновление таймера падения
    game.fall_timer += dt * 1000.0f; // в миллисекундах
    
    // Автоматическое падение
    if (game.fall_timer >= game.fall_speed) {
        if (!move_piece(game.current_piece, 0, 1, game.board)) {
			// Фиксация фигуры
            merge_piece(game);
			// Создание новой фигуры
			spawn_new_piece(game);
		}
		game.fall_timer = 0.0f;
    }
}

// Движение фигуры
bool move_piece(Tetromino& piece, int dx, int dy, const GameBoard& board) {
    Tetromino test_piece = piece;
    test_piece.position.x += dx;
    test_piece.position.y += dy;
    
    if (!check_collision(test_piece, board)) {
        piece = test_piece;
        return true;
    }
    return false;
}

// Фиксация фигуры
void merge_piece(GameData& game) {
    for (const auto& block : game.current_piece.blocks) {
        int x = game.current_piece.position.x + block.x;
        int y = game.current_piece.position.y + block.y;
        
        if (y >= 0 && x >= 0 && x < game.board.width && y < game.board.height) {
            game.board.grid[y][x] = game.current_piece.type;
        }
    }
}

// Проверка коллизий
bool check_collision(const Tetromino& piece, const GameBoard& board) {
    for (const auto& block : piece.blocks) {
        int x = piece.position.x + block.x;
        int y = piece.position.y + block.y;
        
        // Проверка границ
        if (x < 0 || x >= board.width || y >= board.height) {
            return true;
		}
		//Проверка фигур
		if (y >= 0 && board.grid[y][x] != TYPE_NONE) {
			return true;
		}
	}
    return false;
}

// Создание новой фигуры
void spawn_new_piece(GameData& game) {
    game.current_piece = game.next_piece;
    game.next_piece = create_tetromino(std::rand() % 7);
    game.current_piece.position = {BOARD_WIDTH / 2 - 1, 0};
}

// Жесткое падение
void hard_drop_piece(GameData& game) {
	while (move_piece(game.current_piece, 0, 1, game.board)){}
	merge_piece(game);
	spawn_new_piece(game);
}
