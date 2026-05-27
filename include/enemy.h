#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "player.h"
#include "track.h"

struct Enemy {
    float z;
    float x;
    float speed;
    Color color;
    struct Enemy *next;
};

struct EnemyList {
    struct Enemy *head;
    int count;
};

void InitEnemyList(struct EnemyList *list);
void SpawnEnemy(struct EnemyList *list, float playerZ);
void SpawnEnemyAt(struct EnemyList *list, float z, float x, float speed, Color color);
void UpdateEnemies(struct EnemyList *list, float dt, float playerZ);
void DrawEnemies(struct EnemyList *list, struct Player *player, struct Track *track);
void FreeEnemyList(struct EnemyList *list);

#endif
