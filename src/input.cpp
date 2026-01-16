#include "tetris.hpp"
#include "constants.hpp"
#include <iostream>

SDL_Event event;

// Обработка событий
void process_events(GameData& game) {

	game.is_running = true;
	
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
			// Выход
            case SDL_QUIT:
                game.is_running = false;
                break;
			// Нажатие
            case SDL_KEYDOWN:
                handle_key_press(game, event.key.keysym.sym);
                break;
			// Отпускание      
            case SDL_KEYUP:
                handle_key_release(game, event.key.keysym.sym);
                break;
		}
	}
	// Непрерывный ввод
	handle_keyboard(game, game.delta_time);
}

// Обработка нажатия клавиши
void handle_key_press(GameData& game, SDL_Keycode key) {
    // Защита от автоповтора
    if (event.key.repeat) {
        return;
    }
    switch (key) {
		case KEY_HARD_DROP:
            if (!game.is_paused && !game.game_over) {
                hard_drop_piece(game);
            }
            break;
	case KEY_ROTATE_CW:
		if (!game.is_paused && !game.game_over) {
			rotate_piece(game.current_piece, true, game.board);
		}
		break;
            
	case KEY_ROTATE_CCW:
		if (!game.is_paused && !game.game_over) {
			rotate_piece(game.current_piece, false, game.board);
		}
		break;
	}
}

//Обработка отпускания клавиши
void handle_key_release(GameData& game, SDL_Keycode key) {
    (void)game;
    (void)key;
}

// Непрерывный ввод
void handle_keyboard(GameData& game, float dt) {
    if (game.is_paused || game.game_over) return;
    
    static float left_timer = 0.0f;
    static float right_timer = 0.0f;
    static float down_timer = 0.0f;
    static bool left_was_pressed = false;
    static bool right_was_pressed = false;
    static bool down_was_pressed = false;
    
    const uint8_t* keystate = SDL_GetKeyboardState(NULL);
    
    // Движение влево с автоповтором
    if (keystate[SDL_SCANCODE_LEFT]) {
        if (!left_was_pressed || left_timer >= DAS_DELAY) {
            float move_rate = left_was_pressed ? DAS_SPEED : 0.0f;
            if (left_timer >= move_rate) {
                if (move_piece(game.current_piece, -1, 0, game.board)){}
                left_timer = 0.0f;
            }
        }
        left_timer += dt;
        left_was_pressed = true;
    } else {
        left_was_pressed = false;
        left_timer = 0.0f;
    }
    
    // Движение вправо с автоповтором
    if (keystate[SDL_SCANCODE_RIGHT]) {
        if (!right_was_pressed || right_timer >= DAS_DELAY) {
            float move_rate = right_was_pressed ? DAS_SPEED : 0.0f;
            if (right_timer >= move_rate) {
                if (move_piece(game.current_piece, 1, 0, game.board)){}
					right_timer = 0.0f;
            }
        }
        right_timer += dt;
        right_was_pressed = true;
    } else {
        right_was_pressed = false;
        right_timer = 0.0f;
    }
    
    // Быстрое падение вниз
    if (keystate[SDL_SCANCODE_DOWN]) {
        if (!down_was_pressed || down_timer >= 0.05f) {
            if (move_piece(game.current_piece, 0, 1, game.board)){}
            down_timer = 0.0f;
        }
        down_timer += dt;
        down_was_pressed = true;
    } else {
        down_was_pressed = false;
        down_timer = 0.0f;
    }
}
