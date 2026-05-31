#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>

// struct AudioEngine pra controlar o audio do jogo
typedef struct {
    Sound engine;
    Sound wind;

    float masterVolume;
} AudioEngine;

// Estados de Som.
void InitAudio(AudioEngine *audio);
void UpdateAudio(AudioEngine *audio, float ratio, float gain);
void UnloadAudio(AudioEngine *audio);

#endif
