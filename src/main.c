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

  int menuSelectdOption = 0;
  GameState *game = init_game();
  if (!game) {
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

    if(game->current_state == STATE_TITLE && IsKeyPressed(KEY_ENTER)){
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

    BeginDrawing();
    if (game->current_state == STATE_LOGO){
        ClearBackground(BLACK);
        int logoX = (SCREEN_WIDTH / 2) - (logotex.width / 2);
        int logoY = (SCREEN_HEIGHT / 2) - (logotex.height / 2);
        DrawTexture(logotex, logoX, logoY, WHITE);
        DrawText("APRESENTA", 330, 350, 24, GRAY);
    }
    else if (game->current_state == STATE_TITLE) {
        ClearBackground(BLACK);
        DrawText("Enduro", 340, 280, 40, WHITE);
        DrawText("PRESSIONE ENTER", 340, 320, 16, GRAY);
    }
    else if (game->current_state == STATE_MENU) {
        ClearBackground(BLACK);
        DrawText("MENU PRINCIPAL", 280, 150, 30, RED);
        Color colorOpt0 = (menuSelectdOption == 0) ? YELLOW : WHITE;
        Color colorOpt1 = (menuSelectdOption == 1) ? YELLOW : WHITE;
        Color colorOpt2 = (menuSelectdOption == 2) ? YELLOW : GRAY;
        DrawText(TextFormat("%s INICIAR CORRIDA", (menuSelectdOption == 0) ? ">" : " "), 280, 280, 22, colorOpt0);
        DrawText(TextFormat("%s VOLUME  < %3d%% >", (menuSelectdOption == 1) ? ">" : " ",
                            (int)(audio.masterVolume * 100.0f)), 280, 340, 22, colorOpt1);
        DrawText(TextFormat("%s SAIR DO JOGO", (menuSelectdOption == 2) ? ">" : " "), 280, 400, 22, colorOpt2);
    }
    else if (game->current_state == STATE_PLAYING) {
        ClearBackground(RAYWHITE);
        DrawTrack(game->track,game->player);
        DrawPlayer(game->player);
        DrawEnemies(game->enemies, game->player, game->track);
#if DEBUG_HITBOXES
        DrawHitboxes(game->player, game->enemies, game->track);
#endif

        DrawHUD(game);
    }
    else if (game->current_state == STATE_GAME_OVER) {
        ClearBackground(BLACK);
        scores_loaded = false;
        DrawText("FIM DE JOGO", 280, 150, 40, RED);
        DrawText(TextFormat("PONTUAÇÃO FINAL: %d", game->score), 250, 220, 20, WHITE);
        DrawText("DIGITE SEU NOME (Max 9 letras):", 220, 280, 20, RED);
        DrawText(game->player_name, 350, 330, 30, YELLOW);

        if ((int)(GetTime() * 2) % 2 == 0 && game->name_length < 9) {
            DrawText("_", 350 + MeasureText(game->player_name, 30), 330, 30, YELLOW);
        }
        DrawText("PRESSIONE ENTER PARA SALVAR", 230, 420, 20, DARKGRAY);
    }
    else if (game->current_state == STATE_TOP_SCORES) {
        ClearBackground(BLACK);
        DrawText("RANKING ENDURO", 250, 80, 35, GOLD);

        if (!scores_loaded) {
            top_scores_count = LoadTopScores(top_scores, 5);
            scores_loaded = true;
        }

        if (top_scores_count == 0) {
            DrawText("NENHUM SCORE REGISTRADO", 200, 200, 20, GRAY);
        }
        else {
            for (int i = 0; i < top_scores_count; i++) {
                int posY = 180 + (i * 40);
                DrawText(TextFormat("%d", i + 1), 260, posY, 25, WHITE);
                DrawText(top_scores[i].name, 300, posY, 25, SKYBLUE);
                DrawText(TextFormat("%06d", top_scores[i].score), 480, posY, 25, YELLOW);
            }
        }
        DrawText("PRESSIONE ENTER PARA VOLTAR", 200, 500, 20, DARKGRAY);
    }
    EndDrawing();
  }

  UnloadTexture(logotex);
  UnloadTexture(game->player->texture);
  free_game(game);
  UnloadAudio(&audio);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
