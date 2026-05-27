#include "raylib.h"
#include "config.h"
#include "game.h"
#include "player.h"
#include "track.h"
#include "enemy.h"

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Enduro");
  SetTargetFPS(60);

  Image img = LoadImage("LOGO.png");
  ImageResize(&img, 294, 110);
  Texture2D logotex = LoadTextureFromImage(img);
  UnloadImage(img);
  SetTextureFilter(logotex, TEXTURE_FILTER_POINT);

  int menuSelectdOption = 0;
  GameState *game = init_game();

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
            if (menuSelectdOption == 0) change_state(game, STATE_PLAYING);
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
        DrawText(TextFormat("KM/H: %.0f", game->player->speed * 220.0f), 20, 20, 20, BLACK);
        DrawText(TextFormat("DISTANCIA: %.0f M", game->player->z), 20, 50, 20, BLACK);
        DrawText(TextFormat("PONTOS: %d", game->cars_passed_total), 20, 80, 20, BLACK);
    }
    EndDrawing();
  }

  UnloadTexture(logotex);
  free_game(game);
  CloseWindow();
  return 0;
}
