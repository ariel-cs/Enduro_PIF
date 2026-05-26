#include <stdlib.h>
#include "raylib.h"
#include "game.h"
#include "player.h"

GameState *init_game(void){
    GameState *state = (GameState *)malloc(sizeof(GameState));

    state->current_state = STATE_LOGO;
    state->logo_timer = 0.0f;
    state->elapsed_time = 0.0f;
    state->is_paused = false;
    state->player = (Player *)malloc(sizeof(Player));
    state->track = NULL;
    state->enemies = NULL;

    InitPlayer(state->player);

    return state;
}

void change_state(GameState *state, GameStateType new_state){
    state->current_state = new_state;
}

void update_game(GameState *state, float dt){
    state->elapsed_time += dt;

    switch (state->current_state) {
        case STATE_LOGO:
            state->logo_timer +=dt;
            if (state->logo_timer >= 3.0f){
                change_state(state, STATE_TITLE);
            }
            break;

        case STATE_TITLE:
            break;

        case STATE_PLAYING:
            UpdatePlayer(state->player, 0.0f);
            break;

        default:break;
    }
}

void free_game(GameState *state){
    if (state->player != NULL) {
        free(state->player);
    }

    free(state);
}
