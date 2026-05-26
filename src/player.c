#include "raylib.h"
#include "player.h"
#include "config.h"

void InitPlayer(struct Player *player){
    player->z = 0.0f;
    player->x = 0.0f;
    player->speed = 0.0f;
    player->maxSpeed = 4.0f;
    player->acele = 0.05f;
    player->decele = 0.08f;
    player->friction = 0.02f;
}

void UpdatePlayer(struct Player *player, float currentCurve){
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->speed += player->acele;
        if (player->speed > player->maxSpeed) player->speed = player->maxSpeed;
    }
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->speed -= player->decele;
        if (player->speed < 0.0f) player->speed = 0.0f;
    }
    else {
        player->speed -= player->friction;
        if (player->speed < 0.0f) player->speed = 0.0f;
    }

    player->z += player->speed;
    if (player->speed > 0.1f) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player->x -= 0.4f;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player->x += 0.4f;
    }

    player->x -= currentCurve * (player->speed / player->maxSpeed) * 0.02f;

    if(player->x < -2.0f) player->x = -2.0f;
    if(player->x > 2.0f) player->x = 2.0f;
}

void DrawPlayer(void){
    DrawRectangle(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 70, 50, 35, RED);
    DrawRectangle(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT - 85, 40, 15, BLACK);
}