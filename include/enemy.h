#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "player.h"
#include "track.h"

typedef struct Enemy {
    float z;
    float x;
    float speed;
    int faixa;
    bool passou;
    struct Enemy *next;
} Enemy;

typedef struct EnemyList {
    struct Enemy *head;
    int count;
    Texture2D texture;
    int   wheelFrame;  // 0 ou 1: alterna as duas linhas de animação da roda
    float wheelTimer;
} EnemyList;

void InitEnemyList(struct EnemyList *list);
void SpawnEnemy(struct EnemyList *list, float playerZ);
void SpawnEnemyAt(struct EnemyList *list, float z, float x, float speed);
void UpdateEnemies(struct EnemyList *list, float dt, float playerZ);
void CheckPassedEnemies(struct EnemyList *list, float playerZ,int *car_passed,int *car_lost);
void DrawEnemies(struct EnemyList *list, struct Player *player, struct Track *track);
void FreeEnemyList(struct EnemyList *list);

#endif
