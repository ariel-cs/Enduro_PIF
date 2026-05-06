#ifndef GAME_H
#define GAME_H
 
#include <stdbool.h>

// Isso aqui é literalmente a maquina de estados do jogo.

typedef struct Player Player;
typedef struct EnemyList EnemyList;
typedef struct Track Track;

typedef enum {
    STATE_MENU,
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
    int cars_passed_total;
    float elapsed_time;
    bool is_paused;
    int top_score;
} GameState;
 
// API da maquina de estados
GameState *init_game(void);
void update_game(GameState *state, float dt);
void change_state(GameState *state, GameStateType new_state);
void free_game(GameState *state);

#endif