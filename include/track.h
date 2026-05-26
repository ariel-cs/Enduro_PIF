#ifndef TRACK_H
#define TRACK_H

#include "raylib.h"
#include "player.h"

#define TRACK_LENGTH 6000

typedef struct{
    float curve;
    Color colorRoad;
    Color colorZebra;
}TrackSegment;

struct Track{
    TrackSegment segments[TRACK_LENGTH];
};

void InitTrack(struct Track *track);
void DrawTrack(struct Track *track,struct Player *player);

#endif
