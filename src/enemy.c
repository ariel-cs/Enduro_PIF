#include <stdlib.h>
#include <math.h>
#include "enemy.h"
#include "config.h"
#include "track.h"
// fator de escala aplicado aos sprites da sheet
#define ENEMY_SCALE 1.15f

typedef struct {
    int cols;
    int y;
    int w;
    int h;
} EnemyRow;

static EnemyRow enemyRows[] = {
    { 1,   0,  10, 10 }, // row 0 - muito distante
    { 7,  26,  10, 10 }, // row 1
    { 7,  44,  18, 10 }, // row 2
    { 7,  62,  26, 10 }, // row 3
    { 7,  80,  26, 18 }, // row 4
    { 7, 106,  34, 18 }, // row 5
    { 5, 132,  42, 26 }, // row 6
    { 5, 166,  50, 34 }, // row 7
    { 5, 208,  50, 34 }, // row 8 - mais perto
};

static int GetEnemyRowIndex(float y) {
    float h = SCREEN_HEIGHT - HORIZON;
    float t = (y - HORIZON) / h; // 0.0 = longe, 1.0 = perto
    
    if (t < 0.03f) return 0;
    if (t < 0.07f) return 1;
    if (t < 0.13f) return 2;
    if (t < 0.22f) return 3;
    if (t < 0.35f) return 4;
    if (t < 0.50f) return 5;
    if (t < 0.65f) return 6;
    if (t < 0.80f) return 7;
    return 8; // últimos 20% da tela

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
    Image img = LoadImage("assets/enemy_blue.png");
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    list->texture = LoadTextureFromImage(img);
    UnloadImage(img);
}

void SpawnEnemy(struct EnemyList *list, float playerZ) {
    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));

    enemy->z = playerZ + 200.0f + (rand() % 300);
    enemy->faixa = rand() % 3;
    enemy->x = (enemy->faixa - 1) * 0.5f;
    enemy->speed = 0.05f + ((rand() % 26) / 100.0f);
    enemy->passou = false;
    enemy->next = list->head;
    list->head = enemy;
    list->count++;
}

void SpawnEnemyAt(struct EnemyList *list, float z, float x, float speed) {
    struct Enemy *enemy = (struct Enemy *)malloc(sizeof(struct Enemy));

    enemy->z = z;
    enemy->x = x;
    enemy->speed = speed;
    enemy->passou = false;

    if (x < -0.25f) {
        enemy->faixa = 0;
    } else if (x > 0.25f) {
        enemy->faixa = 2;
    } else {
        enemy->faixa = 1;
    }

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

int CountPassedEnemies(struct EnemyList *list, float playerZ) {
    int passed = 0;
    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        if (!enemy->passou && enemy->z < playerZ - 20.0f) {
            enemy->passou = true;
            passed++;
        }

        enemy = enemy->next;
    }

    return passed;
}


void DrawEnemies(struct EnemyList *list, struct Player *player, struct Track *track) {
    struct Enemy *enemy = list->head;
    float cameraTurn = player->x * -100.0f;
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
                    if (cprojectZ < 300.0f) {
                        curveAmount += track->segments[ctrackIndex].curve * (1.0f - cscale) * 0.85f;
                    }
                }
                float enemyScreenX = (SCREEN_WIDTH / 2.0f) +
                                     (enemy->x * 300.0f * powf(scale, 1.4f)) +
                                     curveAmount +
                                     cameraTurn * (1.0f - scale);

                int rowIdx = GetEnemyRowIndex(y);
                EnemyRow row = enemyRows[rowIdx];

                // Busca a curva do segmento na posição Z do inimigo
                int enemyTrackIdx = ((int)enemy->z) % TRACK_LENGTH;
                if (enemyTrackIdx < 0) enemyTrackIdx += TRACK_LENGTH;
                float trackCurve = track->segments[enemyTrackIdx].curve;

                int col = GetEnemyCol(enemy->x, trackCurve, row.cols);

                float destW = row.w * ENEMY_SCALE;
                float destH = row.h * ENEMY_SCALE;
                
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
