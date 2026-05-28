#ifndef PLAYER_H
#define PLAYER_H
#include <raylib.h>

struct Player {
    float z;
    float x;
    float speed;
    float maxSpeed;
    float acele;
    float decele;
    float friction;
    Texture2D texture;
    int   currentFrame;
    float frameTimer;
    float frameSpeed;
};



void InitPlayer(struct Player *player);
void UpdatePlayer(struct Player *player);
void DrawPlayer(struct Player *player);

#endif
