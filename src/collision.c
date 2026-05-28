#include <stddef.h>
#include "raylib.h"
#include "collision.h"
#include "config.h"

static Rectangle GetPlayerRect(struct Player *player) {
    float playerScreenX = (SCREEN_WIDTH / 2.0f) + (player->x * 300.0f);

    return (Rectangle){playerScreenX - 25.0f, SCREEN_HEIGHT - 85.0f, 50.0f, 50.0f};
}

static float GetTrackCurveAmount(struct Player *player, struct Track *track, float y) {
    float curveAmount = 0.0f;

    if (track == NULL) {
        return curveAmount;
    }

    for (int cy = SCREEN_HEIGHT; cy >= (int)y; cy--) {
        float cscale = (float)(cy - HORIZON) / HORIZON;
        float cdynamicY = (cy == HORIZON) ? 0.1f : (float)(cy - HORIZON);
        float cprojectZ = 800.0f / cdynamicY;
        int ctrackIndex = ((int)(player->z + cprojectZ)) % TRACK_LENGTH;

        curveAmount += track->segments[ctrackIndex].curve * (1.0f - cscale) * 0.85f;
    }

    return curveAmount;
}

static bool GetEnemyRect(struct Player *player, struct Enemy *enemy, struct Track *track, Rectangle *rect) {
    float deltaZ = enemy->z - player->z;

    if (deltaZ <= 0.1f) {
        return false;
    }

    float y = HORIZON + 800.0f / deltaZ;

    if (y <= HORIZON || y >= SCREEN_HEIGHT) {
        return false;
    }

    float scale = (y - HORIZON) / HORIZON;
    float cameraTurn = player->x * -100.0f;
    float curveAmount = GetTrackCurveAmount(player, track, y);
    float enemyScreenX = (SCREEN_WIDTH / 2.0f) +
                         (enemy->x * 300.0f * scale) +
                         curveAmount +
                         cameraTurn * (1.0f - scale);
    float enemyWidth = 50.0f * scale;
    float enemyHeight = 35.0f * scale;

    *rect = (Rectangle){enemyScreenX - enemyWidth / 2.0f, y - enemyHeight,
                        enemyWidth, enemyHeight};
    return true;
}

bool CheckEnemyCollision(struct Player *player, struct Enemy *enemy) {
    Rectangle enemyRect;

    if (!GetEnemyRect(player, enemy, NULL, &enemyRect)) {
        return false;
    }

    return CheckCollisionRecs(GetPlayerRect(player), enemyRect);
}

bool CheckPlayerEnemyCollisions(struct Player *player, struct EnemyList *list) {
    return CheckPlayerEnemyTrackCollisions(player, list, NULL);
}

bool CheckPlayerEnemyTrackCollisions(struct Player *player, struct EnemyList *list, struct Track *track) {
    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        Rectangle enemyRect;

        if (GetEnemyRect(player, enemy, track, &enemyRect) &&
            CheckCollisionRecs(GetPlayerRect(player), enemyRect)) {
            return true;
        }

        enemy = enemy->next;
    }

    return false;
}
