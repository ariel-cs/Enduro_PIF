#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Isso aqui é literalmente a maquina de estados do jogo.

typedef struct Player Player;
typedef struct EnemyList EnemyList;
typedef struct Track Track;

typedef enum {
    STATE_LOGO,
    STATE_TITLE,
    STATE_MENU,
    STATE_COUNTDOWN,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_TOP_SCORES
} GameStateType;

// Estado global do jogo.
typedef struct {
    GameStateType current_state;
    Player *player;
    EnemyList *enemies;
    Track *track;
    int day;
    int cars_to_pass;
    int cars_passed_today;
    int score;
    float day_timer;
    float elapsed_time;
    float logo_timer;
    float countdown_timer;
    int countdown_value;
    bool is_paused;
    bool crashed_this_frame; // sinaliza batida na traseira p/ tocar o SFX
    char player_name[10];
    int name_length;
} GameState;

// API da maquina de estados
GameState *init_game(void);
void reset_game(GameState *state);
void update_game(GameState *state, float dt);
void change_state(GameState *state, GameStateType new_state);
void free_game(GameState *state);

#endif
