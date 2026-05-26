#include <stdlib.h>
#include "game.h"
#include "player.h"
#include "track.h"

GameState *init_game(void){
    GameState *state = (GameState *)malloc(sizeof(GameState));

    state->current_state = STATE_LOGO;
    state->logo_timer = 0.0f;
    state->elapsed_time = 0.0f;
    state->is_paused = false;
    state->player = (Player *)malloc(sizeof(Player));
    state->track = (Track *)malloc(sizeof(Track));
    state->enemies = NULL;

    InitPlayer(state->player);
    InitTrack(state->track);

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

        case STATE_PLAYING: {

            UpdatePlayer(state->player);
            }
            break;

        default:break;
    }
}

void free_game(GameState *state){
    if (state->player != NULL) {
        free(state->player);
    }
    if (state->track != NULL) {
        free(state->track);
    }

    free(state);
}
