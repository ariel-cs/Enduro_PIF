#include "raylib.h"
#include "config.h"
#include "game.h"
#include "player.h"
#include "track.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "audio.h"
#include "ui.h"
#include <stdbool.h>

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Enduro");
  InitAudioDevice();
  SetTargetFPS(60);

  AudioEngine audio;
  InitAudio(&audio);

  Image img = LoadImage("assets/LOGO.png");
  ImageResize(&img, 294, 110);
  Texture2D logotex = LoadTextureFromImage(img);
  UnloadImage(img);
  SetTextureFilter(logotex, TEXTURE_FILTER_POINT);

  Image menuImg = LoadImage("assets/Enduro_metade.png");
  Texture2D menuImgtex = LoadTextureFromImage(menuImg);
  UnloadImage(menuImg);

  Image titleImg = LoadImage("assets/Enduro_inteira.png");
  ImageResize(&titleImg, 800, 600);
  Texture2D titleImgtex = LoadTextureFromImage(titleImg);
  UnloadImage(titleImg);

  int menuSelectdOption = 0;
  GameState *game = init_game();
  if (!game) {
    UnloadTexture(titleImgtex);
    UnloadTexture(menuImgtex);
    UnloadTexture(logotex);
    CloseWindow();
    return 1;
  }

  ScoreEntry top_scores[5];
  int top_scores_count = 0;
  bool scores_loaded = false;
  bool wasCrashing = false;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if (game->current_state == STATE_LOGO && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_TITLE);
    }
    else if (game->current_state == STATE_TITLE && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_MENU);
    }
    else if (game->current_state == STATE_MENU) {
        if (IsKeyPressed(KEY_DOWN)) menuSelectdOption = (menuSelectdOption + 1) % 3;
        if (IsKeyPressed(KEY_UP))   menuSelectdOption = (menuSelectdOption + 2) % 3;

        // Volume
        if (menuSelectdOption == 1) {
            if (IsKeyPressed(KEY_RIGHT)) audio.masterVolume += 0.1f;
            if (IsKeyPressed(KEY_LEFT))  audio.masterVolume -= 0.1f;
            if (audio.masterVolume > 1.0f) audio.masterVolume = 1.0f;
            if (audio.masterVolume < 0.0f) audio.masterVolume = 0.0f;
        }

        if (IsKeyPressed(KEY_ENTER)){
            if (menuSelectdOption == 0){

                game->day_timer = DAY_DURATION;
                game->day = 1;
                game->cars_to_pass = 200;
                game->cars_passed_today = 0;
                game->score = 0;

                change_state(game, STATE_PLAYING);
            }
            else if (menuSelectdOption == 2) break;
        }
    }

    update_game(game, dt);

    // SFX de batida na traseira: só dispara quando a colisão começa.
    if (game->crashed_this_frame && !wasCrashing) {
        PlayCrashSound(&audio);
    }
    wasCrashing = game->crashed_this_frame;

    float speedRatio = game->player->speed / game->player->maxSpeed;
    float audioGain = (game->current_state == STATE_PLAYING) ? 1.0f : 0.0f;
    // Prévia audível enquanto ajusta o volume no menu (meia aceleração).
    if (game->current_state == STATE_MENU && menuSelectdOption == 1) {
        speedRatio = 0.5f;
        audioGain = 1.0f;
    }
    UpdateAudio(&audio, speedRatio, audioGain);

    // Lazy loading do ranking: recarrega ao sair do game over.
    if (game->current_state == STATE_GAME_OVER) {
        scores_loaded = false;
    }
    else if (game->current_state == STATE_TOP_SCORES && !scores_loaded) {
        top_scores_count = LoadTopScores(top_scores, 5);
        scores_loaded = true;
    }

    BeginDrawing();
    switch (game->current_state) {
        case STATE_LOGO:
            DrawLogoScreen(logotex);
            break;
        case STATE_TITLE:
            DrawTitleScreen(titleImgtex);
            break;
        case STATE_MENU:
            DrawMenu(menuImgtex, menuSelectdOption, audio.masterVolume);
            break;
        case STATE_PLAYING:
            ClearBackground(RAYWHITE);
            DrawTrack(game->track, game->player);
            DrawPlayer(game->player);
            DrawEnemies(game->enemies, game->player, game->track);
#if DEBUG_HITBOXES
            DrawHitboxes(game->player, game->enemies, game->track);
#endif
            DrawHUD(game);
            break;
        case STATE_GAME_OVER:
            DrawGameOverScreen(game);
            break;
        case STATE_TOP_SCORES:
            DrawTopScoresScreen(top_scores, top_scores_count);
            break;
    }
    EndDrawing();
  }

  UnloadTexture(titleImgtex);
  UnloadTexture(menuImgtex);
  UnloadTexture(logotex);
  UnloadTexture(game->player->texture);
  free_game(game);
  UnloadAudio(&audio);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
