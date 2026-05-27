#include <stddef.h>
#include "raylib.h"
#include "collision.h"
#include "config.h"

static Rectangle GetPlayerRect(struct Player *player) {
    float playerScreenX = (SCREEN_WIDTH / 2.0f) + (player->x * 300.0f);

    return (Rectangle){playerScreenX - 25.0f, SCREEN_HEIGHT - 85.0f, 50.0f, 50.0f};
}

static bool GetEnemyRect(struct Player *player, struct Enemy *enemy, Rectangle *rect) {
    float deltaZ = enemy->z - player->z;

    if (deltaZ <= 0.1f) {
        return false;
    }

    float y = HORIZON + 800.0f / deltaZ;

    if (y <= HORIZON || y >= SCREEN_HEIGHT) {
        return false;
    }

    float scale = (y - HORIZON) / HORIZON;
    float enemyScreenX = (SCREEN_WIDTH / 2.0f) + (enemy->x * 300.0f * scale);
    float enemyWidth = 50.0f * scale;
    float enemyHeight = 35.0f * scale;

    *rect = (Rectangle){enemyScreenX - enemyWidth / 2.0f, y - enemyHeight,
                        enemyWidth, enemyHeight};
    return true;
}

bool CheckEnemyCollision(struct Player *player, struct Enemy *enemy) {
    Rectangle enemyRect;

    if (!GetEnemyRect(player, enemy, &enemyRect)) {
        return false;
    }

    return CheckCollisionRecs(GetPlayerRect(player), enemyRect);
}

bool CheckPlayerEnemyCollisions(struct Player *player, struct EnemyList *list) {
    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        if (CheckEnemyCollision(player, enemy)) {
            return true;
        }

        enemy = enemy->next;
    }

    return false;
}
