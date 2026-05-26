#ifndef TRACK_H
#define TRACK_H

#include "player.h"

struct Track{
    int current_segment;
};

float GetTrackCurve(float z);
void DrawTrack(struct Player *player);

#endif
