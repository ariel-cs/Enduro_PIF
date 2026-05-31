#include "raylib.h"
#include "config.h"
#include "game.h"
#include "player.h"
#include "track.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "audio.h"
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
        Rectangle dest1 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        Rectangle src1 = { 0, 0, titleImgtex.width, titleImgtex.height };
        DrawTexturePro(titleImgtex, src1, dest1, (Vector2){0,0}, 0.0f, WHITE);

        DrawText("Enduro", 52, 82, 80, BLACK);
        DrawText("Enduro", 50, 80, 80, WHITE);
        DrawText("PRESSIONE ENTER", 532, 522, 22, BLACK);
        DrawText("PRESSIONE ENTER", 530, 520, 22, YELLOW);
    }
    else if (game->current_state == STATE_MENU) {
        ClearBackground(BLACK);

        Rectangle dest = { 0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT};
        Rectangle src = { 0, 0, menuImgtex.width, menuImgtex.height };
        DrawTexturePro(menuImgtex, src, dest, (Vector2){0,0}, 0.0f, WHITE);

        DrawText("MENU PRINCIPAL", 450, 150, 30, RED);
        Color colorOpt0 = (menuSelectdOption == 0) ? YELLOW : WHITE;
        Color colorOpt1 = (menuSelectdOption == 1) ? YELLOW : WHITE;
        Color colorOpt2 = (menuSelectdOption == 2) ? YELLOW : GRAY;
        DrawText(TextFormat("%s INICIAR CORRIDA", (menuSelectdOption == 0) ? ">" : " "), 450, 280, 22, colorOpt0);
        DrawText(TextFormat("%s VOLUME  < %3d%% >", (menuSelectdOption == 1) ? ">" : " ",
                            (int)(audio.masterVolume * 100.0f)), 450, 340, 22, colorOpt1);
        DrawText(TextFormat("%s SAIR DO JOGO", (menuSelectdOption == 2) ? ">" : " "), 450, 400, 22, colorOpt2);

    }
    else if (game->current_state == STATE_PLAYING) {
        ClearBackground(RAYWHITE);
        DrawTrack(game->track,game->player);
        DrawPlayer(game->player);
        DrawEnemies(game->enemies, game->player, game->track);
#if DEBUG_HITBOXES
        DrawHitboxes(game->player, game->enemies, game->track);
#endif

        int faltam = game->cars_to_pass - game->cars_passed_today;
        if (faltam < 0) faltam = 0;
        int minutos = (int)(game->day_timer) / 60;
        int segundos = (int)(game->day_timer) % 60;

        DrawText(TextFormat("KM/H: %.0f", game->player->speed * 220.0f), 20, 20, 20, BLACK);
        DrawText(TextFormat("DIST: %.0f M", game->player->z), 20, 50, 20, BLACK);

        DrawText(TextFormat("TEMPO: %02d:%02d", minutos, segundos), 600, 20, 20, BLACK);
        DrawText(TextFormat("PONTOS: %06d", game->score), 600, 50, 20, BLACK);
        DrawText(TextFormat("DIA: %d", game->day), 600, 80, 20, BLACK);

        if (faltam > 0) {
            DrawText(TextFormat("FALTAM: %d", faltam), 320, 20, 30, RED);
        }
        else {
            DrawText(TextFormat("CLASSIFICADO!"), 320, 20, 30, DARKGREEN);
        }
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
