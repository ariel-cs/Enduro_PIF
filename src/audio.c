#include <raylib.h>
#include "audio.h"
#include "game.h"

#define ENGINE_LOOP "assets/sfx/engine_loop.wav"
#define WIND_LOOP   "assets/sfx/wind_loop.wav"
#define CRASH_SFX   "assets/sfx/crash.wav"
#define MENU_MUSIC  "assets/sfx/menu.wav"
#define PLAY_MUSIC  "assets/sfx/play.wav"
#define SCORE_MUSIC "assets/sfx/score.wav"
#define TICK_SFX    "assets/sfx/321.wav"
#define GO_SFX      "assets/sfx/go.wav"

#define MUSIC_NONE  0
#define MUSIC_MENU  1
#define MUSIC_PLAY  2
#define MUSIC_SCORE 3

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void InitAudio(AudioEngine *audio) {
    audio->masterVolume = 1.0f;
    audio->currentMusic = MUSIC_NONE;

    // Carrega como sons estáticos na RAM.
    audio->engine = LoadSound(ENGINE_LOOP);
    audio->wind   = LoadSound(WIND_LOOP);
    audio->crash  = LoadSound(CRASH_SFX);
    audio->menuMusic = LoadSound(MENU_MUSIC);
    audio->playMusic = LoadSound(PLAY_MUSIC);
    audio->scoreMusic = LoadSound(SCORE_MUSIC);
    audio->countdownTick = LoadSound(TICK_SFX);
    audio->countdownGo = LoadSound(GO_SFX);
    SetSoundVolume(audio->engine, 0.0f);
    SetSoundVolume(audio->wind,   0.0f);

    PlaySound(audio->engine);
    PlaySound(audio->wind);
}

void UpdateAudio(AudioEngine *audio, float ratio, float gain) {
    ratio = clampf(ratio, 0.0f, 1.0f);
    gain *= audio->masterVolume; // volume mestre escolhido no menu

    // REPLAYYYYYYY
    if (!IsSoundPlaying(audio->engine)) PlaySound(audio->engine);
    if (!IsSoundPlaying(audio->wind))   PlaySound(audio->wind);

    // Motor: pitch sobe com a velocidade.
    SetSoundPitch (audio->engine, 0.85f + 0.70f * ratio);
    SetSoundVolume(audio->engine, clampf(gain * (0.45f + 0.40f * ratio), 0.0f, 1.0f));

    // Vento: ruido de banda larga.
    SetSoundPitch (audio->wind, 0.9f + 0.6f * ratio);
    SetSoundVolume(audio->wind, clampf(gain * 0.15f * ratio * ratio, 0.0f, 1.0f));
}

static void StopMusic(AudioEngine *audio) {
    StopSound(audio->menuMusic);
    StopSound(audio->playMusic);
    StopSound(audio->scoreMusic);
}

static void PlayMusic(AudioEngine *audio, int music) {
    if (audio->currentMusic == music) {
        if (music == MUSIC_MENU && !IsSoundPlaying(audio->menuMusic)) PlaySound(audio->menuMusic);
        if (music == MUSIC_PLAY && !IsSoundPlaying(audio->playMusic)) PlaySound(audio->playMusic);
        if (music == MUSIC_SCORE && !IsSoundPlaying(audio->scoreMusic)) PlaySound(audio->scoreMusic);
    } else {
        StopMusic(audio);
        audio->currentMusic = music;

        if (music == MUSIC_MENU) PlaySound(audio->menuMusic);
        if (music == MUSIC_PLAY) PlaySound(audio->playMusic);
        if (music == MUSIC_SCORE) PlaySound(audio->scoreMusic);
    }

    SetSoundVolume(audio->menuMusic, clampf(audio->masterVolume * 0.35f, 0.0f, 1.0f));
    SetSoundVolume(audio->playMusic, clampf(audio->masterVolume * 0.30f, 0.0f, 1.0f));
    SetSoundVolume(audio->scoreMusic, clampf(audio->masterVolume * 0.35f, 0.0f, 1.0f));
}

void UpdateMusicForState(AudioEngine *audio, int state) {
    if (state == STATE_TITLE || state == STATE_MENU) PlayMusic(audio, MUSIC_MENU);
    else if (state == STATE_COUNTDOWN || state == STATE_PLAYING) PlayMusic(audio, MUSIC_PLAY);
    else if (state == STATE_GAME_OVER || state == STATE_TOP_SCORES) PlayMusic(audio, MUSIC_SCORE);
    else {
        StopMusic(audio);
        audio->currentMusic = MUSIC_NONE;
    }
}

// Toca o som de batida quando o jogador acerta a traseira de um carro.
void PlayCrashSound(AudioEngine *audio) {
    SetSoundVolume(audio->crash, clampf(audio->masterVolume, 0.0f, 1.0f));
    PlaySound(audio->crash);
}

void PlayMenuSound(AudioEngine *audio) {
    SetSoundVolume(audio->countdownTick, clampf(audio->masterVolume * 0.7f, 0.0f, 1.0f));
    PlaySound(audio->countdownTick);
}

void PlayCountdownTick(AudioEngine *audio) {
    SetSoundVolume(audio->countdownTick, clampf(audio->masterVolume * 0.7f, 0.0f, 1.0f));
    PlaySound(audio->countdownTick);
}

void PlayCountdownGo(AudioEngine *audio) {
    SetSoundVolume(audio->countdownGo, clampf(audio->masterVolume, 0.0f, 1.0f));
    PlaySound(audio->countdownGo);
}

void UnloadAudio(AudioEngine *audio) {
    StopSound(audio->engine);
    StopSound(audio->wind);
    StopSound(audio->crash);
    StopMusic(audio);
    UnloadSound(audio->engine);
    UnloadSound(audio->wind);
    UnloadSound(audio->crash);
    UnloadSound(audio->menuMusic);
    UnloadSound(audio->playMusic);
    UnloadSound(audio->scoreMusic);
    UnloadSound(audio->countdownTick);
    UnloadSound(audio->countdownGo);
}
