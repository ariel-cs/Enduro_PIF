#ifndef TRACK_H
#define TRACK_H

#include "raylib.h"
#include "player.h"

#define TRACK_LENGTH 1000
#define PARALLAX_LAYERS 5

typedef struct{
    float curve;
    Color colorRoad;
    Color colorZebra;
}TrackSegment;

struct Track{
    TrackSegment segments[TRACK_LENGTH];
    Texture2D background[PARALLAX_LAYERS];
    float backgroundOffset;
};

void InitTrack(struct Track *track);
void UpdateTrackParallax(struct Track *track, struct Player *player, float dt);
void DrawTrack(struct Track *track,struct Player *player);
void FreeTrack(struct Track *track);

#endif
