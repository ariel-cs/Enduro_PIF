#ifndef CONFIG_H //impede duplicidade de leitura

#define CONFIG_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define HUD_HEIGHT 140                          // faixa do painel na base (estilo Enduro)
#define PLAY_HEIGHT (SCREEN_HEIGHT - HUD_HEIGHT) // 460 - área de corrida (pista)
#define HORIZON (PLAY_HEIGHT / 2)                // 230 - mantém HORIZON = PLAY_HEIGHT/2

#define TRACK_BASE_WIDTH 700.0f
#define VEHICLE_TRACK_WIDTH_RATIO (1.0f / 4.0f)
#define VEHICLE_BASE_WIDTH (TRACK_BASE_WIDTH * VEHICLE_TRACK_WIDTH_RATIO)

#define DEBUG_HITBOXES 0

#define DAY_DURATION 100.0f

#endif
