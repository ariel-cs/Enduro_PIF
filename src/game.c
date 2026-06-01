#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "config.h"
#include "player.h"
#include "track.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"

GameState *init_game(void){  //inicia o jogo zera pontuação cria pista e inimigos
    srand((unsigned int)time(NULL));

    GameState *state = (GameState *)malloc(sizeof(GameState));
    if (state == NULL) {
        return NULL;
    }

    state->current_state = STATE_LOGO;
    state->logo_timer = 0.0f;
    state->elapsed_time = 0.0f;
    state->is_paused = false;
    state->crashed_this_frame = false;
    state->day = 1;
    state->day_timer = DAY_DURATION;
    state->cars_to_pass = 200; // meta do dia 1
    state->cars_passed_today = 0;
    state->score = 0;
    state->name_length = 0;
    state->player_name[0] = '\0';

    state->player = (Player *)malloc(sizeof(Player));
    state->track = (Track *)malloc(sizeof(Track));
    state->enemies = (EnemyList *)malloc(sizeof(EnemyList));
    if (state->player == NULL || state->track == NULL || state->enemies == NULL) {
        free(state->player);
        free(state->track);
        free(state->enemies);
        free(state);
        return NULL;
    }
    //Simplesmente não tem inimigo nas primeiras 1000 metros então isso aqui conserta!
    InitEnemyList(state->enemies);
    for (int i = 0; i < 18; i++) {
        float z = 100.0f + (rand() % 1100);  // 100 a 1200
        float x = ((rand() % 1601) - 800) / 1000.0f;
        float speed = 0.05f + ((rand() % 26) / 100.0f);
        SpawnEnemyAt(state->enemies, z, x, speed);
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

            state->crashed_this_frame = false;

            int trackIndex = ((int)state->player->z) % TRACK_LENGTH;
            float trackCurve = state->track->segments[trackIndex].curve;

            UpdatePlayer(state->player, trackCurve);
            UpdateTrackParallax(state->track, state->player, dt);
            UpdateEnemies(state->enemies, dt, state->player->z);

            state->day_timer -= dt;

            int passed = 0;
            int lost = 0;
            CheckPassedEnemies(state->enemies, state->player->z, &passed, &lost);

            if (lost > 0){
                state->cars_passed_today -= lost;
                if (state->cars_passed_today < 0) state->cars_passed_today = 0;
            } // subtrai a meta caso seja passado

            if (passed > 0){

                float speed_ratio = state->player->speed / state->player->maxSpeed;
                float multiplier = 1.0f + speed_ratio; //multiplicador de x1 ate x2 gradativamente

                state->cars_passed_today += passed;
                state->score += (int)((5 * passed) * multiplier); // pontuação 5 por ultrapassagem e multiplicador
            }

            if (CheckPlayerEnemyTrackCollisions(state->player, state->enemies, state->track)) {
                state->player->speed *= 0.35f; // desaceleração na batida nesse caso perde 35% da velocidade
                state->crashed_this_frame = true; // dispara o SFX de batida
            }

            if(state->day_timer <= 0.0f){
                if (state->cars_passed_today >= state->cars_to_pass) {
                    state->day++;
                    state->day_timer = DAY_DURATION;
                    state->cars_passed_today = 0;
                    if (state->day == 2) state->cars_to_pass = 300; //meta dia 2
                    else state->cars_to_pass = 400; //meta dia 3 para frente
                }
                else {
                    state->name_length = 0;
                    state->player_name[0] = '\0';
                    change_state(state, STATE_GAME_OVER);
                }
            }
        } break;

        case STATE_GAME_OVER: {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <=125) && (state->name_length < 9)) {
                    state->player_name[state->name_length] = (char)key;
                    state->player_name[state->name_length + 1] = '\0';
                   state->name_length++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                state->name_length--;
                if (state->name_length < 0) state->name_length = 0;
                state->player_name[state->name_length] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && state->name_length > 0){
                SaveScore(state->player_name, state->score);
                change_state(state, STATE_TOP_SCORES);
            }
        }break;

        case STATE_TOP_SCORES: {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
                //importante ainda falta coisa
                change_state(state, STATE_MENU);
            }
        } break;
        default: break;
    }
}

void reset_game(GameState *state){
    state->day = 1;
    state->day_timer = DAY_DURATION;
    state -> cars_to_pass = 200;
    state->cars_passed_today = 0;
    state->score = 0;

    state->player->z = 0.0f;
    state->player->x = 0.0f;
    state->player->speed = 0.0f;

    FreeEnemyList(state->enemies);
    InitEnemyList(state->enemies);
    for (int i = 0; i < 18; i++) {
        float z = 100.0f + (rand() % 1100);  // 100 a 1200
        float x = ((rand() % 1601) - 800) / 1000.0f;
        float speed = 0.05f + ((rand() % 26) / 100.0f);
        SpawnEnemyAt(state->enemies, z, x, speed);
    }
}

void free_game(GameState *state){
    if (state->player != NULL) {
        free(state->player);
    }
    if (state->track != NULL) {
        FreeTrack(state->track);
        free(state->track);
    }
    if (state->enemies != NULL) {
        FreeEnemyList(state->enemies);
        free(state->enemies);
    }

    free(state);
}
