#include <math.h>
#include "raylib.h"
#include "track.h"
#include "config.h"


#define BACKGROUND_CURVE_DEADZONE 0.05f
#define BACKGROUND_SCROLL_FORCE 420.0f
#define BACKGROUND_TURN_SWAY 70.0f


static const char *BACKGROUND_FILES[PARALLAX_LAYERS] = {
    "assets/background/8.png",
    "assets/background/7.png",
    "assets/background/6.png",
    "assets/background/4.png",
    "assets/background/3.png",
    "assets/background/2.png",
};

static const float BACKGROUND_PARALLAX[PARALLAX_LAYERS] = {
    0.0f, 0.10f, 0.16f, 0.34f, 0.48f, 0.48f
};

static const float BACKGROUND_SCALE[PARALLAX_LAYERS] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
};

static const float BACKGROUND_Y_OFFSET[PARALLAX_LAYERS] = {
    0.0f, 90.0f, 100.0f, 40.0f, 29.0f, 115.0f
};

static float alisar(float t) {
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

static float progresso(int i, float inicio, float fim) {
    return alisar((i - inicio) / (fim - inicio));
}

void InitTrack(struct Track *track){
    track->backgroundOffset = 0.0f;

    for (int i = 0; i < PARALLAX_LAYERS; i++) {
        track->background[i] = LoadTexture(BACKGROUND_FILES[i]);
        SetTextureFilter(track->background[i], TEXTURE_FILTER_POINT);
    }

    for (int i = 0; i < TRACK_LENGTH; i++) {
        if (i < TRACK_LENGTH/7.5) track->segments[i].curve = 0.0f;
        else if (i < TRACK_LENGTH/5.5){
            float t = progresso(i, TRACK_LENGTH/7.5f, TRACK_LENGTH/5.5f);
            track->segments[i].curve = t * 1.5f;
        }
        else if (i < TRACK_LENGTH/2.7) track->segments[i].curve = 1.5f;
        else if (i < TRACK_LENGTH/2.4){
            float t = progresso(i, TRACK_LENGTH/2.7f, TRACK_LENGTH/2.4f);
            track->segments[i].curve = (1.0f - t) * 1.5f;
        }
        else if (i < TRACK_LENGTH/1.818) track->segments[i].curve = 0.0f;
        else if (i < TRACK_LENGTH/1.666){
            float t = progresso(i, TRACK_LENGTH/1.818f, TRACK_LENGTH/1.666f);
            track->segments[i].curve = t * -1.8f;
        }
        else if (i < TRACK_LENGTH/1.276) track->segments[i].curve = -1.8f;
        else if (i < TRACK_LENGTH/1.2){
            float t = progresso(i, TRACK_LENGTH/1.276f, TRACK_LENGTH/1.2f);
            track->segments[i].curve = (1.0f - t) * -1.8f;
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

    for (int repeticao = 0; repeticao < 8; repeticao++) {
        float curvaSuave[TRACK_LENGTH];

        for (int i = 0; i < TRACK_LENGTH; i++) {
            int anterior = (i - 1 + TRACK_LENGTH) % TRACK_LENGTH;
            int proximo = (i + 1) % TRACK_LENGTH;

            curvaSuave[i] = (
                track->segments[anterior].curve +
                track->segments[i].curve * 2.0f +
                track->segments[proximo].curve
            ) / 4.0f;
        }

        for (int i = 0; i < TRACK_LENGTH; i++) {
            track->segments[i].curve = curvaSuave[i];
        }
    }
}

static float GetBackgroundCurve(struct Track *track, struct Player *player) {
    int trackIndex = ((int)player->z) % TRACK_LENGTH;
    if (trackIndex < 0) trackIndex += TRACK_LENGTH;
    return track->segments[trackIndex].curve;
}

void UpdateTrackParallax(struct Track *track, struct Player *player, float dt) {
    float curve = GetBackgroundCurve(track, player);

    if (curve > -BACKGROUND_CURVE_DEADZONE && curve < BACKGROUND_CURVE_DEADZONE) {
        return;
    }

    track->backgroundOffset += curve * player->speed * dt * BACKGROUND_SCROLL_FORCE;
}

static void DrawParallaxBackground(struct Track *track, struct Player *player) {
    float skyHeight = HORIZON;

    for (int i = 0; i < PARALLAX_LAYERS; i++) {
        Texture2D texture = track->background[i];

        if (texture.id == 0) {
            continue;
        }

        float scale = (skyHeight / texture.height) * BACKGROUND_SCALE[i];
        float destW = texture.width * scale;
        float destH = texture.height * scale;
        float turnOffset = player->x * BACKGROUND_TURN_SWAY;
        float layerOffset = (track->backgroundOffset + turnOffset) * BACKGROUND_PARALLAX[i];
        float x = fmodf(-layerOffset, destW);

        if (x > 0.0f) {
            x -= destW;
        }

        Rectangle src = {0.0f, 0.0f, (float)texture.width, (float)texture.height};

        for (; x < SCREEN_WIDTH; x += destW) {
            Rectangle dest = {x, BACKGROUND_Y_OFFSET[i], destW, destH};
            DrawTexturePro(texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
}

void DrawTrack(struct Track *track,struct Player *player){
    DrawRectangle(0, 0, SCREEN_WIDTH, HORIZON, SKYBLUE);
    DrawParallaxBackground(track, player);
    DrawRectangle(0, HORIZON, SCREEN_WIDTH, HORIZON, (Color){61, 130, 54, 255});

    float curveAmout = 0.0f;
    float cameraTurn = player->x * -100.0f;

    for (int y = PLAY_HEIGHT; y >= HORIZON; y--) {
        float scale = (float)(y - HORIZON) / HORIZON;
        float dynamicY = (y == HORIZON) ? 0.1f: (float)(y - HORIZON);
        float projectZ = 800.0f / dynamicY;

        int trackIndex = ((int)(player->z + projectZ)) % TRACK_LENGTH;

        float targetCurve = track->segments[trackIndex].curve;
        Color colorRoad = track->segments[trackIndex].colorRoad;
        Color colorZebra = track->segments[trackIndex].colorZebra;

        // curva só acumula dentro do alcance de visão
        float maxDraw = 300.0f;
        if (projectZ < maxDraw) {
            curveAmout += targetCurve * (1.0f - scale) * 0.85f;
        }

        float centerX = (SCREEN_WIDTH / 2.0f) + curveAmout + cameraTurn * (1.0f - scale);
        float perspScale = powf(scale, 1.2f);
        float roadWidth = TRACK_BASE_WIDTH * perspScale;
        float zebraWidth = 35.0f * perspScale;

        DrawLine(centerX - roadWidth/2 - zebraWidth, y, centerX - roadWidth/2, y, colorZebra);
        DrawLine(centerX - roadWidth/2, y, centerX + roadWidth/2, y, colorRoad);
        DrawLine(centerX + roadWidth/2, y, centerX + roadWidth/2 + zebraWidth, y, colorZebra);
    }
}

void FreeTrack(struct Track *track) {
    for (int i = 0; i < PARALLAX_LAYERS; i++) {
        if (track->background[i].id != 0) {
            UnloadTexture(track->background[i]);
        }
    }
}
