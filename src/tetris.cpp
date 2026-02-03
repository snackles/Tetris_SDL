#include "../include/tetris.hpp"
#include "../include/constants.hpp"
#include <iostream>
#include <random>

static std::mt19937 gen(std::random_device{}());

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

	 // Инициализация шрифта
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
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
	game.score = 0;
    game.level = 1;
    game.lines_cleared = 0;
    game.game_state = STATE_PLAYING;
	game.fall_speed = INITIAL_FALL_SPEED;
    game.fall_timer = 0.0f;
    game.game_over = false;
	game.is_paused = false;
    game.is_running = true;

	// Инициализация графики
    if (!init_graphics(game)) {
        std::cerr << "Failed to initialize graphics!\n";
        return false;
    }

	// Создание фигур
	static std::uniform_int_distribution<int> shape_dist(0, 6);
    game.next_piece = create_tetromino(shape_dist(gen));
    spawn_new_piece(game);

	return true;
}

// Инициализация графики
bool init_graphics(GameData& game) {
	 // Загрузка шрифта
    game.font = TTF_OpenFont("assets/fonts/Tetris.ttf", 24);
    if (!game.font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
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

	if (game.font != NULL) {
        TTF_CloseFont(game.font);
        game.font = NULL;
    }
    
    // Завершение SDL подсистем
	TTF_Quit();
    SDL_Quit();
}

// Создание фигуры
Tetromino create_tetromino(int type) {
    Tetromino piece;
    piece.type = type;
    piece.rotation = 0;
    piece.color = get_color_for_type(type);
    piece.position = {BOARD_WIDTH / 2 - 1, 0};
    
    for (int i = 0; i < 4; ++i) {
        piece.blocks[i] = TETROMINO_SHAPES[type][i];
    }
    
    return piece;
}

// Получение цвета для фигуры
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
    
    // Падение
    if (game.fall_timer >= game.fall_speed) {
        if (!move_piece(game.current_piece, 0, 1, game.board)) {
			// Фиксация фигуры
            merge_piece(game);
			
			// Очистка линий
			int lines = clear_completed_lines(game.board);
			if (lines > 0) {
				// Подсчет очков
                switch (lines) {
				case 1: add_score(game, SCORE_SINGLE * game.level); break;
				case 2: add_score(game, SCORE_DOUBLE * game.level); break;
				case 3: add_score(game, SCORE_TRIPLE * game.level); break;
				case 4: add_score(game, SCORE_TETRIS * game.level); break;
                }
                
                game.lines_cleared += lines;
                if (game.lines_cleared >= game.level * LINES_PER_LEVEL) {
                    game.level++;
                    game.fall_speed *= SPEED_INCREASE_PER_LEVEL;
                }
            }
			
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

// Поворот
void rotate_piece(Tetromino& piece, bool clockwise, const GameBoard& board) {
    if (piece.type == TYPE_O) return;
    
    Tetromino test_piece = piece;
    
    for (auto& block : test_piece.blocks) {
        int x = block.x;
        int y = block.y;
        
        if (clockwise) {
            block.x = -y;
            block.y = x;
        } else {
            block.x = y;
            block.y = -x;
        }
    }
    
    // Касание стен
    Point offsets[] = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}};
    
    for (const auto& offset : offsets) {
        Tetromino kick_test = test_piece;
        kick_test.position.x += offset.x;
        kick_test.position.y += offset.y;
        
        if (!check_collision(kick_test, board)) {
            piece = kick_test;
            piece.rotation = (piece.rotation + (clockwise ? 1 : 3)) % 4;
            return;
        }
    }
}

// Очистка линий
int clear_completed_lines(GameBoard& board) {
    int lines_cleared = 0;
    
    for (int y = board.height - 1; y >= 0; --y) {
        bool line_full = true;
        
        for (int x = 0; x < board.width; ++x) {
            if (board.grid[y][x] == TYPE_NONE) {
                line_full = false;
                break;
            }
        }
        
        if (line_full) {
            for (int y2 = y; y2 > 0; --y2) {
                for (int x = 0; x < board.width; ++x) {
                    board.grid[y2][x] = board.grid[y2 - 1][x];
                }
            }
            
            for (int x = 0; x < board.width; ++x) {
                board.grid[0][x] = TYPE_NONE;
            }
            
            ++lines_cleared;
            ++y;
        }
    }
    
    return lines_cleared;
}

// Создание новой фигуры
void spawn_new_piece(GameData& game) {
    game.current_piece = game.next_piece;
	static std::uniform_int_distribution<int> shape_dist(0, 6);
    game.next_piece = create_tetromino(shape_dist(gen));
    game.current_piece.position = {BOARD_WIDTH / 2 - 1, 0};
	if (check_collision(game.current_piece, game.board)){
			game.game_over = true;
		}
}

// Жесткое падение
void hard_drop_piece(GameData& game) {
	int cells_dropped = 0;
	while (move_piece(game.current_piece, 0, 1, game.board)){
		cells_dropped++;
	}

	game.score += cells_dropped * SCORE_HARD_DROP_PER_CELL;
	merge_piece(game);
}

void add_score(GameData& game, int points) {
    game.score += points;
}

void init_random() {
    static bool initialized = false;
    if (!initialized) {
        // Комбинация времени и random_device для лучшей энтропии
        auto seed = std::chrono::high_resolution_clock::now()
                   .time_since_epoch().count() ^ std::random_device{}();
        gen.seed(seed);
        initialized = true;
    }
}

void reset_game(GameData& game) {
    // Очистка поля
    for (auto& row : game.board.grid) {
        std::fill(row.begin(), row.end(), TYPE_NONE);
    }
    
    // Сброс состояния
    game.score = 0;
    game.level = 1;
    game.lines_cleared = 0;
	 game.game_state = STATE_PLAYING;
    game.fall_speed = INITIAL_FALL_SPEED;
    game.game_over = false;
    game.is_paused = false;
    
    // Создание новых фигур
	static std::uniform_int_distribution<int> shape_dist(0, 6);
    game.next_piece = create_tetromino(shape_dist(gen));
    spawn_new_piece(game);
}
