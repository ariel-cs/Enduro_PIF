#include "raylib.h"
#include "player.h"
#include "config.h"

void InitPlayer(struct Player *player){
    player->z = 0.0f;
    player->x = 0.0f;
    player->speed = 0.0f;
    player->maxSpeed = 1.0f;
    player->acele = 0.002f;
    player->decele = 0.005f;
    player->friction = 0.0002f;
}

void UpdatePlayer(struct Player *player, float curva){
    if (IsKeyDown(KEY_SPACE)) {
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
    if (player->speed > 0.0f) {
        float acele_lateral=0.0f+(player->speed)*0.03;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player->x -= acele_lateral;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player->x += acele_lateral;
        if (curva > -0.05f && curva < 0.05f) curva = 0.0f;

        float curvando= curva * player->speed * 0.005f;
        player->x -= curvando;
    }


    if(player->x < -0.77f) player->x = -0.77f;
    if(player->x > 0.77f) player->x = 0.77f;
}

void DrawPlayer(struct Player *player){
    float playerScreenX = (SCREEN_WIDTH / 2.0f) + (player->x * 300.0f);

    DrawRectangle(playerScreenX - 25, SCREEN_HEIGHT - 70, 50, 35, RED);
    DrawRectangle(playerScreenX - 20, SCREEN_HEIGHT - 85, 40, 15, BLACK);
}
