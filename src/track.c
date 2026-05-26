#include "raylib.h"
#include "track.h"
#include "config.h"

float GetTrackCurve(float z){
    int loopZ = (int)z %6000;
    if (loopZ < 1000) return 0.0f;
    if (loopZ < 2500) return 1.5f;
    if (loopZ < 3500) return 0.0f;
    if (loopZ < 5000) return -1.8f;
    return 0.0f;
}

void DrawTrack(struct Player *player){
    DrawRectangle(0, 0, SCREEN_WIDTH, HORIZON, SKYBLUE);
    DrawRectangle(0, HORIZON, SCREEN_WIDTH, HORIZON, GREEN);

    float curveAmout = 0.0f;

    for (int y = SCREEN_HEIGHT; y >= HORIZON; y--) {
        float scale = (float)(y - HORIZON) / HORIZON;
        float dynamicY = (y == HORIZON) ? 0.1f: (float)(y - HORIZON);
        float projectZ = 250.0f / dynamicY;

        float targetCurve = GetTrackCurve(player->z + projectZ);

        curveAmout += targetCurve * (1.0f - scale) * 0.85f;

        float centerX = (SCREEN_WIDTH / 2.0f) + curveAmout;
        float roadWidth = 600.0f * scale;
        float zebraWidth = 35.0f * scale;

        int segment = (int)(projectZ + player->z * 0.5f);
        Color colorRoad = (segment % 2 == 0) ? GRAY : DARKGRAY;
        Color colorZebra = (segment % 2 == 0) ? RED : WHITE;

        DrawLine(centerX - roadWidth/2 - zebraWidth, y, centerX - roadWidth/2, y, colorZebra);
        DrawLine(centerX - roadWidth/2, y, centerX + roadWidth/2, y, colorRoad);
        DrawLine(centerX + roadWidth/2, y, centerX + roadWidth/2 + zebraWidth, y, colorZebra);
    }
}
