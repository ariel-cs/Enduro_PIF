#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include "enemy.h"
#include "player.h"

bool CheckEnemyCollision(struct Player *player, struct Enemy *enemy);
bool CheckPlayerEnemyCollisions(struct Player *player, struct EnemyList *list);

#endif
