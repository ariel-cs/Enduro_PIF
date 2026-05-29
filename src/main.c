#include "raylib.h"
#include "config.h"
#include "game.h"
#include "player.h"
#include "track.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include <stdbool.h>

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Enduro");
  SetTargetFPS(60);

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

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if(game->current_state == STATE_TITLE && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_MENU);
    }
    else if (game->current_state == STATE_MENU) {
        if (IsKeyPressed(KEY_DOWN)|| IsKeyPressed(KEY_UP)) {
            menuSelectdOption = !menuSelectdOption;
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
            else break;
        }
    }

    update_game(game, dt);

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
        Color colorOpt1 = (menuSelectdOption == 0) ? YELLOW : WHITE;
        Color colorOpt2 = (menuSelectdOption == 1) ? YELLOW : GRAY;
        DrawText(TextFormat("%s INICIAR CORRIDA", (menuSelectdOption == 0) ? ">" : " "), 280, 280, 22, colorOpt1);
        DrawText(TextFormat("%s SAIR DO JOGO", (menuSelectdOption == 1) ? ">" : " "), 280, 340, 22, colorOpt2);
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
            DrawText(TextFormat("FALTAM: %d", faltam), 350, 20, 30, RED);
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

  UnloadTexture(logotex);
  UnloadTexture(game->player->texture);
  free_game(game);
  CloseWindow();
  return 0;
}
