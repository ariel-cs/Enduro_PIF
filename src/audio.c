#include <raylib.h>
#include "audio.h"

#define ENGINE_LOOP "assets/sfx/engine_loop.wav"
#define WIND_LOOP   "assets/sfx/wind_loop.wav"

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void InitAudio(AudioEngine *audio) {
    audio->masterVolume = 1.0f;

    // Carrega como sons estáticos na RAM.
    audio->engine = LoadSound(ENGINE_LOOP);
    audio->wind   = LoadSound(WIND_LOOP);
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

void UnloadAudio(AudioEngine *audio) {
    StopSound(audio->engine);
    StopSound(audio->wind);
    UnloadSound(audio->engine);
    UnloadSound(audio->wind);
}
