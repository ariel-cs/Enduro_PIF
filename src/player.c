#include "raylib.h"
#include "player.h"
#include "config.h"
#include <stdbool.h>

void InitPlayer(struct Player *player){
    player->z = 0.0f;
    player->x = 0.0f;
    player->speed = 0.0f;
    player->maxSpeed = 1.0f;
    player->acele = 0.002f;
    player->decele = 0.005f;
    player->friction = 0.0002f;
    player->currentFrame = 0;
    player->frameTimer = 0.0f;
    player->frameSpeed = 0.1f;

    Image img = LoadImage("assets/player_sheet.png");
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageColorReplace(&img, PURPLE, BLANK);
    ImageResizeNN(&img, 68 * 5, 60 * 2);
    player->texture = LoadTextureFromImage(img);
    SetTextureFilter(player->texture, TEXTURE_FILTER_POINT);
    UnloadImage(img);
}

static int GetPlayerColumn(struct Player *player){
    bool goingRight  = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
    bool goingLeft = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);

    if (!goingLeft && !goingRight) return 2; // reto

    float abs_x = player->x < 0 ? -player->x : player->x;

    if (goingLeft) {
        return abs_x > 0.4f ? 0 : 1; // esq total ou esq início
    } else {
        return abs_x > 0.4f ? 4 : 3; // dir total ou dir início
    }
}

void UpdatePlayer(struct Player *player, float curva){
    if (IsKeyDown(KEY_SPACE)) {
        player->speed += player->acele;
        if (player->speed > player->maxSpeed) player->speed = player->maxSpeed;
    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->speed -= player->decele;
        if (player->speed < 0.0f) player->speed = 0.0f;
    } else {
        player->speed -= player->friction;
        if (player->speed < 0.0f) player->speed = 0.0f;
    }

    player->z += player->speed;

    if (player->speed > 0.0f) {
        float acele_lateral = player->speed * 0.03f;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player->x -= acele_lateral;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player->x += acele_lateral;
        if (curva > -0.05f && curva < 0.05f) curva = 0.0f;

        float curvando= curva * player->speed * 0.005f;
        player->x -= curvando;
    }

    if (player->x < -0.77f) player->x = -0.77f;
    if (player->x >  0.77f) player->x =  0.77f;

    // avança frame só em movimento
    if (player->speed > 0.0f) {
        player->frameTimer += GetFrameTime();
        if (player->frameTimer >= player->frameSpeed) {
            player->frameTimer = 0.0f;
            player->currentFrame = (player->currentFrame + 1) % 2;
        }
    } else {
        player->currentFrame = 0;
        player->frameTimer   = 0.0f;
    }
}

void DrawPlayer(struct Player *player){
    int frameW = player->texture.width  / 5;
    int frameH = player->texture.height / 2;
    float destW = VEHICLE_BASE_WIDTH;
    float destH = frameH * (destW / frameW);

    int col = GetPlayerColumn(player);

    Rectangle src = {
        (float)(col * frameW),
        (float)(player->currentFrame * frameH), // linha 0 ou 1
        (float)frameW,
        (float)frameH
    };

    Rectangle dest = {
        (SCREEN_WIDTH  / 2.0f) - (destW / 2.0f) + (player->x * (TRACK_BASE_WIDTH / 2.0f)),
        SCREEN_HEIGHT - 10.0f - destH,
        destW,
        destH
    };

    DrawTexturePro(player->texture, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}
