#include "raylib.h"
#include "track.h"
#include "config.h"

void InitTrack(struct Track *track){
    for (int i = 0; i < TRACK_LENGTH; i++) {

        if (i < TRACK_LENGTH/7.5) track->segments[i].curve = 0.0f;
        else if (i < TRACK_LENGTH/5.5){
            float t = (float)(i - TRACK_LENGTH/7.5) / 300.0f;
            track->segments[i].curve = t * 1.5f;
        }
        else if (i < TRACK_LENGTH/2.7) track->segments[i].curve = 1.5f;
        else if (i < TRACK_LENGTH/2.4){
            float t = (float)(TRACK_LENGTH/2.4 - i) / 300.0f;
            track->segments[i].curve = t * 1.5f;
        }
        else if (i < TRACK_LENGTH/1.8) track->segments[i].curve = 0.0f;
        else if (i < TRACK_LENGTH/1.6){
            float t = (float)(i - TRACK_LENGTH/1.8) / 300.0f;
            track->segments[i].curve = t * -1.8f;
        }
        else if (i < TRACK_LENGTH/1.28) track->segments[i].curve = -1.8f;
        else if (i < TRACK_LENGTH/1.2){
            float t = (float)(TRACK_LENGTH/1.2 - i);
            track->segments[i].curve = t * -1.8f;
        }
        else track->segments[i].curve = 0.0f;

        int segmentGroup = i / 2;
        if (segmentGroup % 2 == 0){
            track->segments[i].colorRoad = GRAY;
            track->segments[i].colorZebra = RED;
        }
        else{
            track->segments[i].colorRoad = DARKGRAY;
            track->segments[i].colorZebra = WHITE;
        }
    }
}

void DrawTrack(struct Track *track,struct Player *player){
    DrawRectangle(0, 0, SCREEN_WIDTH, HORIZON, SKYBLUE);
    DrawRectangle(0, HORIZON, SCREEN_WIDTH, HORIZON, GREEN);

    float curveAmout = 0.0f;

    for (int y = SCREEN_HEIGHT; y >= HORIZON; y--) {
        float scale = (float)(y - HORIZON) / HORIZON;
        float dynamicY = (y == HORIZON) ? 0.1f: (float)(y - HORIZON);
        float projectZ = 800.0f / dynamicY;

        int trackIndex = ((int)(player->z + projectZ)) % TRACK_LENGTH;

        float targetCurve = track->segments[trackIndex].curve;
        Color colorRoad = track->segments[trackIndex].colorRoad;
        Color colorZebra = track->segments[trackIndex].colorZebra;

        curveAmout += targetCurve * (1.0f - scale) * 0.85f;

        float centerX = (SCREEN_WIDTH / 2.0f) + curveAmout;
        float roadWidth = 600.0f * scale;
        float zebraWidth = 35.0f * scale;

        DrawLine(centerX - roadWidth/2 - zebraWidth, y, centerX - roadWidth/2, y, colorZebra);
        DrawLine(centerX - roadWidth/2, y, centerX + roadWidth/2, y, colorRoad);
        DrawLine(centerX + roadWidth/2, y, centerX + roadWidth/2 + zebraWidth, y, colorZebra);
    }
}
