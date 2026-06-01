#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>

// struct AudioEngine pra controlar o audio do jogo
typedef struct {
    Sound engine;
    Sound wind;
    Sound crash;
    Sound menuMusic;
    Sound playMusic;
    Sound scoreMusic;
    Sound countdownTick;
    Sound countdownGo;

    float masterVolume;
    int currentMusic;
} AudioEngine;

// Estados de Som.
void InitAudio(AudioEngine *audio);
void UpdateAudio(AudioEngine *audio, float ratio, float gain);
void UpdateMusicForState(AudioEngine *audio, int state);
void PlayCrashSound(AudioEngine *audio); // batida na traseira de outro carro
void PlayMenuSound(AudioEngine *audio);
void PlayCountdownTick(AudioEngine *audio);
void PlayCountdownGo(AudioEngine *audio);
void UnloadAudio(AudioEngine *audio);

#endif
