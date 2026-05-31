#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "enemy.h"
#include "config.h"
#include "track.h"

typedef struct {
    int cols;
    int y;
    int w;
    int h;
} EnemyRow;

// Linhas 0-6: tamanhos da perspectiva (longe -> perto).
// Linhas 7 e 8: mesmo tamanho (mais perto), são os dois quadros de
// animação das rodas, alternados no tempo (não pela distância).
static EnemyRow enemyRows[] = {
    { 1,   0,  10, 10 }, // row 0 - muito distante
    { 7,  26,  10, 10 }, // row 1
    { 7,  44,  18, 10 }, // row 2
    { 7,  62,  26, 10 }, // row 3
    { 7,  80,  26, 18 }, // row 4
    { 7, 106,  34, 18 }, // row 5
    { 5, 132,  42, 26 }, // row 6
    { 5, 166,  50, 34 }, // row 7 - mais perto, quadro de roda A
    { 5, 208,  50, 34 }, // row 8 - mais perto, quadro de roda B
};

#define ENEMY_ROW_WHEEL_A 7
#define ENEMY_ROW_WHEEL_B 8
#define ENEMY_WHEEL_FRAME_SPEED 0.1f  // segundos por quadro
#define ENEMY_SPAWN_ATTEMPTS 12
#define ENEMY_MIN_SAME_LANE_DISTANCE 95.0f
#define ENEMY_MIN_BLOCK_DISTANCE 70.0f

static float GetEnemyLaneX(int faixa) {
    if (faixa < 0){
    return (-1) * 1.0f;
    }
    if (faixa < 0){
    return (0) * 0.7f;
    }
    return 1 * 0.7f;
}

static int GetEnemyLaneFromX(float x) {
    if (x < -0.25f) return 0;
    if (x > 0.25f) return 2;
    return 1;
}

static bool IsEnemySpawnSafe(struct EnemyList *list, float z, int faixa) {
    bool blockedLanes[3] = {false, false, false};
    blockedLanes[faixa] = true;

    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        float distance = fabsf(enemy->z - z);

        if (enemy->faixa == faixa && distance < ENEMY_MIN_SAME_LANE_DISTANCE) {
            return false;
        }

        if (distance < ENEMY_MIN_BLOCK_DISTANCE && enemy->faixa >= 0 && enemy->faixa < 3) {
            blockedLanes[enemy->faixa] = true;
        }

        enemy = enemy->next;
    }

    return !(blockedLanes[0] && blockedLanes[1] && blockedLanes[2]);
}

static int GetEnemyRowIndex(float y) {
    float h = PLAY_HEIGHT - HORIZON;
    float t = (y - HORIZON) / h; // 0.0 = longe, 1.0 = perto
    
    if (t < 0.03f) return 0;
    if (t < 0.07f) return 1;
    if (t < 0.13f) return 2;
    if (t < 0.22f) return 3;
    if (t < 0.35f) return 4;
    if (t < 0.50f) return 5;
    if (t < 0.65f) return 6;
    return ENEMY_ROW_WHEEL_A; // mais perto: quadro escolhido pela animação
}



static int GetEnemyCol(float enemyX, float trackCurve, int maxCols) {
    if (maxCols == 1) return 0;

    // Combina posição lateral do inimigo com a curva da pista.
    float apparentAngle = enemyX - trackCurve * 0.35f;

    int col;

    if (maxCols == 7) {
        if      (apparentAngle < -0.6f) col = 0;
        else if (apparentAngle < -0.35f) col = 1;
        else if (apparentAngle < -0.12f) col = 2;
        else if (apparentAngle >  0.6f) col = 6;
        else if (apparentAngle >  0.35f) col = 5;
        else if (apparentAngle >  0.12f) col = 4;
        else col = 3;
    } else {
        // 5 colunas (rows 6-8, mais perto)
        if      (apparentAngle < -0.5f) col = 0;
        else if (apparentAngle < -0.15f) col = 1;
        else if (apparentAngle >  0.5f) col = 4;
        else if (apparentAngle >  0.15f) col = 3;
        else col = 2;
    }

    if (col < 0) col = 0;
    if (col >= maxCols) col = maxCols - 1;
    return col;
}
void InitEnemyList(struct EnemyList *list) {
    list->head = NULL;
    list->count = 0;
    list->wheelFrame = 0;
    list->wheelTimer = 0.0f;
    Image img = LoadImage("assets/enemy_blue.png");
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    list->texture = LoadTextureFromImage(img);
    SetTextureFilter(list->texture, TEXTURE_FILTER_POINT);
    UnloadImage(img);
}

void SpawnEnemy(struct EnemyList *list, float playerZ) {
    float z = 0.0f;
    int faixa = 0;

    for (int i = 0; i < ENEMY_SPAWN_ATTEMPTS; i++) {
        z = playerZ + 200.0f + (rand() % 450);
        faixa = rand() % 3;

        if (IsEnemySpawnSafe(list, z, faixa)) {
            break;
        }

        if (i == ENEMY_SPAWN_ATTEMPTS - 1) {
            return;
        }
    }

    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));
    if (enemy == NULL) {
        return;
    }

    enemy->z = z;
    enemy->faixa = faixa;
    enemy->x = GetEnemyLaneX(enemy->faixa);
    enemy->speed = 0.05f + ((rand() % 26) / 100.0f);
    enemy->passou = false;
    enemy->next = list->head;
    list->head = enemy;
    list->count++;
}

void SpawnEnemyAt(struct EnemyList *list, float z, float x, float speed) {
    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));
    if (enemy == NULL) {
        return;
    }

    enemy->z = z;
    enemy->faixa = GetEnemyLaneFromX(x);

    if (!IsEnemySpawnSafe(list, enemy->z, enemy->faixa)) {
        free(enemy);
        return;
    }

    enemy->x = GetEnemyLaneX(enemy->faixa);
    enemy->speed = speed;
    enemy->passou = false;

    enemy->next = list->head;
    list->head = enemy;
    list->count++;
}

void UpdateEnemies(struct EnemyList *list, float dt, float playerZ) {
    // avança a animação das rodas (alterna entre as linhas 7 e 8)
    list->wheelTimer += dt;
    if (list->wheelTimer >= ENEMY_WHEEL_FRAME_SPEED) {
        list->wheelTimer = 0.0f;
        list->wheelFrame = (list->wheelFrame + 1) % 2;
    }

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
        int before = list->count;
        SpawnEnemy(list, playerZ);
        if (list->count == before) {
            break; // spawn falhou (malloc), evita laço infinito
        }
    }
}

void CheckPassedEnemies(struct EnemyList *list, float playerZ, int *car_passed, int *car_lost){
    *car_passed = 0;
    *car_lost = 0;

    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        if (!enemy->passou && enemy->z < playerZ - 20.0f) { //conta os inimigos passados
            enemy->passou = true;                          // ele verifica que passou e adiciona
            (*car_passed)++;
        }
        else if (enemy->passou && enemy->z > playerZ + 20.0f) { //serve para inimigos que passaram o jogador
            enemy->passou = false;                             // ele verifica se passou e quarda o valor que vai ser tirado
            (*car_lost)++;
        }
        enemy = enemy->next;
    }
}


void DrawEnemies(struct EnemyList *list, struct Player *player, struct Track *track) {
    struct Enemy *enemy = list->head;
    float cameraTurn = player->x * -100.0f;
    while (enemy != NULL) {
        float deltaZ = enemy->z - player->z;
        if (deltaZ > 0.1f) {
            float y = HORIZON + 800.0f / deltaZ;
            if (y > HORIZON && y < PLAY_HEIGHT) {
                float scale = (y - HORIZON) / HORIZON;
                float curveAmount = 0.0f;
                for (int cy = PLAY_HEIGHT; cy >= (int)y; cy--) {
                    float cscale = (float)(cy - HORIZON) / HORIZON;
                    float cdynamicY = (cy == HORIZON) ? 0.1f : (float)(cy - HORIZON);
                    float cprojectZ = 800.0f / cdynamicY;
                    int ctrackIndex = ((int)(player->z + cprojectZ)) % TRACK_LENGTH;
                    if (cprojectZ < 300.0f) {
                        curveAmount += track->segments[ctrackIndex].curve * (1.0f - cscale) * 0.85f;
                    }
                }
                float enemyScreenX = (SCREEN_WIDTH / 2.0f) +
                                     (enemy->x * (TRACK_BASE_WIDTH / 2.0f) * powf(scale, 1.4f)) +
                                     curveAmount +
                                     cameraTurn * (1.0f - scale);

                int rowIdx = GetEnemyRowIndex(y);
                // na faixa mais próxima, alterna entre os dois quadros de roda;
                // desloca por faixa para os carros não girarem em sincronia
                if (rowIdx == ENEMY_ROW_WHEEL_A) {
                    rowIdx = ((list->wheelFrame + enemy->faixa) % 2)
                                 ? ENEMY_ROW_WHEEL_B : ENEMY_ROW_WHEEL_A;
                }
                EnemyRow row = enemyRows[rowIdx];

                // Busca a curva do segmento na posição Z do inimigo
                int enemyTrackIdx = ((int)enemy->z) % TRACK_LENGTH;
                if (enemyTrackIdx < 0) enemyTrackIdx += TRACK_LENGTH;
                float trackCurve = track->segments[enemyTrackIdx].curve;

                int col = GetEnemyCol(enemy->x, trackCurve, row.cols);

                float roadWidth = TRACK_BASE_WIDTH * powf(scale, 1.2f);
                float destW = roadWidth * VEHICLE_TRACK_WIDTH_RATIO;
                float destH = row.h * (destW / row.w);
                
                Rectangle src = {
                    (float)(col * row.w),
                    (float)(row.y),
                    (float)(row.w),
                    (float)(row.h)
                };
                
                Rectangle dest = {
                    enemyScreenX - destW / 2.0f,
                    y - destH,
                    destW,
                    destH
                };
                
                DrawTexturePro(list->texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
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
    UnloadTexture(list->texture);
}
