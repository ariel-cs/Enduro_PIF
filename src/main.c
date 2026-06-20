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
#include <math.h>

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Enduro");
  InitAudioDevice();
  SetTargetFPS(60);
  int monitor = GetCurrentMonitor();
  int monitorWidth = GetMonitorWidth(monitor);
  int monitorHeight = GetMonitorHeight(monitor);

  SetWindowSize(monitorWidth, monitorHeight);
  ToggleFullscreen();

  RenderTexture2D gameTarget =
    LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  SetTextureFilter(gameTarget.texture, TEXTURE_FILTER_BILINEAR);

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
    UnloadRenderTexture(gameTarget);
    CloseAudioDevice();
    CloseWindow();
    return 1;
  }

  ScoreEntry top_scores[5];
  int top_scores_count = 0;
  bool scores_loaded = false;
  bool wasCrashing = false;
  int lastCountdownValue = -1;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if (game->current_state == STATE_LOGO && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_TITLE);
    }
    else if (game->current_state == STATE_TITLE && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_MENU);
    }
    else if (game->current_state == STATE_MENU) {
        if (IsKeyPressed(KEY_DOWN)) {
            menuSelectdOption = (menuSelectdOption + 1) % 4;
            PlayMenuSound(&audio);
        }
        if (IsKeyPressed(KEY_UP)) {
            menuSelectdOption = (menuSelectdOption + 3) % 4;
            PlayMenuSound(&audio);
        }

        // Volume
        if (menuSelectdOption == 1) {
            if (IsKeyPressed(KEY_RIGHT)) audio.masterVolume += 0.1f;
            if (IsKeyPressed(KEY_LEFT))  audio.masterVolume -= 0.1f;
            if (audio.masterVolume > 1.0f) audio.masterVolume = 1.0f;
            if (audio.masterVolume < 0.0f) audio.masterVolume = 0.0f;
        }

        if (IsKeyPressed(KEY_ENTER)){
            PlayMenuSound(&audio);
            if (menuSelectdOption == 0){

                reset_game(game);

                change_state(game, STATE_COUNTDOWN);
                lastCountdownValue = game->countdown_value;
                PlayCountdownTick(&audio);
            }
            else if (menuSelectdOption == 2){
                change_state(game, STATE_TOP_SCORES);
            }
            else if (menuSelectdOption == 3) break;
        }
    }
    else if (game->current_state == STATE_TOP_SCORES) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE)) {
            change_state(game, STATE_MENU);
        }
    }

    update_game(game, dt);
    UpdateMusicForState(&audio, game->current_state);

    if (game->current_state == STATE_COUNTDOWN && game->countdown_value != lastCountdownValue) {
        lastCountdownValue = game->countdown_value;

        if (game->countdown_value > 0) PlayCountdownTick(&audio);
        else PlayCountdownGo(&audio);
    }

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

    // Lazy loading do ranking: recarrega ao sair do game over ou ao voltar ao menu,
    // garantindo que a tela de recordes mostre sempre a lista atualizada.
    if (game->current_state == STATE_GAME_OVER || game->current_state == STATE_MENU) {
        scores_loaded = false;
    }
    else if (game->current_state == STATE_TOP_SCORES && !scores_loaded) {
        top_scores_count = LoadTopScores(top_scores, 5);
        scores_loaded = true;
    }

    // O jogo continua sendo desenhado em 800x600. Depois, essa imagem e
    // ampliada mantendo a proporcao original, sem deformar os graficos.
    BeginTextureMode(gameTarget);
    ClearBackground(BLACK);
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
        case STATE_COUNTDOWN:
            ClearBackground(RAYWHITE);
            DrawTrack(game->track, game->player);
            DrawPlayer(game->player);
            DrawEnemies(game->enemies, game->player, game->track);
            DrawHUD(game);
            if (game->countdown_value > 0) {
                DrawText(TextFormat("%d", game->countdown_value), SCREEN_WIDTH / 2 - 24, PLAY_HEIGHT / 2 - 70, 96, YELLOW);
            } else {
                DrawText("GO!", SCREEN_WIDTH / 2 - 80, PLAY_HEIGHT / 2 - 70, 80, GREEN);
            }
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
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    float scaleX = (float)GetScreenWidth() / SCREEN_WIDTH;
    float scaleY = (float)GetScreenHeight() / SCREEN_HEIGHT;
    float scale = fminf(scaleX, scaleY);
    float drawWidth = SCREEN_WIDTH * scale;
    float drawHeight = SCREEN_HEIGHT * scale;

    Rectangle source = {
        0.0f, 0.0f,
        (float)SCREEN_WIDTH, -(float)SCREEN_HEIGHT
    };
    Rectangle destination = {
        ((float)GetScreenWidth() - drawWidth) / 2.0f,
        ((float)GetScreenHeight() - drawHeight) / 2.0f,
        drawWidth, drawHeight
    };

    DrawTexturePro(gameTarget.texture, source, destination,
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
  }

  UnloadTexture(titleImgtex);
  UnloadTexture(menuImgtex);
  UnloadTexture(logotex);
  UnloadRenderTexture(gameTarget);
  UnloadTexture(game->player->texture);
  free_game(game);
  UnloadAudio(&audio);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
