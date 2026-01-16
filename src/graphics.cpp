#include "tetris.hpp"
#include "constants.hpp"
#include <string>

// Отрисовка всей игры
void render_game(GameData& game) {
	
    // Очистка экрана
	SDL_SetRenderDrawColor(game.renderer, 
        COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, 
        COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(game.renderer);
    
    // Рисование игрового поля
    draw_board(game);

	// Рисование текущей фигуры
    draw_current_piece(game);

	// Рисование следующей фигуры
    draw_next_piece(game);
}

// Рисование блока
void draw_block(GameData& game, int x, int y, const SDL_Color& color, bool is_ghost) {
    SDL_Rect rect = {
        BOARD_OFFSET_X + x * BLOCK_SIZE + BLOCK_OUTLINE / 2,
        BOARD_OFFSET_Y + y * BLOCK_SIZE + BLOCK_OUTLINE / 2,
        BLOCK_SIZE - BLOCK_OUTLINE,
        BLOCK_SIZE - BLOCK_OUTLINE
    };
	
	// Цвет блока
	SDL_SetRenderDrawColor(game.renderer, 
            color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(game.renderer, &rect);	
}

// Рисование игрового поля
void draw_board(GameData& game) {
    // Фон поля
    SDL_Rect board_bg = {
        BOARD_OFFSET_X - BLOCK_OUTLINE,
        BOARD_OFFSET_Y - BLOCK_OUTLINE,
        BOARD_WIDTH * BLOCK_SIZE + BLOCK_OUTLINE * 2,
        BOARD_HEIGHT * BLOCK_SIZE + BLOCK_OUTLINE * 2
    };
    
    SDL_SetRenderDrawColor(game.renderer, 
        COLOR_BOARD.r, COLOR_BOARD.g, COLOR_BOARD.b, COLOR_BOARD.a);
    SDL_RenderFillRect(game.renderer, &board_bg);
    
    // Сетка
    draw_grid(game);
    
	// Блоки на поле
     for (int y = 0; y < game.board.height; ++y) {
        for (int x = 0; x < game.board.width; ++x) {
            if (game.board.grid[y][x] != TYPE_NONE){
                SDL_Color color = get_color_for_type(game.board.grid[y][x]);
                draw_block(game, x, y, color);
            }
        }
    }
 }

// Рисование сетки
void draw_grid(GameData& game) {
	
    SDL_SetRenderDrawColor(game.renderer, 
        COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
    
    // Вертикальные линии
    for (int x = 0; x <= BOARD_WIDTH; ++x) {
        SDL_RenderDrawLine(game.renderer,
            BOARD_OFFSET_X + x * BLOCK_SIZE,
            BOARD_OFFSET_Y,
            BOARD_OFFSET_X + x * BLOCK_SIZE,
            BOARD_OFFSET_Y + BOARD_HEIGHT * BLOCK_SIZE);
    }
    
    // Горизонтальные линии
    for (int y = 0; y <= BOARD_HEIGHT; ++y) {
        SDL_RenderDrawLine(game.renderer,
            BOARD_OFFSET_X,
            BOARD_OFFSET_Y + y * BLOCK_SIZE,
            BOARD_OFFSET_X + BOARD_WIDTH * BLOCK_SIZE,
            BOARD_OFFSET_Y + y * BLOCK_SIZE);
    }
}

// Рисование текущей фигуры
void draw_current_piece(GameData& game) {
    for (const auto& block : game.current_piece.blocks) {
        int x = game.current_piece.position.x + block.x;
        int y = game.current_piece.position.y + block.y;
        
        if (y >= 0) {
            draw_block(game, x, y, game.current_piece.color);
        }
    }
}

// Рисование следующей фигуры (Поправить расположение фона/фигуры)
void draw_next_piece(GameData& game) {
    // Фон
   SDL_Rect next_bg = {NEXT_PIECE_X, NEXT_PIECE_Y, 120, 120};
	SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(game.renderer, &next_bg);

	// Рисуем следующую фигуру
    for (const auto& block : game.next_piece.blocks) {
        int x = NEXT_PIECE_X / BLOCK_SIZE + block.x;
        int y = NEXT_PIECE_Y / BLOCK_SIZE + block.y;
        draw_block(game, x, y, game.next_piece.color);
    }
}
