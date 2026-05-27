#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "collision.h"
#include "enemy.h"
#include "player.h"

static bool ListContainsZ(struct EnemyList *list, float z) {
    struct Enemy *enemy = list->head;

    while (enemy != NULL) {
        if (enemy->z == z) {
            return true;
        }

        enemy = enemy->next;
    }

    return false;
}

static void TestSpawnAndFree(void) {
    struct EnemyList list;
    InitEnemyList(&list);

    SpawnEnemyAt(&list, 100.0f, 0.0f, 0.1f, BLUE);
    SpawnEnemyAt(&list, 150.0f, -0.5f, 0.1f, RED);

    assert(list.count == 2);
    assert(list.head != NULL);
    assert(list.head->passou == false);

    FreeEnemyList(&list);
    assert(list.count == 0);
    assert(list.head == NULL);
}

static void TestPassedEnemies(void) {
    struct EnemyList list;
    InitEnemyList(&list);

    SpawnEnemyAt(&list, 10.0f, 0.0f, 0.1f, BLUE);
    SpawnEnemyAt(&list, 200.0f, 0.0f, 0.1f, RED);

    assert(CountPassedEnemies(&list, 40.0f) == 1);
    assert(CountPassedEnemies(&list, 40.0f) == 0);

    FreeEnemyList(&list);
}

static void TestRemoveOldEnemies(void) {
    struct EnemyList list;
    InitEnemyList(&list);

    SpawnEnemyAt(&list, -500.0f, 0.0f, 0.1f, BLUE);
    assert(ListContainsZ(&list, -500.0f));

    UpdateEnemies(&list, 0.0f, 0.0f);

    assert(!ListContainsZ(&list, -500.0f));
    assert(list.count == 25);

    FreeEnemyList(&list);
}

static void TestCollision(void) {
    struct Player player;
    InitPlayer(&player);
    player.x = 0.0f;
    player.z = 0.0f;

    struct Enemy hit = {
        .z = 3.2f,
        .x = 0.0f,
        .speed = 0.1f,
        .faixa = 1,
        .passou = false,
        .color = BLUE,
        .next = NULL,
    };

    struct Enemy miss = hit;
    miss.x = 0.8f;

    assert(CheckEnemyCollision(&player, &hit));
    assert(!CheckEnemyCollision(&player, &miss));
}

int main(void) {
    TestSpawnAndFree();
    TestPassedEnemies();
    TestRemoveOldEnemies();
    TestCollision();

    printf("test_enemies: ok\n");
    return 0;
}
