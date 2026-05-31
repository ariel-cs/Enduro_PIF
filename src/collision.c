#include <stddef.h>
#include <math.h>
#include "raylib.h"
#include "collision.h"
#include "config.h"

#define ENEMY_HITBOX_WIDTH_SCALE 0.60f
#define ENEMY_HITBOX_HEIGHT_SCALE 0.80f
#define ENEMY_HITBOX_OFFSET_X_RATIO -0.08f

static float GetEnemyHeightRatio(float y) {
    float h = PLAY_HEIGHT - HORIZON;
    float t = (y - HORIZON) / h;

    if (t < 0.03f) return 10.0f / 10.0f;
    if (t < 0.07f) return 10.0f / 10.0f;
    if (t < 0.13f) return 10.0f / 18.0f;
    if (t < 0.22f) return 10.0f / 26.0f;
    if (t < 0.35f) return 18.0f / 26.0f;
    if (t < 0.50f) return 18.0f / 34.0f;
    if (t < 0.65f) return 26.0f / 42.0f;
    return 34.0f / 50.0f;
}

static Rectangle GetPlayerRect(struct Player *player) {
    float playerScreenX = (SCREEN_WIDTH / 2.0f) + (player->x * (TRACK_BASE_WIDTH / 2.0f));
    float playerWidth = 100.0f;
    float playerHeight = 70.0f;

    return (Rectangle){playerScreenX - playerWidth / 1.7f,
                       PLAY_HEIGHT - 10.0f - playerHeight,
                       playerWidth,
                       playerHeight};
}

static float GetTrackCurveAmount(struct Player *player, struct Track *track, float y) {
    float curveAmount = 0.0f;

    if (track == NULL) {
        return curveAmount;
    }

    for (int cy = PLAY_HEIGHT; cy >= (int)y; cy--) {
        float cscale = (float)(cy - HORIZON) / HORIZON;
        float cdynamicY = (cy == HORIZON) ? 0.1f : (float)(cy - HORIZON);
        float cprojectZ = 800.0f / cdynamicY;
        int ctrackIndex = ((int)(player->z + cprojectZ)) % TRACK_LENGTH;

        // mesma regra do renderer (DrawEnemies): curva só acumula dentro do alcance de visão
        if (cprojectZ < 300.0f) {
            curveAmount += track->segments[ctrackIndex].curve * (1.0f - cscale) * 0.85f;
        }
    }

    return curveAmount;
}

static bool GetEnemyRect(struct Player *player, struct Enemy *enemy, struct Track *track, Rectangle *rect) {
    float deltaZ = enemy->z - player->z;

    if (deltaZ <= 0.1f) {
        return false;
    }

    float y = HORIZON + 800.0f / deltaZ;

    if (y <= HORIZON || y >= PLAY_HEIGHT) {
        return false;
    }

    float scale = (y - HORIZON) / HORIZON;
    float cameraTurn = player->x * -100.0f;
    float curveAmount = GetTrackCurveAmount(player, track, y);
    // mesma projeção do renderer (DrawEnemies) para o hitbox alinhar com o sprite
    float enemyScreenX = (SCREEN_WIDTH / 2.0f) +
                         (enemy->x * (TRACK_BASE_WIDTH / 2.0f) * powf(scale, 1.4f)) +
                         curveAmount +
                         cameraTurn * (1.0f - scale);
    float roadWidth = TRACK_BASE_WIDTH * powf(scale, 1.2f);
    float enemyWidth = roadWidth * VEHICLE_TRACK_WIDTH_RATIO * ENEMY_HITBOX_WIDTH_SCALE;
    float enemyHeight = enemyWidth * GetEnemyHeightRatio(y) * ENEMY_HITBOX_HEIGHT_SCALE;
    float enemyHitboxX = enemyScreenX + enemyWidth * ENEMY_HITBOX_OFFSET_X_RATIO;

    *rect = (Rectangle){enemyHitboxX - enemyWidth / 2.0f, y - enemyHeight,
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

void DrawHitboxes(struct Player *player, struct EnemyList *list, struct Track *track) {
    Rectangle playerRect = GetPlayerRect(player);

    DrawRectangleRec(playerRect, Fade(GREEN, 0.25f));
    DrawRectangleLinesEx(playerRect, 2.0f, GREEN);

    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        Rectangle enemyRect;

        if (GetEnemyRect(player, enemy, track, &enemyRect)) {
            DrawRectangleRec(enemyRect, Fade(RED, 0.25f));
            DrawRectangleLinesEx(enemyRect, 2.0f, RED);
        }

        enemy = enemy->next;
    }
}
