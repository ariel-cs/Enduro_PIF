#include <stdlib.h>
#include "enemy.h"
#include "config.h"
#include "track.h"

void InitEnemyList(struct EnemyList *list) {
    list->head = NULL;
    list->count = 0;
}

void SpawnEnemy(struct EnemyList *list, float playerZ) {
    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));

    enemy->z = playerZ + 200.0f + (rand() % 300);
    enemy->x = ((rand() % 1601) - 800) / 1000.0f;
    enemy->speed = 0.05f + ((rand() % 26) / 100.0f);

    int colorChoice = rand() % 5;
    switch (colorChoice) {
        case 0: enemy->color = BLUE; break;
        case 1: enemy->color = DARKGREEN; break;
        case 2: enemy->color = PURPLE; break;
        case 3: enemy->color = ORANGE; break;
        default: enemy->color = MAROON; break;
    }

    enemy->next = list->head;
    list->head = enemy;
    list->count++;
}

void SpawnEnemyAt(struct EnemyList *list, float z, float x, float speed, Color color) {
    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));

    enemy->z = z;
    enemy->x = x;
    enemy->speed = speed;
    enemy->color = color;

    enemy->next = list->head;
    list->head = enemy;
    list->count++;
}

void UpdateEnemies(struct EnemyList *list, float dt, float playerZ) {
    (void)dt; // dt não usado, corrigir no ultimo dia
    struct Enemy **current = &list->head;

    while (*current != NULL) {
        struct Enemy *enemy = *current;

        enemy->z += enemy->speed;

        if (enemy->z < playerZ - 400.0f) {
            *current = enemy->next;
            free(enemy);
            list->count--;
        } else {
            current = &enemy->next;
        }
    }

    while (list->count < 25) {
        SpawnEnemy(list, playerZ);
    }
}

void DrawEnemies(struct EnemyList *list, struct Player *player, struct Track *track) {
    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        float deltaZ = enemy->z - player->z;

        if (deltaZ > 0.1f) {
            float y = HORIZON + 800.0f / deltaZ;

            if (y > HORIZON && y < SCREEN_HEIGHT) {
                float scale = (y - HORIZON) / HORIZON;

                float curveAmount = 0.0f;
                for (int cy = SCREEN_HEIGHT; cy >= (int)y; cy--) {
                    float cscale = (float)(cy - HORIZON) / HORIZON;
                    float cdynamicY = (cy == HORIZON) ? 0.1f : (float)(cy - HORIZON);
                    float cprojectZ = 800.0f / cdynamicY;
                    int ctrackIndex = ((int)(player->z + cprojectZ)) % TRACK_LENGTH;
                    curveAmount += track->segments[ctrackIndex].curve * (1.0f - cscale) * 0.85f;
                }

                float enemyScreenX = (SCREEN_WIDTH / 2.0f) + (enemy->x * 300.0f * scale) + curveAmount;

                float enemyWidth = 50.0f * scale;
                float enemyHeight = 35.0f * scale;

                if (enemyWidth > 2.0f) {
                    DrawRectangle(enemyScreenX - enemyWidth / 2, y - enemyHeight,
                                  enemyWidth, enemyHeight, enemy->color);
                    DrawRectangle(enemyScreenX - enemyWidth * 0.4f, y - enemyHeight - enemyHeight * 0.4f,
                                  enemyWidth * 0.8f, enemyHeight * 0.4f, BLACK);
                }
            }
        }

        enemy = enemy->next;
    }
}

void FreeEnemyList(struct EnemyList *list) {
    struct Enemy *current = list->head;

    while (current != NULL) {
        struct Enemy *next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->count = 0;
}
