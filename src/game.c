#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "player.h"
#include "track.h"
#include "enemy.h"
#include "collision.h"

GameState *init_game(void){
    srand((unsigned int)time(NULL));

    GameState *state = (GameState *)malloc(sizeof(GameState));

    state->current_state = STATE_LOGO;
    state->logo_timer = 0.0f;
    state->elapsed_time = 0.0f;
    state->is_paused = false;
    state->day = 1;
    state->cars_to_pass = 0;
    state->cars_passed_today = 0;
    state->cars_passed_total = 0;
    state->top_score = 0;
    state->player = (Player *)malloc(sizeof(Player));
    state->track = (Track *)malloc(sizeof(Track));
    state->enemies = (EnemyList *)malloc(sizeof(EnemyList));
    //Simplesmente não tem inimigo nas primeiras 1000 metros então isso aqui conserta!
    InitEnemyList(state->enemies);
    for (int i = 0; i < 18; i++) {
        float z = 100.0f + (rand() % 1100);  // 100 a 1200
        float x = ((rand() % 1601) - 800) / 1000.0f;
        float speed = 0.05f + ((rand() % 26) / 100.0f);
        Color color;
        int colorChoice = rand() % 5;
        switch (colorChoice) {
            case 0: color = BLUE; break;
            case 1: color = DARKGREEN; break;
            case 2: color = PURPLE; break;
            case 3: color = ORANGE; break;
            default: color = MAROON; break;
        }
        SpawnEnemyAt(state->enemies, z, x, speed, color);
    }

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
            if (state->logo_timer >= 2.0f){
                change_state(state, STATE_TITLE);
            }
            break;

        case STATE_TITLE:
            break;

        case STATE_PLAYING: {

            UpdatePlayer(state->player);
            UpdateEnemies(state->enemies, dt, state->player->z);
            int passed = CountPassedEnemies(state->enemies, state->player->z);
            state->cars_passed_today += passed;
            state->cars_passed_total += passed;
            if (CheckPlayerEnemyCollisions(state->player, state->enemies)) {
                state->player->speed *= 0.35f;
            }
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
    if (state->enemies != NULL) {
        FreeEnemyList(state->enemies);
        free(state->enemies);
    }

    free(state);
}
