#ifndef PLAYER_H
#define PLAYER_H

struct Player {
    float z;
    float x;
    float speed;
    float maxSpeed;
    float acele;
    float decele;
    float friction;
};

void InitPlayer(struct Player *player);
void UpdatePlayer(struct Player *player, float curva);
void DrawPlayer(struct Player *player);

#endif
