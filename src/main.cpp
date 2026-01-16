#include "tetris.hpp"
#include "constants.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== Tetris SDL2 (SFML-like) ===\n";
    
    GameData game;
    
    try {
        // Инициализация игры
        if (!init_game(game)) {
            std::cerr << "Failed to initialize game!\n";
            return EXIT_FAILURE;
        }
        
        // Главный игровой цикл
        game.last_time = SDL_GetTicks();
		
        while (game.is_running) {
			// Обработка событий
            process_events(game);
			
            // Расчет дельты времени
            Uint32 current_time = SDL_GetTicks();
            game.delta_time = (current_time - game.last_time) / 1000.0f;
            game.last_time = current_time;;
			
			// Отрисовка игры
			render_game(game);
			SDL_RenderPresent(game.renderer);
			
            // Обновление игры
            if (!game.is_paused && game.game_state == STATE_PLAYING) {
                update_game(game, game.delta_time);
            }
            
            // Ограничение FPS
            if (game.delta_time < FRAME_TIME) {
                SDL_Delay(static_cast<Uint32>((FRAME_TIME - game.delta_time) * 1000));
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    // Освобождение ресурсов
    cleanup(game);
    
    return EXIT_SUCCESS;
}
